//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_NEURON_H
#define SKELGENERATOR_NEURON_H

#include "SamplePoint.h"
#include "Dendrite.h"
#include "../Types.h"

namespace skelgenerator {
    class Neuron {
        std::vector<SamplePoint> soma;
        std::vector<Dendrite> apical;
        std::vector<std::vector<Dendrite>> basals;

    public:
        Neuron(std::string& apiFile,std::vector<std::string>& basalFiles);

    private:
        std::vector<Segment> generateSegments(TDendrite dendrite);

        Segment getSegment(const TSegment &segment);
    };
}


#endif //SKELGENERATOR_NEURON_H
