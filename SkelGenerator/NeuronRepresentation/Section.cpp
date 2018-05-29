//
// Created by ivelascog on 13/04/18.
//

#include <iostream>
#include "Section.h"


namespace skelgenerator {


    Section::Section(const std::string &name) {
        this->name = name;

    }

    void Section::addPoint(SamplePoint* &samplePoint) {
        this->points.push_back(samplePoint);
    }

    int Section::size() {
        return static_cast<int>(points.size());
    }

    std::tuple<Section *, Section *> Section::split(int point) {
        auto segment1 = new Section(this->name + "-1");
        auto segment2 = new Section(this->name + "-2");
        for (int i = 0; i < point; i++) {
            segment1->addPoint(points[i]);
        }

        for (int i = point; i < this->size(); i++) {
            segment2->addPoint(points[i]);
        }

        return std::make_tuple(segment1, segment2);
    }

    const std::string &Section::getName() const {
        return name;
    }

    std::string Section::to_asc(std::string tab) {
        std::stringstream ss;
        int i =1 ;
        if (this->size() == 0) {
            ss << tab  << "Empty Section: " << this->name;
        }
        ss << tab << "; " << this->name << std::endl;
        for (const auto &point:this->points) {
            ss << point->to_asc(tab) <<";  " << i << std::endl;
            i++;
        }
        return ss.str();
    }

    Section *Section::unionSection(Section *section1, Section *section2) {
        auto resultSegment = new Section(section1->getName() + "+" +  section2->getName());
        for (const auto &point :section1->points) {
            resultSegment->points.push_back(point);
        }

        for (const auto &point :section2->points) {
            resultSegment->points.push_back(point);
        }
        return resultSegment;
    }

    void Section::reverse() {
        std::reverse(this->points.begin(),this->points.end());
    }

    void Section::trim(int i) {
        this->points.erase(this->points.begin(),this->points.begin()+i);

    }

    void Section::addPoint(Eigen::Vector3f point, float radius) {
        auto p = new SamplePoint(point,radius);
        points.push_back(p);

    }

    void Section::addPoint(Spine *&samplePoint, int pos) {
        this->points.insert(this->points.begin() + pos, samplePoint);

    }

    std::tuple<std::string, std::string> Section::to_neuronice(int &counter, int parent, int type) {
        std::stringstream ssSkel;
        std::stringstream ssSpines;

        ssSkel << std::get<0>(this->points[0]->to_neuronize(counter,parent,type)) << std::endl;

        for (int i= 1; i< this->size(); i++){
            auto point = this->points[i];
            if (point->isSpine()) {
                // TODO
            } else {
                ssSkel << std::get<0>(point->to_neuronize(counter,counter -1,type)) << std::endl;
            }

        }
        return std::make_tuple(ssSkel.str(),"");
    }
}


