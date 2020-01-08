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
        std::string apiFile, imarisFile;
        std::vector<std::string> basalFiles;
        SamplePoint soma;
        Dendrite* apical;
        std::vector<Dendrite *> basals;
        float connectionThreshold;
        int reamingSegments;
        int reamingSpines;
        int numDendrites;
        spineSet spines;
        std::vector<TSpineImaris> imarisSpines;
        std::vector<std::vector<SamplePoint>> longsSpines;
    private:
        bool incorrectConecctions;
    public:

        Neuron(const std::string& apiFile_,const  std::vector<std::string>& basalFiles_, const std::string& imarisFile_ = std::string(), const std::string& longsFile_ = std::string() , float connectionThreshold_ = 3);
        void addSpinesLongs (const std::string& longsFile_);
        std::string to_asc();
        std::string to_asc(const std::vector<std::vector<Eigen::Vector3f>>& contours,const Eigen::Vector3f& displacement = {0,0,0});

        std::string to_swc(bool spines = false);
        void spines_to_obj_with_base(std::string dirPath);
        void spines_to_obj_without_base(std::string dirPath);
        void imarisSpinesToObj(std::string dirPath);
        bool isIncorrectConecctions() const;

        int getReamingSegments() const;

        int getReamingSpines() const;

        const std::vector<TSpineImaris> &getImarisSpines() const;

        const std::string &getApiFile() const;

        const std::string &getImarisFile() const;

        const std::vector<std::string> &getBasalFiles() const;

        bool hasImarisSpines() const;

        bool hasFilamentSpines() const;

        const spineSet &getSpines() const;

        static std::pair<Eigen::Vector3d,Eigen::Vector3d> getBB(const TFragment& fragment);
        static float computeOverlap(const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB1,const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB2);



    private:
        std::vector<Section *> generateFragments(TDendrite dendrite);

        Section * getFragment(const TFragment &fragment);


        SubDendrite* computeSubDendrite(Section* fragment, int initPoint,std::set<Section*>& reamingFragments);


        std::tuple<SubDendrite *, int> computeDendrite(std::vector<Section *> fragments);


        void procesSkel(const TDendrite& apiDendrite,const std::vector<TDendrite>& basalDendrites);

        void generateSoma();

        void procesSpinesFilament(TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites);

        spineSet generateSpines(const TDendrite &dendrite);

        void addSpines(Dendrite *dendrite, spineSet &spines);

        std::tuple<Section *, int, float> getPosSpine(SubDendrite *subDendrite, Spine *spine);

        void removeDuplicates(float threshold = 0.75);

        void procesSpinesLongs();

        //static std::pair<Eigen::Vector3d,Eigen::Vector3d> getBB(const TFragment& fragment);

        //float computeOverlap(const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB1,const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB2);

    };
}


#endif //SKELGENERATOR_NEURON_H
