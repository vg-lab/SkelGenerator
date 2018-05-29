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

        Section *getSec() const;

        SubDendrite *getRamification1() const;

        SubDendrite *getRamification2() const;


        void setRamification1(SubDendrite *ramification1);

        void setRamification2(SubDendrite *ramification2);

        std::string to_asc(std::string tab);

        std::tuple<std::string,std::string> to_neuronice(int& counter,int parent,int type);

    };
}


#endif //SKELGENERATOR_SUBDENDRITE_H
