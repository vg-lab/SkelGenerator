//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SEGMENT_H
#define SKELGENERATOR_SEGMENT_H


#include <vector>
#include "SamplePoint.h"

namespace skelgenerator {

    class Segment {
        std::vector<SamplePoint> points;
        std::string name;

    public:
        Segment() = default;
        Segment(const std::string& name);
        void addPoint(SamplePoint& samplePoint);
        int size();

        SamplePoint& operator[] (std::size_t idx) { return points[idx];}

        const SamplePoint& operator[] (std::size_t idx) const { return points[idx];}

        std::tuple<Segment *,Segment *> split(int i);

        const std::string &getName() const;

        std::string to_asc(std::string tab);

        static Segment * unionSegment(Segment * segment1, Segment *segment2);


    };

}


#endif //SKELGENERATOR_SEGMENT_H
