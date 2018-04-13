//
// Created by ivelascog on 13/04/18.
//

#include "Segment.h"


namespace skelgenerator {

     std::ostream &operator<<(std::ostream &Str,const Segment& segment) {
        for(const SamplePoint &point:segment.points){
            Str << point <<std::endl;
        }
        return Str;
    }
}
