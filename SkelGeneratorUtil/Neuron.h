//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_NEURON_H
#define SKELGENERATOR_NEURON_H

#include <skelgenerator/api.h>

#include <set>
#include <QThreadPool>
#include "VRMLReader.h"
#include "SamplePoint.h"
#include "Dendrite.h"
#include "Types.h"
#include "Spine.h"
#include "Mesh.h"

namespace skelgenerator {
    /** Internally the class have Fragments in Section class until the skeleton is finished procesing.
     *  Once the skeleton is processed it only contains sections.
    **/

    class SKELGENERATOR_API Neuron {
        std::string _apiFile, _imarisFile;
        std::vector<std::string> _basalFiles;
        std::string _somaFile;
        std::vector<Section> _apiFragments;
        spineSet _apiSpines;
        std::vector<std::vector<Section>> _basalsFragments;
        std::vector<spineSet> _basalsSpines;
        int _segmentCounter;
        SamplePoint _soma;
        Mesh* _somaMesh;
        Dendrite* _apical;
        std::vector<Dendrite *> _basals;
        float _connectionThreshold;
        int _reamingSegments;
        int _reamingSpines;
        int _numDendrites;
        spineSet _spines;
        std::vector<TSpineImaris> _imarisSpines;
        std::vector<std::vector<SamplePoint>> _longsSpines;
        QThreadPool _pool;



    private:
        bool incorrectConecctions;
    public:
        struct OOBB {
            Eigen::Vector3d center;
            Eigen::Vector3d a0,a1,a2;
            double d0,d1,d2;
        };

        Neuron( const std::string& apiFile_,
                const std::vector<std::string>& basalFiles_,
                const std::string& somaFile_ = "", const std::string& imarisFile_ = "",
                const std::string& longsFile_ = "", float connectionThreshold_ = 3 );

        void reComputeSkel(float connectionThreshold);

        virtual ~Neuron();

        void addSpinesLongs (const std::string& longsFile_);
        void addImarisSpines( const std::string& imarisFile_);
        void clearImarisSpines();
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

        static Neuron::OOBB getBB(const TFragment& fragment);

        static bool collide(const Neuron::OOBB& BB1, const OOBB &BB2);

        bool hasSomaMesh() const;

        void exportSomaMesh(const std::string& filePath);



    private:
        std::vector<Section> generateFragments(TDendrite dendrite);

        Section getFragment(const TFragment &fragment);


        SubDendrite* computeSubDendrite(Section fragment, int initPoint,std::set<Section>& reamingFragments);


        std::tuple<SubDendrite *, int> computeDendrite(std::vector<Section> fragments);


        void procesSkel(float connectionThreshold);

        void generateSoma();

        void procesSpinesFilament();

        static spineSet generateSpines(const TDendrite &dendrite);

        void addSpines(Dendrite *dendrite, spineSet &spines);

        static std::tuple<Section *, int, float> getPosSpine(SubDendrite *subDendrite, const std::shared_ptr<Spine>& spine);

        void removeDuplicates(float threshold = 0.5f);

        void procesSpinesLongs();

        //static std::pair<Eigen::Vector3d,Eigen::Vector3d> getBB(const TFragment& fragment);

        //float computeOverlap(const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB1,const std::pair<Eigen::Vector3d,Eigen::Vector3d>& BB2);

        static void removeFragments(TDendrite& dendrite);

        static bool checkPointsInsideBB(const OOBB& oobb, const TFragment& fragment);

        static void exportFragmentAndBB(const OOBB &oobb, const TFragment &fragment, const std::string& prefixName);

        void improveInitialFragments();

        void forceTwoInitPoints();

        static bool checkPointsInsideMesh(const TFragment &cfragment, const TFragment &ifragment);

      void generateSomaMesh( );
    };
}


#endif //SKELGENERATOR_NEURON_H
