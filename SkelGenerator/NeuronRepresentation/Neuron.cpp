//
// Created by ivelascog on 13/04/18.
//

#include "Neuron.h"
#include "../VRMLReader.h"
#include "Section.h"
#include "../Types.h"
#include <eigen3/Eigen/Dense>
#include <stack>
#include <set>
#include <QPixelFormat>
#include <iostream>


namespace skelgenerator {

    Neuron::Neuron(std::string &apiFile, std::vector<std::string> &basalFiles, int connectionThreshold) {
        this->connectionThreshold = connectionThreshold;
        this->apical = nullptr;

        auto apiDendrite = VRMLReader::readVrmlApical(apiFile);

        std::vector<TDendrite> basalDendrites;
        for (const auto &basalFile : basalFiles) {
            auto thisBasal = VRMLReader::readBasalFile(basalFile);
            basalDendrites.insert(basalDendrites.end(), thisBasal.begin(), thisBasal.end());
        }

        procesSkel(apiDendrite,basalDendrites);

    }

    std::vector<Section *> Neuron::generateFragments(TDendrite dendrite) {
        std::vector<Section *> fragments;
        for (const auto &fragment : dendrite.fragments) {
            fragments.push_back(getFragment(fragment));
        }

        return fragments;

    }

    Section *Neuron::getFragment(const TFragment &fragment) {
        auto sectionSkel = new Section(fragment.nombre);
        for (int cir = 0; cir < fragment.nCircles; cir++) {
            auto medio = Eigen::Vector3f(0, 0, 0);
            for (int i = 0; i < 17; i++) {
                medio += fragment.points[cir * 17 + i];
            }
            medio = medio/17;
            float radius = (medio - fragment.points[cir * 17 + 1]).norm();
            SamplePoint samplePoint(medio, radius);
            sectionSkel->addPoint(samplePoint);
        }
        return sectionSkel;
    }

    SubDendrite *Neuron::computeDendrite(std::vector<Section *> fragments) {
        std::cout << "Parsing Skel\n" << std::flush;
        std::set<Section *> reaminFragments;
        for (int i = 0; i < fragments.size(); i++) {
            reaminFragments.insert(fragments[i]);
        }

        return computeSubDendrite(fragments[0],0, reaminFragments);

    }

