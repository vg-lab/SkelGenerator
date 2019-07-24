//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SAMPLEDATA_H
#define SKELGENERATOR_SAMPLEDATA_H

#include <skelgenerator/api.h>


#include <ostream>
#include <Eigen/Dense>
#include <iostream>
namespace skelgenerator {

    class SamplePoint {

    protected:
        Eigen::Vector3d point;
        float r{-1};

    public:
        SamplePoint(Eigen::Vector3d &point, float r);

        SamplePoint() = default;

	virtual ~SamplePoint() {}
	
        SKELGENERATOR_API const Eigen::Vector3d &getPoint() const;

        float getRadius() const;

        virtual std::string to_asc(std::string tab) const;

        virtual std::string to_swc(int &counter, int parent, int type);

        virtual bool isSpine();
    };

}










#endif //SKELGENERATOR_SAMPLEDATA_H
