//
// Created by ivelascog on 17/11/17.
//

#ifndef IMARISSKELTEST_TYPES_H
#define IMARISSKELTEST_TYPES_H


#include <vector>
#include <string>
#include <eigen3/Eigen/Dense>
namespace skelgenerator {

    typedef struct {
        int s1, p1, s2, p2, index; //primer segmento(encontro la conexion), punto concreto donde se conecta ,segundo segmento,punto donde conectara el segundo segmento,punto donde conecta con el primer segmento
    } TConn;

    typedef struct {
        int seg, initPoint, idx;
    } TSegToProcess;

    typedef struct {
        std::string nombre;
        std::vector <Eigen::Vector3f> points;
        unsigned int nCircles;
    } TSegment;
}



#endif //IMARISSKELTEST_TYPES_H
