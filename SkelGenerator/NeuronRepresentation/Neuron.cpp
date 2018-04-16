//
// Created by ivelascog on 13/04/18.
//

#include "Neuron.h"
#include "../VRMLReader.h"
#include "Segment.h"
#include "../Types.h"
#include <eigen3/Eigen/Dense>


namespace skelgenerator {

    Neuron::Neuron(std::string& apiFile, std::vector<std::string>& basalFiles) {
        auto apiDendrite = VRMLReader::readVrmlApical(apiFile);
        std::vector<TDendrite> basalDendrites;
        for (const auto& basalFile : basalFiles) {
            auto thisBasal = VRMLReader::readBasalFile(basalFile);
            basalDendrites.insert(basalDendrites.end(),thisBasal.begin(),thisBasal.end());
        }

        auto apiSegments = generateSegments(apiDendrite);
    }

    std::vector<Segment> Neuron::generateSegments(TDendrite dendrite) {
        std::vector<Segment> segments;
        for (const auto& segment : dendrite.segments) {
            segments.push_back(getSegment(segment));
        }

    }

    Segment Neuron::getSegment(const TSegment& segment) {
        Segment segmentSkel(segment.nombre);
        for(int cir=0;cir< segment.nCircles;cir++) {
            auto medio = Eigen::Vector3f(0,0,0);
            for (int i=0; i< 17; i++){
                medio += segment.points[cir*17 +i];
            }
            float radius = (medio - segment.points[cir * 17 +1]).norm();
            SamplePoint samplePoint(medio,radius);
            segmentSkel.addPoint(samplePoint);
        }
        return segmentSkel;
    }


}
