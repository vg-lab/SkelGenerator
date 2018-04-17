//
// Created by ivelascog on 17/11/17.
//

#ifndef IMARISSKELTEST_TYPES_H
#define IMARISSKELTEST_TYPES_H


#include <vector>
#include <string>
#include <eigen3/Eigen/Dense>
#include "NeuronRepresentation/Segment.h"

namespace skelgenerator {

    typedef struct {
        Segment * segment1;
        int  p1;
        Segment * segment2;
        int p2 /*,index*/;
        //primer segmento(encontro la conexion), punto concreto donde se conecta ,segundo segmento,punto donde conectara el segundo segmento,punto donde conecta con el primer segmento
    } TConn;

    typedef struct {
        Segment segment;
        int initPoint, idx;
    } TSegToProcess;

    typedef struct {
        std::string nombre;
        std::vector <Eigen::Vector3f> points;
        unsigned int nCircles;
    } TSegment;

    typedef struct {
        std::string nombre;
        std::vector <Eigen::Vector3f> points;
        unsigned int nCircles;
    } TSpine;

    typedef struct {
        std::vector<TSegment> segments;
        std::vector<TSpine> spines;
    } TDendrite;


}



#endif //IMARISSKELTEST_TYPES_H
