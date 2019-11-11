//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_NEURON_H
#define SKELGENERATOR_NEURON_H

#include <skelgenerator/api.h>

#include <set>
#include "VRMLReader.h"
#include "SamplePoint.h"
#include "Dendrite.h"
#include "Types.h"
#include "Spine.h"

namespace skelgenerator {
    /** Internally the class have Fragments in Section class until the skeleton is finished procesing.
     *  Once the skeleton is processed it only contains sections.
    **/

    class SKELGENERATOR_API Neuron {
        SamplePoint soma;
        Dendrite* apical;
        std::vector<Dendrite *> basals;
        float connectionThreshold;
        int reamingSegments;
        int reamingSpines;
        spineSet spines;
        std::vector<TSpineImaris> imarisSpines;


    private:
        bool incorrectConecctions;

    public:
        Neuron(std::string& apiFile,std::vector<std::string>& basalFiles,const std::string& imarisFile = std::string() , float connectionThreshold_ = 3);

        std::string to_asc();
        std::string to_swc(bool spines = false);
        void spines_to_obj_with_base(std::string dirPath);
        void spines_to_obj_without_base(std::string dirPath);
        void imarisSpinesToObj(std::string dirPath);

        bool isIncorrectConecctions() const;

        int getReamingSegments() const;

        int getReamingSpines() const;

        bool hasImarisSpines() const;

        const spineSet &getSpines() const;

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

        void removeDuplicates(float threshold = 0.75);


    };
}


#endif //SKELGENERATOR_NEURON_H
