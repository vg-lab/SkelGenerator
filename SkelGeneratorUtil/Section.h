//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SECTION_H
#define SKELGENERATOR_SECTION_H


#include <vector>
#include <boost/shared_ptr.hpp>
#include "SamplePoint.h"
#include "Spine.h"

namespace skelgenerator {

    typedef std::vector<std::shared_ptr<SamplePoint>>::iterator iterator;
    typedef std::vector<std::shared_ptr<SamplePoint>>::const_iterator const_iterator;

    class Section {
        std::vector<std::shared_ptr<SamplePoint>> points;
        std::string name;

    public:
        Section() = default;

        explicit Section(const std::string& name);

        void addPoint(std::shared_ptr<SamplePoint> &samplePoint);
        int size();

        void remove(int index);

        std::shared_ptr<SamplePoint> & operator[] (std::size_t idx) { return points[idx];}

        std::shared_ptr<SamplePoint> operator[] (std::size_t idx) const { return points[idx];}

        std::tuple<Section, Section> split(int i);

        const std::string &getName() const;

        std::string to_asc(std::string tab, int i);

        static Section unionSection(const Section& section1, const Section& section2);

        std::string to_swc(int &counter, int parent, int type, bool spines,int init = 0);


        void reverse();

        void trim(int i);

        void addPoint(Eigen::Vector3d point, float radius);

        void addPoint(std::shared_ptr<Spine> &samplePoint, int pos);

        iterator begin() { return points.begin(); }

        iterator end() { return points.end(); }

        const_iterator begin() const { return points.begin(); }

        const_iterator end() const { return points.end(); }

        bool operator<(const Section &rhs) const;


    };

}


#endif //SKELGENERATOR_SECTION_H
