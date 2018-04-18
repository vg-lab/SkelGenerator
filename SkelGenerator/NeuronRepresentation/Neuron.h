//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_NEURON_H
#define SKELGENERATOR_NEURON_H

#include <set>
#include "SamplePoint.h"
#include "Dendrite.h"
#include "../Types.h"

namespace skelgenerator {
    class Neuron {
        std::vector<SamplePoint> soma;
        Dendrite apical;
        std::vector<Dendrite> basals;
        float connectionThreshold;

    public:
        Neuron(std::string& apiFile,std::vector<std::string>& basalFiles, int connectionThreshold = 3);

        std::string to_asc();

    private:
        std::vector<Segment *> generateSegments(TDendrite dendrite);

        Segment * getSegment(const TSegment &segment);

        SubDendrite* computeSubDendrite(Segment* segment, int initPoint,std::set<Segment*>& reamingSegments);


        SubDendrite * computeDendrite(std::vector<Segment *> segments);


    };
}


#endif //SKELGENERATOR_NEURON_H
