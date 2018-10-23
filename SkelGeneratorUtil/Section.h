//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SECTION_H
#define SKELGENERATOR_SECTION_H


#include <vector>
#include "SamplePoint.h"
#include "Spine.h"

namespace skelgenerator {

    typedef std::vector<SamplePoint *>::iterator iterator;
    typedef std::vector<SamplePoint *>::const_iterator const_iterator;

    class Section {
        std::vector<SamplePoint*> points;
        std::string name;

    public:
        Section() = default;
        Section(const std::string& name);
        void addPoint(SamplePoint*& samplePoint);
        int size();

        SamplePoint*& operator[] (std::size_t idx) { return points[idx];}

        const SamplePoint* operator[] (std::size_t idx) const { return points[idx];}

        std::tuple<Section *,Section *> split(int i);

        const std::string &getName() const;

        std::string to_asc(std::string tab);

        static Section * unionSection(Section *section1, Section *section2);

        std::string to_swc(int &counter, int parent, int type, bool spines);


        void reverse();

        void trim(int i);

        void addPoint(Eigen::Vector3d point, float radius);

        void addPoint(Spine *&samplePoint, int pos);

        iterator begin() { return points.begin(); }

        iterator end() { return points.end(); }

        const_iterator begin() const { return points.begin(); }

        const_iterator end() const { return points.end(); }
    };

}


#endif //SKELGENERATOR_SECTION_H
