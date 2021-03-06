//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "Section.h"


namespace skelgenerator {


    Section::Section(const std::string &name_) {
        this->name = name_;

    }

    void Section::addPoint(std::shared_ptr<SamplePoint> &samplePoint) {
        this->points.push_back(samplePoint);
    }

    int Section::size() const {
        return static_cast<int>(points.size());
    }

    std::tuple<Section, Section> Section::split(int point) {
        Section segment1 (this->name + "-1");
        Section segment2 (this->name + "-2");
        for (int i = 0; i < point; i++) {
            segment1.addPoint(points[i]);
        }

        for (int i = point; i < this->size(); i++) {
            segment2.addPoint(points[i]);
        }

        return std::make_tuple(segment1, segment2);
    }

    const std::string &Section::getName() const {
        return name;
    }

    std::string Section::to_asc(std::string tab, int init) {
        std::stringstream ss;
        int counter =1 ;
        if (this->size() == 0) {
            ss << tab  << "Empty Section: " << this->name;
            std::cerr << "[ERROR] Empty section founded, this is an error on SkelGenerator" << std::endl;
        }
        ss << tab << "; " << this->name << std::endl;
        for (size_t i = init;i< points.size(); i++) {
            auto point = points[i];
            ss << point->to_asc(tab) <<"\t;\t" << counter << std::endl;
            counter++;
        }
        return ss.str();
    }

    Section Section::unionSection(const Section& section1, const Section& section2) {
        Section resultSegment (section1.getName() + "+" +  section2.getName());
        for (const auto &point :section1.points) {
            resultSegment.points.push_back(point);
        }

        for (const auto &point :section2.points) {
            resultSegment.points.push_back(point);
        }
        return resultSegment;
    }

    void Section::reverse() {
        std::reverse(this->points.begin(),this->points.end());
    }

    void Section::trim(int i) {
        this->points.erase(this->points.begin(),this->points.begin()+i);

    }

    void Section::addPoint(Eigen::Vector3d point, float radius) {
        auto p = std::make_shared<SamplePoint>(point,radius);
        points.push_back(p);

    }

    void Section::addPoint(std::shared_ptr<Spine> &samplePoint, int pos) {
        this->points.insert(this->points.begin() + pos, samplePoint);
    }

    std::string Section::to_swc(int &counter, int parent, int type,bool spines,int init) {
        std::stringstream ssSkel;

        ssSkel << this->points[init]->to_swc(counter, parent, type)<< std::endl;
        int currentParent = counter -1;
        bool postSpine = false;

        for (int i= init + 1; i< this->size(); i++){
            auto point = this->points[i];
            if (point->isSpine()) {
                if (spines) {
                    ssSkel << point->to_swc(counter, currentParent,type);
                    postSpine = true;

                }
            } else {
                if (!postSpine) {
                    currentParent = counter-1 ;
                    ssSkel << point->to_swc(counter, currentParent, type) << std::endl;
                } else {
                    ssSkel << point->to_swc(counter, currentParent, type) << std::endl;
                    postSpine = false;
                }
            }
        }
        return ssSkel.str();
    }

    void Section::remove(int index){
        this->points.erase(this->points.begin() + index);
    }

    bool Section::operator<(const Section &rhs) const {
        return name < rhs.name;
    }

}


