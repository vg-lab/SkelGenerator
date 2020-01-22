//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "Dendrite.h"
namespace skelgenerator {



    void Dendrite::setDendtype(DENDTYPE dendtype_) {
            Dendrite::dendtype = dendtype_;
    }

    void Dendrite::setDendrite(SubDendrite *dendrite_) {
        Dendrite::dendrite = dendrite_;
    }

    std::string Dendrite::to_asc(std::string tab, int init) {
        std::stringstream ss;
        std::string color = this->dendtype==APICAL ? "Green": "Blue";
        std::string type = this->dendtype == APICAL ? "Apical" : "Dendrite";
        ss << tab << " (\t(Color " << color << ")" << std::endl;
        ss << tab << "(" << type << ")" << std::endl;
        ss << this->dendrite->to_asc(tab, init);
        ss << tab << ") ; End of Tree" << std::endl;
        return ss.str();
    }

    SubDendrite *Dendrite::getDendrite() const {
        return dendrite;
    }

    std::string Dendrite::to_swc(int &counter, bool spines) {
        int type = this->dendtype == APICAL ? 4:3;
        return this->dendrite->to_swc(counter, 1, type,spines,3);
    }

    void Dendrite::removeDuplication(float threshold) {
        dendrite->removeDuplicates(threshold);
    }

    void Dendrite::improveInit() {
        Section* sec = this->dendrite->getSec();
        int i = 0;
        float r1 = 1;
        float r2 = 0.1;
        while (!(r1/r2 > 0.8f && r1/r2 < 1.2f) && i < sec->size()) {
            i++;
            r1 = (*sec)[i]->getRadius();
            r2 = (*sec)[i - 1]->getRadius();
        }

        this->dendrite->getSec()->trim(i);
    }
}


