//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_DENDRITE_H
#define SKELGENERATOR_DENDRITE_H


#include <vector>
#include "SamplePoint.h"
#include "Segment.h"
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

        std::string to_asc(std::string tab);


    };
}


#endif //SKELGENERATOR_DENDRITE_H
