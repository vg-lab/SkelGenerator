//
// Created by ivelascog on 13/04/18.
//

#include "Neuron.h"
#include "../VRMLReader.h"
#include "Segment.h"
#include "../Types.h"
#include <eigen3/Eigen/Dense>
#include <stack>
#include <set>
#include <QPixelFormat>
#include <iostream>


namespace skelgenerator {

    Neuron::Neuron(std::string &apiFile, std::vector<std::string> &basalFiles, int connectionThreshold) {
        this->connectionThreshold = connectionThreshold;
        /*auto apiDendrite = VRMLReader::readVrmlApical(apiFile);

        std::vector<TDendrite> basalDendrites;
        for (const auto &basalFile : basalFiles) {
            auto thisBasal = VRMLReader::readBasalFile(basalFile);
            basalDendrites.insert(basalDendrites.end(), thisBasal.begin(), thisBasal.end());
        }

        auto apiSegments = generateSegments(apiDendrite);
        std::vector<std::vector<Segment *>> basalsSegments;
         for (const auto &basalDedrite: basalDendrites) {
            basalsSegments.push_back(generateSegments(basalDedrite));
        } */

        auto apiDendrite = VRMLReader::readBasalFile(basalFiles[0])[3];
        //auto apiDendrite = VRMLReader::readVrmlApical(apiFile);
        auto apiSegments = generateSegments(apiDendrite);


        Dendrite apiDendriteSkel{};



        apiDendriteSkel.setDendrite(computeDendrite(apiSegments));
        apiDendriteSkel.setDendtype(APICAL);
        this->apical = apiDendriteSkel;

    }

    std::vector<Segment *> Neuron::generateSegments(TDendrite dendrite) {
        std::vector<Segment *> segments;
        for (const auto &segment : dendrite.segments) {
            segments.push_back(getSegment(segment));
        }

        return segments;

    }

    Segment *Neuron::getSegment(const TSegment &segment) {
        auto segmentSkel = new Segment(segment.nombre);
        for (int cir = 0; cir < segment.nCircles; cir++) {
            auto medio = Eigen::Vector3f(0, 0, 0);
            for (int i = 0; i < 17; i++) {
                medio += segment.points[cir * 17 + i];
            }
            medio = medio/17;
            float radius = (medio - segment.points[cir * 17 + 1]).norm();
            SamplePoint samplePoint(medio, radius);
            segmentSkel->addPoint(samplePoint);
        }
        return segmentSkel;
    }

    SubDendrite *Neuron::computeDendrite(std::vector<Segment *> segments) {
        std::cout << "Parsing Skel\n" << std::flush;
        std::set<Segment *> reaminSegments;
        for (int i = 0; i < segments.size(); i++) {
            reaminSegments.insert(segments[i]);
        }

        return computeSubDendrite(segments[0], reaminSegments);

    }

    SubDendrite *Neuron::computeSubDendrite(Segment *segment, std::set<Segment *> &reamingSegments) {
        std::cout << "Processing segment: " << segment->getName() << "\n" << std::flush;
        reamingSegments.erase(segment);
        std::vector<TConn> conns;
        for (auto anotherSegment : reamingSegments) {
            float minDistance = 1000;
            int minPoint1 = 0;
            int minPoint2 = 0;
            for (int i = 0; i < segment->size(); i++) {
                for (int j = 0; j < anotherSegment->size(); j++) {
                    float dist = ((*segment)[i].getPoint() - (*anotherSegment)[j].getPoint()).norm();

                    if (dist < minDistance) {
                        minDistance = dist;
                        minPoint1 = i;
                        minPoint2 = j;
                    }
                }
            }

            if (minDistance < connectionThreshold) {
                TConn conn{segment, minPoint1, anotherSegment, minPoint2};
                conns.push_back(conn);
            }
        }

        //TODO dar la vuelta ramas invertidas;

        if (conns.empty()) {
            return new SubDendrite(segment);
        } else {

            bool isAllConnsInFinal = true;
            TConn *firstCoon;
            int min = segment->size();
            for (auto &coon : conns) {
                isAllConnsInFinal = coon.p1 == segment->size() - 1;
                if (coon.p1 < min) {
                    firstCoon = &coon;
                }
            }

            if (isAllConnsInFinal) { // Es una Seccion

                if (conns.size() == 1) { // Si solo tiene una conexion se unen el segmento actual con el encontrado
                    auto con = conns[0];
                    auto unionSegment = Segment::unionSegment(segment, con.segment2);
                    reamingSegments.erase(segment);
                    reamingSegments.erase(con.segment2);
                    return computeSubDendrite(unionSegment, reamingSegments);
                } else if (conns.size() == 2) { //Seccion perfecta con 2 ramificaciones
                    auto con1 = conns[0];
                    auto con2 = conns[1];
                    auto subDendrite = new SubDendrite(segment);
                    reamingSegments.erase(con1.segment2);
                    reamingSegments.erase(con2.segment2);
                    subDendrite->setRamification1(computeSubDendrite(con1.segment2, reamingSegments));
                    subDendrite->setRamification1(computeSubDendrite(con2.segment2, reamingSegments));
                    return subDendrite;
                } else { //Mas de dos ramificaciones
                    //TODO Cuidado con ramificaciones de mas de 2 dendritas
                    std::cout << "ramificacion de " << conns.size() << " Dendrites" << std::endl;
                }


            } else {
                auto segmentSplit = segment->split(firstCoon->p1);
                auto segment1 = std::get<0>(segmentSplit);
                auto segment2 = std::get<1>(segmentSplit);

                reamingSegments.erase(firstCoon->segment2);
                auto subDendrite = new SubDendrite(segment1);
                subDendrite->setRamification1(computeSubDendrite(segment2, reamingSegments));
                subDendrite->setRamification2(computeSubDendrite(firstCoon->segment2, reamingSegments));
                return subDendrite;
            }
        }
    }




    std::string Neuron::to_asc() {
        std::string tab;
        std::stringstream ss;
        tab +="\t";
        ss << this->apical.to_asc(tab);
        return ss.str();
    }
}











