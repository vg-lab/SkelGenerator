//
// Created by ivelascog on 13/04/18.
//

#include "Neuron.h"
#include "../VRMLReader.h"
#include "Section.h"
#include "../Types.h"
#include "Spine.h"
#include <eigen3/Eigen/Dense>
#include <stack>
#include <set>
#include <unordered_set>
#include <iostream>


namespace skelgenerator {

    Neuron::Neuron(std::string &apiFile, std::vector<std::string> &basalFiles, int connectionThreshold) {
        std::cout << "Conection Treshold " << connectionThreshold << std::endl;
        this->connectionThreshold = connectionThreshold;
        this->apical = nullptr;
        TDendrite apiDendrite = {};
        if (!apiFile.empty()) {
            apiDendrite = VRMLReader::readVrmlApical(apiFile);
        }

        std::vector<TDendrite> basalDendrites;
        for (const auto &basalFile : basalFiles) {
            auto thisBasal = VRMLReader::readBasalFile(basalFile);
            basalDendrites.insert(basalDendrites.end(), thisBasal.begin(), thisBasal.end());
        }

        procesSkel(apiDendrite, basalDendrites);
        generateSoma();
        // procesSpines(apiDendrite, basalDendrites);

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
            medio = medio / 17;
            float radius = (medio - fragment.points[cir * 17 + 1]).norm();
            sectionSkel->addPoint(medio, radius);
        }
        return sectionSkel;
    }

    std::tuple<SubDendrite *, int> Neuron::computeDendrite(std::vector<Section *> fragments) {
        std::cout << "Parsing Skel\n" << std::flush;
        std::set<Section *> reaminFragments;
        for (int i = 0; i < fragments.size(); i++) {
            reaminFragments.insert(fragments[i]);
        }

        auto subDendrite = computeSubDendrite(fragments[0], 0, reaminFragments);

        return std::make_tuple(subDendrite, reaminFragments.size());;

    }

    SubDendrite *Neuron::computeSubDendrite(Section *fragment, int initPoint, std::set<Section *> &reamingFragments) {
        std::cout << "Processing fragment: " << fragment->getName() << "\n" << std::flush;
        reamingFragments.erase(fragment);
        std::vector<TConn> conns;
        for (auto anotherFragment : reamingFragments) {
            float minDistance = 1000;
            int minPoint1 = 0;
            int minPoint2 = 0;
            for (int i = 0; i < fragment->size(); i++) {
                for (int j = 0; j < anotherFragment->size(); j++) {
                    auto p1 = (*fragment)[i]->getPoint();
                    auto p2 = (*anotherFragment)[j]->getPoint();
                    float dist = (p1 - p2).norm();

                    if (dist < minDistance) {
                        minDistance = dist;
                        minPoint1 = i;
                        minPoint2 = j;
                    }
                }
            }

            if (minDistance < connectionThreshold) {
                if (fragment->getName() == "FilamentSegment600000013") {
                    std::cout << "    ";
                }
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
        for (auto conn = conns.begin(); conn < conns.end(); conn++) {
            conn->p1 -= initPoint;
            /* Si alguna conexion es negativa indica que se ha realizado una mala conexion de segmentos anteriores por
             * un connectionTreshold demasiado elevado, se opta por eliminar las conexiones problematicas y proseguir
             * con una solucion , Notese que estas conexiones eliminadas pueden volver a ser conectadas por otros fragmentos*/
            if (conn->p1 < 0) {
                conns.erase(conn);
            }
        }

        if (conns.empty()) {
            return new SubDendrite(fragment);
        } else {

            if (conns.size() == 2) { //Seccion perfecta con 2 ramificaciones
                auto con1 = conns[0];
                auto con2 = conns[1];
                int dist = abs(con1.p1 - con2.p1);
                if (dist < 2 && (con1.p1 == fragment->size() - 1 || con2.p1 == fragment->size() - 1)) {
                    std::cout << "Seccion" << std::endl;
                    auto subDendrite = new SubDendrite(fragment);
                    reamingFragments.erase(con1.fragment2);
                    reamingFragments.erase(con2.fragment2);
                    subDendrite->setRamification1(computeSubDendrite(con1.fragment2, con1.p2, reamingFragments));
                    subDendrite->setRamification2(computeSubDendrite(con2.fragment2, con2.p2, reamingFragments));
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


            if (conns.size() == 1 && (conns[0].p1 == fragment->size() - 1 || conns[0].p1 ==
                                                                             0)) { // Si solo tiene una conexion se unen el segmento actual con el encontrado
                auto con = conns[0];
                std::cout << "Segmento suelto " << con.p1 << " = " << fragment->size() - 1 << std::endl;
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
                    return computeSubDendrite(unionSegment, con.p2, reamingFragments);
                } else {
                    auto unionSegment = Section::unionSection(fragment, con.fragment2);
                    return computeSubDendrite(unionSegment, initPoint, reamingFragments);
                }
            }

            std::vector<TConn> connsInSamePoint;
            connsInSamePoint.push_back(*firstCoon);

            for (auto &con : conns) {
                if (&con != firstCoon) {
                    if (con.p1 == firstCoon->p1) {
                        connsInSamePoint.push_back(con);
                    }
                }
            }

            if (connsInSamePoint.size() > 1) {
                std::cout << "more than 1 conection in one point :" << connsInSamePoint.size() << std::endl;

                int splitPoint = std::max(1, firstCoon->p1 - static_cast<int>(connsInSamePoint.size()));

                auto segmentSplit = fragment->split(splitPoint);
                auto segment1 = std::get<0>(segmentSplit);
                auto segment2 = std::get<1>(segmentSplit);

                reamingFragments.erase(firstCoon->fragment2);
                auto subDendrite = new SubDendrite(segment1);
                subDendrite->setRamification1(computeSubDendrite(segment2, 0, reamingFragments));
                subDendrite->setRamification2(
                        computeSubDendrite(firstCoon->fragment2, firstCoon->p2, reamingFragments));
                return subDendrite;
            }

            auto segmentSplit = fragment->split(firstCoon->p1);
            auto segment1 = std::get<0>(segmentSplit);
            auto segment2 = std::get<1>(segmentSplit);


            reamingFragments.erase(firstCoon->fragment2);
            auto subDendrite = new SubDendrite(segment1);
            subDendrite->setRamification1(computeSubDendrite(segment2, 0, reamingFragments));
            subDendrite->setRamification2(computeSubDendrite(firstCoon->fragment2, firstCoon->p2, reamingFragments));
            return subDendrite;

        }
    }


    std::string Neuron::to_asc() {
        std::string tab;
        std::stringstream ss;
        ss << "(\"CellBody\"" << std::endl;
        tab += "\t";
        ss << tab << "(Color RGB (255, 128, 64))" << std::endl;
        ss << tab << "(CellBody)" << std::endl;
        for (auto& point : soma) {
            ss << point.to_asc(tab) << std::endl;
        }
        ss << ")" << std::endl;
        if (this->apical != nullptr)
            ss << this->apical->to_asc(tab);
        for (auto basal:this->basals) {
            ss << basal->to_asc(tab);
        }
        return ss.str();
    }

    void Neuron::procesSkel(const TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites) {
        int reamingFragments = 0;
        if (!apiDendrite.fragments.empty()) {
            auto apiFragments = generateFragments(apiDendrite);
            auto apiDendriteSkel = new Dendrite();

            auto resultApi = computeDendrite(apiFragments);
            apiDendriteSkel->setDendrite(std::get<0>(resultApi));
            apiDendriteSkel->setDendtype(APICAL);
            this->apical = apiDendriteSkel;

            reamingFragments += std::get<1>(resultApi);
        }

        std::vector<std::vector<Section *>> basalsFragments;
        for (const auto &basalDedrite: basalDendrites) {
            basalsFragments.push_back(generateFragments(basalDedrite));
        }


        for (const auto &basalFragments: basalsFragments) {
            auto basalDend = new Dendrite();
            auto result = computeDendrite(basalFragments);
            basalDend->setDendtype(BASAL);
            basalDend->setDendrite(std::get<0>(result));
            reamingFragments += std::get<1>(result);
            this->basals.push_back(basalDend);
        }
        this->reamingSegments = reamingFragments;
    }


    void Neuron::procesSpines(TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites) {
        if (apiDendrite.fragments.empty()) {
            auto apiSpines = generateSpines(apiDendrite);
            addSpines(apical, apiSpines);
        }

        std::vector<spineSet> basalsSpines;
        for (const auto &basal:basalDendrites) {
            std::cout << "-------------------------- Basal ------------------------" << std::endl;
            basalsSpines.push_back(generateSpines(basal));
        }


        for (int i = 0; i < basalsSpines.size(); i++) {
            addSpines(basals[i], basalsSpines[i]);
        }
    }


    spineSet Neuron::generateSpines(const TDendrite &dendrite) {
        spineSet spines;
        for (const auto &spine:dendrite.spines) {
            auto spineSkel = new Spine(spine.nombre);
            for (int cir = 0; cir < spine.nCircles; cir++) {
                auto medio = Eigen::Vector3f(0, 0, 0);
                for (int i = 0; i < 17; i++) {
                    medio += spine.points[cir * 17 + i];
                }
                medio = medio / 17;
                float radius = (medio - spine.points[cir * 17 + 1]).norm();
                spineSkel->addPoint(medio, radius);
            }
            spineSkel->calculatePoints();
            spines.insert(spineSkel);
        }

        return spines;

    }

    void Neuron::addSpines(Dendrite *dendrite, spineSet &spines) {
        std::vector<std::tuple<Spine *, Section *, int, float>> spinesToInsert;
        for (auto spine :spines) {
            auto result = getPosSpine(dendrite->getDendrite(), spine);
            auto sec = std::get<0>(result);
            auto pos = std::get<1>(result);
            auto min = std::get<2>(result);
            if (min < connectionThreshold) {
                if (pos == 0) {
                    sec->addPoint(spine, 1);
                } else {
                    sec->addPoint(spine, pos);
                }
            } else {
                std::cout << "Error espina no conectada" << std::endl;
            }
        }
    }


    std::tuple<Section *, int, float> Neuron::getPosSpine(SubDendrite *subDendrite, Spine *spine) {
        const auto &insertPoint = spine->getInsertPoint();
        auto sec = subDendrite->getSec();
        float min = 1000.0f;
        int mini = 0;
        for (int i = 0; i < sec->size(); i++) {
            auto skelSample = (*sec)[i];
            if (!skelSample->isSpine()) {
                float dist = (insertPoint - (*sec)[i]->getPoint()).norm();

                if (dist < min) {
                    min = dist;
                    mini = i;
                }
            }
        }

        if (subDendrite->getRamification1() == nullptr && subDendrite->getRamification1() == nullptr) {
            return std::make_tuple(sec, mini, min);
        } else {
            auto result1 = getPosSpine(subDendrite->getRamification1(), spine);
            auto result2 = getPosSpine(subDendrite->getRamification2(), spine);
            auto minDist1 = std::get<2>(result1);
            auto minDist2 = std::get<2>(result2);

            if (min <= minDist1 && min <= minDist2) {
                return std::make_tuple(sec, mini, min);
            } else if (minDist1 <= min && minDist1 <= minDist2) {
                return result1;
            } else if (minDist2 <= min && minDist2 <= minDist1) {
                return result2;
            }
        }
    }

    int Neuron::getReamingSegments() const {
        return reamingSegments;
    }

    int Neuron::getReamingSpines() const {
        return reamingSpines;
    }

    void Neuron::generateSoma() {
        Eigen::Vector3f somaBasal(0, 0, 0);


        for (auto &basal: this->basals) {
            auto sec = basal->getDendrite()->getSec();
            somaBasal += (*sec)[0]->getPoint();
        }

        auto Zplane = 0;

        auto somaRadius = 1000000;
        Eigen::Vector3f somaCenter(0,0,0);

        if (apical != nullptr) {
            auto secApical = apical->getDendrite()->getSec();
            Eigen::Vector3f somaApical = (*secApical)[0]->getPoint();
            Zplane += somaApical[2];

            somaCenter = (somaApical + somaBasal) / (1 + this->basals.size());

            if ((somaApical - somaCenter).norm() < somaRadius) {
                somaRadius = (somaApical - somaCenter).norm();
            }

        } else {
            somaCenter = somaBasal / this->basals.size();
        }

        for (auto &basal:this->basals) {
            Eigen::Vector3f firstPoint = (*basal->getDendrite()->getSec())[0]->getPoint();
            Zplane += firstPoint[2];
            if ((firstPoint - somaCenter).norm() < somaRadius)
                somaRadius = (firstPoint - somaCenter).norm();
        }

        if (apical != nullptr) {
            Zplane /= 1 + this->basals.size();
        } else {
            Zplane /= this->basals.size();
        }

        auto centerX = somaCenter[0];
        auto centerY = somaCenter[1];

        for (int i = 0; i<360; i+=10) {
            auto x = centerX + somaRadius * cos(i);
            auto y = centerY + somaRadius * sin(i);
            Eigen::Vector3f point (x,y,somaCenter[2]);
            SamplePoint pointr(point,0.15f);
            soma.push_back(pointr);
        }









    }

    void Neuron::to_neuronize(std::ostream skel, std::ostream spines) {
         skel <<

    }


}















