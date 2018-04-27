//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SPINE_H
#define SKELGENERATOR_SPINE_H

#include "SamplePoint.h"

namespace skelgenerator {


    class Spine : public SamplePoint {
        /** The attributes inherited from SamplePoint are the endpoint and its radius **/
        std::vector<SamplePoint> medialAxis;
        Eigen::Vector3f insertPoint;
        std::string name;

    public:
        explicit Spine(const std::string &name);

        const Eigen::Vector3f &getInsertPoint() const;

        const std::string &getName() const;

        void addPoint(Eigen::Vector3f point, float radius);

        void calculatePoints();

        std::string to_asc(std::string tab) const;

        bool operator==(const Spine &spine) {
            return insertPoint == spine.insertPoint;
        }
    };


}

namespace std {
    template<>
    struct hash<skelgenerator::Spine *> {
        size_t operator()(const skelgenerator::Spine *&obj) const {
            auto result = 31 + hash<float>()(obj->getInsertPoint().x());
            result *= 31 + hash<float>()(obj->getInsertPoint().y());
            result *= 31 + hash<float>()(obj->getInsertPoint().z());
            return result;
        }
    };
}


#endif //SKELGENERATOR_SPINE_H
