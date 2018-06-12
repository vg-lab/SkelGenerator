//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "SubDendrite.h"
namespace skelgenerator{
    



    void SubDendrite::setRamification1(SubDendrite *ramification1) {
            SubDendrite::ramification1 = ramification1;
    }

    void SubDendrite::setRamification2(SubDendrite *ramification2) {
            SubDendrite::ramification2 = ramification2;
    }

    SubDendrite::SubDendrite(Section* section) {
        this->sec = section;
        this->ramification1 = nullptr;
        this->ramification2 = nullptr;

    }

    std::string SubDendrite::to_asc(std::string tab) {
        std::stringstream ss;
        tab +="\t";
        ss  << this->sec->to_asc(tab);
        if (this->ramification1 == nullptr && this->ramification2 == nullptr) {
            ss << tab << "Normal" <<std::endl;
       } else {
            ss << tab << "("<< std::endl;
            ss << this->ramification1->to_asc(tab);
            ss << tab << "|" << std::endl;
            ss << this->ramification2->to_asc(tab);
            ss << tab << ")  ; End of Split" << std::endl;

        }
        return ss.str();
    }

    Section *SubDendrite::getSec() const {
        return sec;
    }

    SubDendrite *SubDendrite::getRamification1() const {
        return ramification1;
    }

    SubDendrite *SubDendrite::getRamification2() const {
        return ramification2;
    }

    std::string SubDendrite::to_swc(int &counter, int parent, int type, bool spines) {
        std::stringstream ssSkel;
        std::stringstream ssSpines;
        ssSkel << this->getSec()->to_swc(counter, parent, type,spines);
        if (this->getRamification1() != nullptr && this->getRamification2() != nullptr) {
            int newParent = counter;
            ssSkel << this->getRamification1()->to_swc(counter, newParent, type,spines);
            ssSkel << this->getRamification2()->to_swc(counter, newParent, type,spines);
        }

        return ssSkel.str();

    }
}
