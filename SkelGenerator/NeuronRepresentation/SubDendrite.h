//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SUBDENDRITE_H
#define SKELGENERATOR_SUBDENDRITE_H

#include "Segment.h"
#include "SubDendrite.h"

namespace skelgenerator {
    class SubDendrite {
        Segment segment;
        SubDendrite* ramification1;
        SubDendrite* ramification2;

    public:
        friend std::ostream &operator<<(std::ostream &Str, const SubDendrite& dendrite);
    };
}


#endif //SKELGENERATOR_SUBDENDRITE_H
