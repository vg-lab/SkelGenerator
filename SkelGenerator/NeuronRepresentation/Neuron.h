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
    /** Internally the class have Fragments in Section class until the skeleton is finished procesing.
     *  Once the skeleton is processed it only contains sections.
    **/

    class Neuron {
        std::vector<SamplePoint> soma;
        Dendrite* apical;
        std::vector<Dendrite> basals;
        float connectionThreshold;

    public:
        Neuron(std::string& apiFile,std::vector<std::string>& basalFiles, int connectionThreshold = 3);

        std::string to_asc();

    private:
        std::vector<Section *> generateFragments(TDendrite dendrite);

        Section * getFragment(const TFragment &fragment);

        SubDendrite* computeSubDendrite(Section* fragment, int initPoint,std::set<Section*>& reamingFragments);


        SubDendrite * computeDendrite(std::vector<Section *> fragments);


        void procesSkel(const std::string& apifile, const std::vector<std::string>& basalFiles);
    };
}


#endif //SKELGENERATOR_NEURON_H
