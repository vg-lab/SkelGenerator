//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SPINE_H
#define SKELGENERATOR_SPINE_H

#include "SamplePoint.h"
#include <unordered_set>

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

        std::string to_asc(std::string tab) const override;

        bool isSpine() override;

    };


    struct SpinePtrComparator {
        bool operator()(Spine *const &obj1, Spine *&obj2) const {
            return obj1->getInsertPoint() == obj2->getInsertPoint();
        }
    };

    struct SpinePtrHasher {
        size_t operator()(Spine *const &obj) const {
            auto result = 31 + std::hash<float>()(obj->getInsertPoint().x());
            result *= 31 + std::hash<float>()(obj->getInsertPoint().y());
            result *= 31 + std::hash<float>()(obj->getInsertPoint().z());
            return result;
        };
    };

    using spineSet =  std::unordered_set<Spine *, SpinePtrHasher, SpinePtrComparator>;


}






#endif //SKELGENERATOR_SPINE_H
