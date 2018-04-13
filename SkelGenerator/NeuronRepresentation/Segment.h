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
        friend std::ostream& operator<<(std::ostream &Str,const Segment & segment);
    };

}


#endif //SKELGENERATOR_SEGMENT_H
