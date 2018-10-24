//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_SPINE_H
#define SKELGENERATOR_SPINE_H

#include "SamplePoint.h"
#include <unordered_set>
#include <vector>


namespace skelgenerator {


    typedef struct {
        std::vector <Eigen::Vector3d> points;
        unsigned int nCircles;
        std::vector<std::vector<int>> faces;
    } TShape;

    typedef struct {
        std::string nombre;
        std::vector<TShape> shapes;
    } TSpine;


    class Spine : public SamplePoint {
        /** The attributes inherited from SamplePoint are the endpoint and its radius **/
        std::vector<SamplePoint> medialAxis;
        Eigen::Vector3d insertPoint;
        TSpine spine3D;
        unsigned int id;



    public:

        static int counter;

        explicit Spine(TSpine spine3D);

        const Eigen::Vector3d &getInsertPoint() const;

        const std::string &getName() const;

        void addPoint(Eigen::Vector3d point, float radius);

        void calculatePoints();

        std::string to_asc(std::string tab) const override;

        bool isSpine() override;

        void to_obj(const std::string &path,int i);

        void to_obj_without_base (const std::string &path,int i);

        void shape_to_obj(const std::string &path,int shapeIndex);

        std::string to_swc(int &counter, int parent, int type) override;

    };


    struct SpinePtrComparator {
    public:
        bool operator()(Spine *const &obj1, Spine *const &obj2) const {
            return obj1->getInsertPoint() == obj2->getInsertPoint();
        }
    };

    struct SpinePtrHasher {
    public:
        size_t operator()(Spine* const &obj) const {
            auto result = 31 + std::hash<float>()(obj->getInsertPoint().x());
            result *= 31 + std::hash<float>()(obj->getInsertPoint().y());
            result *= 31 + std::hash<float>()(obj->getInsertPoint().z());
            return result;
        };
    };

    using spineSet =  std::unordered_set<Spine *, SpinePtrHasher, SpinePtrComparator>;


}






#endif //SKELGENERATOR_SPINE_H
