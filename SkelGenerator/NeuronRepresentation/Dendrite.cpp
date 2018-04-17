//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "Dendrite.h"
namespace skelgenerator {



    void Dendrite::setDendtype(DENDTYPE dendtype) {
            Dendrite::dendtype = dendtype;
    }

    void Dendrite::setDendrite(SubDendrite *dendrite) {
        Dendrite::dendrite = dendrite;
    }

    std::string Dendrite::to_asc(std::string tab) {
        std::stringstream ss;
        std::string color = this->dendtype==APICAL ? "Green": "Blue";
        std::string type = this->dendtype == APICAL ? "Apical" : "Dendrite";
        ss << tab << " (\t(Color " << color << ")" << std::endl;
        ss << tab << "(" << type << ")" << std::endl;
        ss << this->dendrite->to_asc(tab);
        ss << tab << ") ; End of Tree";
        return ss.str();
    }
}


