//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "Segment.h"


namespace skelgenerator {


    Segment::Segment(const std::string &name) {
        this->name = name;

    }

    void Segment::addPoint(SamplePoint &samplePoint) {
        this->points.push_back(samplePoint);
    }

    int Segment::size() {
        return static_cast<int>(points.size());
    }

    std::tuple<Segment *, Segment *> Segment::split(int point) {
        auto segment1 = new Segment(this->name + "-1");
        auto segment2 = new Segment(this->name + "-2");
        for (int i = 0; i <= point; i++) {
            segment1->addPoint(points[i]);
        }

        for (int i = point + 1; i < this->size(); i++) {
            segment2->addPoint(points[i]);
        }

        return std::make_tuple(segment1, segment2);
    }

    const std::string &Segment::getName() const {
        return name;
    }

    std::string Segment::to_asc(std::string tab) {
        std::stringstream ss;
        int i =1 ;
        for (const SamplePoint &point:this->points) {
            ss << point.to_asc(tab) <<";  " << i << std::endl;
            i++;
        }
        return ss.str();
    }

    Segment *Segment::unionSegment(Segment *segment1, Segment* segment2) {
        auto resultSegment = new Segment(segment1->getName() + "+" +  segment2->getName());
        for (const auto &point :segment1->points) {
            resultSegment->points.push_back(point);
        }

        for (const auto &point :segment2->points) {
            resultSegment->points.push_back(point);
        }
        return resultSegment;
    }
}


