//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "SubDendrite.h"
namespace skelgenerator {

    void SubDendrite::setRamification1(SubDendrite *ramification1_) {
        SubDendrite::ramification1 = ramification1_;
    }

    void SubDendrite::setRamification2(SubDendrite *ramification2_) {
        SubDendrite::ramification2 = ramification2_;
    }

    SubDendrite::SubDendrite(Section section) {
        this->sec = section;
        this->ramification1 = nullptr;
        this->ramification2 = nullptr;

    }

    std::string SubDendrite::to_asc(std::string tab, int init) {
        std::stringstream ss;
        tab += "\t";
        ss << this->sec.to_asc(tab, init);
        if (this->ramification1 == nullptr && this->ramification2 == nullptr) {
            ss << tab << "Normal" << std::endl;
        } else {
            if (this->ramification1 != nullptr && this->ramification2 != nullptr) {
                ss << tab << "(" << std::endl;
                ss << this->ramification1->to_asc(tab, 0);
                ss << tab << "|" << std::endl;
                ss << this->ramification2->to_asc(tab, 0);
                ss << tab << ")  ; End of Split" << std::endl;
            } else if (this->ramification1 != nullptr) {
                ss << tab << "(" << std::endl;
                ss << this->ramification1->to_asc(tab,0);
                ss << tab << ")  ; End of Split" << std::endl;
            } else if (this->ramification2 != nullptr) {
                ss << tab << "(" << std::endl;
                ss << this->ramification2->to_asc(tab,0);
                ss << tab << ")  ; End of Split" << std::endl;
            }
        }
        return ss.str();
    }

    Section& SubDendrite::getSec() {
        return sec;
    }


    SubDendrite *SubDendrite::getRamification1() const {
        return ramification1;
    }

    SubDendrite *SubDendrite::getRamification2() const {
        return ramification2;
    }

    std::string SubDendrite::to_swc(int &counter, int parent, int type, bool spines, int init) {
        std::stringstream ssSkel;
        std::stringstream ssSpines;
        ssSkel << this->getSec().to_swc(counter, parent, type, spines, init);
        if (this->getRamification1() != nullptr && this->getRamification2() != nullptr) {
            int newParent = counter - 1;
            ssSkel << this->getRamification1()->to_swc(counter, newParent, type, spines);
            ssSkel << this->getRamification2()->to_swc(counter, newParent, type, spines);
        }

        return ssSkel.str();

    }

    void SubDendrite::removeDuplicates(float threshold) {
        if (this->getRamification1() != nullptr) {
            auto r1Sec = this->getRamification1()->getSec();
            for (const auto &point : this->getSec()) {
                for (int i = r1Sec.size() - 1; i >= 0; i--) {
                    float d = (point->getPoint() - r1Sec[i]->getPoint()).norm();
                    if (d < threshold && r1Sec.size() > 2) {
                        r1Sec.remove(i);
                    }
                }
            }

            if (r1Sec.size() == 0) {
                auto r1r1 = this->getRamification1()->getRamification1();
                auto r1r2 = this->getRamification1()->getRamification2();
                if (r1r1 == nullptr && r1r2 == nullptr) {
                    delete(this->ramification1);
                    this->setRamification1(nullptr);
                } else {
                    if (r1r1 != nullptr && r1r1->getSec().size() > 0) {
                        auto r1r1Sec = r1r1->getSec();
                        auto p = r1r1Sec[0]->getPoint();
                        auto r = r1r1Sec[0]->getRadius();
                        r1Sec.addPoint(p, r);
                        r1r1Sec.remove(0);
                    } else {
                        auto r1r2Sec = r1r2->getSec();
                        auto p = r1r2Sec[0]->getPoint();
                        auto r = r1r2Sec[0]->getRadius();
                        r1Sec.addPoint(p, r);
                        r1r2Sec.remove(0);
                    }
                }
            }
            if (this->getRamification1() != nullptr) {
                this->getRamification1()->removeDuplicates(threshold);
            }
        }

        if (this->getRamification2() != nullptr) {
            auto r2Sec = this->getRamification2()->getSec();
            for (const auto &point : this->getSec()) {
                for (int i = r2Sec.size() - 1; i >= 0; i--) {
                    if ((point->getPoint() - r2Sec[i]->getPoint()).norm() < threshold && r2Sec.size() > 2) {
                        r2Sec.remove(i);
                    }
                }
            }

            if (r2Sec.size() == 0) {
                auto r2r1 = this->getRamification2()->getRamification1();
                auto r2r2 = this->getRamification2()->getRamification2();
                if (r2r1 == nullptr && r2r2 == nullptr){
                    delete(this->ramification2);
                    this->setRamification2(nullptr);
                } else {
                    if (r2r1 != nullptr && r2r1->getSec().size() > 0) {
                        auto r2r1Sec = r2r1->getSec();
                        auto p = r2r1Sec[0]->getPoint();
                        auto r = r2r1Sec[0]->getRadius();
                        r2Sec.addPoint(p, r);
                        r2r1Sec.remove(0);
                    } else {
                        auto r2r2Sec = r2r2->getSec();
                        auto p = r2r2Sec[0]->getPoint();
                        auto r = r2r2Sec[0]->getRadius();
                        r2Sec.addPoint(p, r);
                        r2r2Sec.remove(0);
                    }
                }
            }

            if (this->getRamification2() != nullptr) {
                this->getRamification2()->removeDuplicates(threshold);
            }
        }
    }

    SubDendrite::~SubDendrite() {
        delete this->ramification1;
        delete this->ramification2;

    }
}

