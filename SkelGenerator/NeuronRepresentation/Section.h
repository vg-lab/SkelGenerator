//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SECTION_H
#define SKELGENERATOR_SECTION_H


#include <vector>
#include "SamplePoint.h"

namespace skelgenerator {

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


        void reverse();

        void trim(int i);

        void addPoint(Eigen::Matrix<float, 3, 1> point, float radius);
    };

}


#endif //SKELGENERATOR_SECTION_H