    SubDendrite *Neuron::computeSubDendrite(Section *fragment,int initPoint, std::set<Section *>& reamingFragments) {
        std::cout << "Processing fragment: " << fragment->getName() << "\n" << std::flush;
        reamingFragments.erase(fragment);
        std::vector<TConn> conns;
        for (auto anotherFragment : reamingFragments) {
            float minDistance = 1000;
            int minPoint1 = 0;
            int minPoint2 = 0;
            for (int i = 0; i < fragment->size(); i++) {
                for (int j = 0; j < anotherFragment->size(); j++) {
                    float dist = ((*fragment)[i].getPoint() - (*anotherFragment)[j].getPoint()).norm();

                    if (dist < minDistance) {
                        minDistance = dist;
                        minPoint1 = i;
                        minPoint2 = j;
                    }
                }
            }

            if (minDistance < connectionThreshold) {
                TConn conn{fragment, minPoint1, anotherFragment, minPoint2};
                conns.push_back(conn);
            }
        }

        //Le damos la vuelta a fragmentos al reves
        if (initPoint > fragment->size() / 2) {
            std::cout << "Rama del reves" << std::endl;
            fragment->reverse();

            initPoint = (fragment->size() - initPoint);
            for (auto &conn : conns) {
                conn.p1 = (fragment->size() - conn.p1);
            }
        }

       fragment->trim(initPoint);
        for (auto& conn :conns) {
            conn.p1 -= initPoint;
        }

        if (conns.empty()) {
            return new SubDendrite(fragment);
        } else {

            if (fragment->getName() == "FilamentSegment600000012") {
                std::cout << "Encontrado";
            }
            if (conns.size() == 2) { //Seccion perfecta con 2 ramificaciones
                auto con1 = conns[0];
                auto con2 = conns[1];
                int dist = abs(con1.p1 - con2.p1);
                if (dist < 2 && (con1.p1 == fragment->size() - 1 || con2.p1 == fragment->size() - 1)) {
                    std::cout << "Seccion" << std::endl;
                    auto subDendrite = new SubDendrite(fragment);
                    reamingFragments.erase(con1.fragment2);
                    reamingFragments.erase(con2.fragment2);
                    subDendrite->setRamification1(computeSubDendrite(con1.fragment2,con1.p2, reamingFragments));
                    subDendrite->setRamification2(computeSubDendrite(con2.fragment2,con2.p2, reamingFragments));
                    return subDendrite;
                }
            }


            TConn *firstCoon;
            int min = fragment->size();
            for (auto &coon : conns) {
                if (coon.p1 < min) {
                    firstCoon = &coon;
                    min = coon.p1;
                }
            }


            if (conns.size() == 1 && (conns[0].p1 == fragment->size() -1 || conns[0].p1 == 0)) { // Si solo tiene una conexion se unen el segmento actual con el encontrado
                auto con = conns[0];
                std::cout << "Segmento suelto " << con.p1 << " = " << fragment->size()-1 << std::endl;
                reamingFragments.erase(fragment);
                reamingFragments.erase(con.fragment2);
                //Comprobamos que el segmento con el que vamos a unir no este del reves
                if (con.p2 > con.fragment2->size() / 2) {
                    std::cout << "Rama del reves" << std::endl;
                    con.fragment2->reverse();

                    con.p2 = (con.fragment2->size() - con.p2);
                }
                if (con.p1 == 0) {
                    auto unionSegment = Section::unionSection(con.fragment2, fragment);
                    return computeSubDendrite(unionSegment,con.p2, reamingFragments);
                } else {
                    auto unionSegment = Section::unionSection(fragment, con.fragment2);
                    return computeSubDendrite(unionSegment,initPoint, reamingFragments);
                }
            }

            std::vector<TConn> connsInSamePoint;
            connsInSamePoint.push_back(*firstCoon);

            for( auto& con : conns) {
                if (&con != firstCoon) {
                    if (con.p1 == firstCoon->p1){
                        connsInSamePoint.push_back(con);
                    }
                }
            }

            if (connsInSamePoint.size() > 1) {
                std::cout << "more than 1 conection in one point :"<< connsInSamePoint.size() <<std::endl;

                auto segmentSplit = fragment->split(firstCoon->p1- static_cast<int>(connsInSamePoint.size()));
                auto segment1 = std::get<0>(segmentSplit);
                auto segment2 = std::get<1>(segmentSplit);

                reamingFragments.erase(firstCoon->fragment2);
                auto subDendrite = new SubDendrite(segment1);
                subDendrite->setRamification1(computeSubDendrite(segment2,0, reamingFragments));
                subDendrite->setRamification2(computeSubDendrite(firstCoon->fragment2,firstCoon->p2, reamingFragments));
                return subDendrite;
            }

            auto segmentSplit = fragment->split(firstCoon->p1);
            auto segment1 = std::get<0>(segmentSplit);
            auto segment2 = std::get<1>(segmentSplit);



            reamingFragments.erase(firstCoon->fragment2);
            auto subDendrite = new SubDendrite(segment1);
            subDendrite->setRamification1(computeSubDendrite(segment2,0, reamingFragments));
            subDendrite->setRamification2(computeSubDendrite(firstCoon->fragment2,firstCoon->p2, reamingFragments));
            return subDendrite;

        }
    }




    std::string Neuron::to_asc() {
        std::string tab;
        std::stringstream ss;
        tab +="\t";
        if (this->apical!= nullptr)
        ss << this->apical->to_asc(tab);
        for (auto basal:this->basals) {
            ss << basal.to_asc(tab);
        }
        return ss.str();
    }

    void Neuron::procesSkel(const TDendrite& apiDendrite,const std::vector<TDendrite>& basalDendrites) {

        auto apiFragments = generateFragments(apiDendrite);
        std::vector<std::vector<Section *>> basalsFragments;
        for (const auto &basalDedrite: basalDendrites) {
            basalsFragments.push_back(generateFragments(basalDedrite));
        }
        auto apiDendriteSkel = new Dendrite();

        apiDendriteSkel->setDendrite(computeDendrite(apiFragments));
        apiDendriteSkel->setDendtype(APICAL);
        this->apical = apiDendriteSkel;

        for (const auto &basalFragments: basalsFragments) {
            Dendrite basalDend;
            basalDend.setDendtype(BASAL);
            basalDend.setDendrite(computeDendrite(basalFragments));
            this->basals.push_back(basalDend);
        }
    }

    void Neuron::procesSpines(const TDendrite& apiDendrite,const std::vector<TDendrite>& basalDendrites) {


    }
}











