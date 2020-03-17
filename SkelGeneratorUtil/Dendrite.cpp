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
        Section sec = this->dendrite->getSec();
        int i = 0;
        float r1 = 1;
        float r2 = 0.1;
        while (!(r1/r2 > 0.8f && r1/r2 < 1.2f) && i < sec.size() - 1) {

            i++;
            r1 = sec[i]->getRadius();
            r2 = sec[i - 1]->getRadius();
        }

        if (i != sec.size()) {
            this->dendrite->getSec().trim(i);
        }
    }

    Dendrite::~Dendrite() {
        delete (this->dendrite);
    }

    void Dendrite::forceTwoInitPoints(){
        auto dend = this->dendrite;
        while (dend->getSec().size() < 2) {
            if (dend->getRamification1() != nullptr && dend->getRamification1()->getSec().size() > 0) {
                dend->getSec().addPoint(dend->getRamification1()->getSec()[0]);
                dend->getRamification1()->getSec().remove(0);
            } else if (dend->getRamification2() != nullptr && dend->getRamification2()->getSec().size() > 0) {
                dend->getSec().addPoint(dend->getRamification2()->getSec()[0]);
                dend->getRamification2()->getSec().remove(0);
            }
        }
    }
}


