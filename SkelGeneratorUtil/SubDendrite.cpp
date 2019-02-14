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

    std::string SubDendrite::to_asc(std::string tab, int init) {
        std::stringstream ss;
        tab +="\t";
        ss  << this->sec->to_asc(tab, init);
        if (this->ramification1 == nullptr && this->ramification2 == nullptr) {
            ss << tab << "Normal" <<std::endl;
       } else {
            ss << tab << "("<< std::endl;
            ss << this->ramification1->to_asc(tab, 0);
            ss << tab << "|" << std::endl;
            ss << this->ramification2->to_asc(tab, 0);
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

    std::string SubDendrite::to_swc(int &counter, int parent, int type, bool spines,int init) {
        std::stringstream ssSkel;
        std::stringstream ssSpines;
        ssSkel << this->getSec()->to_swc(counter, parent, type,spines,init);
        if (this->getRamification1() != nullptr && this->getRamification2() != nullptr) {
            int newParent = counter - 1;
            ssSkel << this->getRamification1()->to_swc(counter, newParent, type,spines);
            ssSkel << this->getRamification2()->to_swc(counter, newParent, type,spines);
        }

        return ssSkel.str();

    }

    void SubDendrite::removeDuplicates(float threshold) {
        if (this->getRamification1() != nullptr) {
            auto r1Sec = *(this->getRamification1()->getSec());
            for (const auto &point : *(this->getSec())) {
                for (int i = r1Sec.size() - 1; i >= 0; i--) {
                    if ((point->getPoint() - r1Sec[i]->getPoint()).norm() < threshold) {
                        r1Sec.remove(i);
                    }
                }
            }

            this->getRamification1()->removeDuplicates(threshold);
        }

        if (this->getRamification2() != nullptr) {
            auto r2Sec = *(this->getRamification2()->getSec());
            for (const auto &point : *(this->getSec())) {
                for (int i = r2Sec.size() - 1; i >= 0; i--) {
                    if ((point->getPoint() - r2Sec[i]->getPoint()).norm() < threshold) {
                        r2Sec.remove(i);
                    }
                }
            }

            this->getRamification2()->removeDuplicates(threshold);

        }







    }
}
