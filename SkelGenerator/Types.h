//
// Created by ivelascog on 17/11/17.
//

#ifndef IMARISSKELTEST_TYPES_H
#define IMARISSKELTEST_TYPES_H


#include <vector>
#include <string>
#include <eigen3/Eigen/Dense>
#include "NeuronRepresentation/Section.h"

namespace skelgenerator {

    typedef struct {
        Section * fragment1;
        int  p1;
        Section * fragment2;
        int p2 /*,index*/;
        //primer segmento(encontro la conexion), punto concreto donde se conecta ,segundo segmento,punto donde conectara el segundo segmento,punto donde conecta con el primer segmento
    } TConn;

    typedef struct {
        Section fragment;
        int initPoint, idx;
    } TSegToProcess;

    typedef struct {
        std::string nombre;
        std::vector <Eigen::Vector3f> points;
        unsigned int nCircles;
    } TFragment;


    typedef struct {
        std::vector<TFragment> fragments;
        std::vector<TSpine> spines;
    } TDendrite;


}



#endif //IMARISSKELTEST_TYPES_H
