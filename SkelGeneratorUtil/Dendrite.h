//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_DENDRITE_H
#define SKELGENERATOR_DENDRITE_H


#include <vector>
#include "SamplePoint.h"
#include "Section.h"
#include "SubDendrite.h"


namespace skelgenerator {

    typedef enum DENDTYPE {
        APICAL, BASAL
    };

    class Dendrite {
        SubDendrite * dendrite;
        DENDTYPE dendtype;
    public:
        void setDendrite(SubDendrite *dendrite);

        void setDendtype(DENDTYPE dendtype);

        std::string to_asc(std::string tab, int i);

        SubDendrite *getDendrite() const;

        std::string to_swc(int &counter, bool spines);

        void removeDuplication(int threshold);




    };
}


#endif //SKELGENERATOR_DENDRITE_H
