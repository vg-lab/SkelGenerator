//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_NEURON_H
#define SKELGENERATOR_NEURON_H

#include <set>
#include "SamplePoint.h"
#include "Dendrite.h"
#include "../Types.h"
#include "Spine.h"

namespace skelgenerator {
    /** Internally the class have Fragments in Section class until the skeleton is finished procesing.
     *  Once the skeleton is processed it only contains sections.
    **/

    class Neuron {
        SamplePoint soma;
        Dendrite* apical;
        std::vector<Dendrite *> basals;
        float connectionThreshold;
        int reamingSegments;
        int reamingSpines;
        spineSet spines;

    public:
        Neuron(std::string& apiFile,std::vector<std::string>& basalFiles, int connectionThreshold = 3);

        std::string to_asc();
        std::string to_swc(bool spines = false);
        void spines_to_obj(std::string dirPath);

        int getReamingSegments() const;

        int getReamingSpines() const;

    private:
        std::vector<Section *> generateFragments(TDendrite dendrite);

        Section * getFragment(const TFragment &fragment);


        SubDendrite* computeSubDendrite(Section* fragment, int initPoint,std::set<Section*>& reamingFragments);


        std::tuple<SubDendrite *, int> computeDendrite(std::vector<Section *> fragments);


        void procesSkel(const TDendrite& apiDendrite,const std::vector<TDendrite>& basalDendrites);

        void generateSoma();

        void procesSpines(TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites);

        spineSet generateSpines(const TDendrite &dendrite);

        void addSpines(Dendrite *dendrite, spineSet &spines);


        std::tuple<Section *, int, float> getPosSpine(SubDendrite *subDendrite, Spine *spine);

    };
}


#endif //SKELGENERATOR_NEURON_H
