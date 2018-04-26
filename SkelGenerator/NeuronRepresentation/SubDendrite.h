//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SUBDENDRITE_H
#define SKELGENERATOR_SUBDENDRITE_H

#include "Section.h"
#include "SubDendrite.h"
#include <ostream>

namespace skelgenerator {
    class SubDendrite {
        Section * sec;
        SubDendrite* ramification1;
        SubDendrite* ramification2;

    public:
        SubDendrite(Section* section);


        void setRamification1(SubDendrite *ramification1);

        void setRamification2(SubDendrite *ramification2);

        std::string to_asc(std::string tab);

    };
}


#endif //SKELGENERATOR_SUBDENDRITE_H
