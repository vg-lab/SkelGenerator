//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SUBDENDRITE_H
#define SKELGENERATOR_SUBDENDRITE_H

#include "Segment.h"
#include "SubDendrite.h"
#include <ostream>

namespace skelgenerator {
    class SubDendrite {
        Segment * segment;
        SubDendrite* ramification1;
        SubDendrite* ramification2;

    public:
        SubDendrite(Segment* segment);


        void setRamification1(SubDendrite *ramification1);

        void setRamification2(SubDendrite *ramification2);

        std::string to_asc(std::string tab);

    };
}


#endif //SKELGENERATOR_SUBDENDRITE_H
