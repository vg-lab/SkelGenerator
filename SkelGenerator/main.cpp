#include "MainWindow.h"
#include <QApplication>
#include <iostream>
#include <SkelGeneratorUtil/Neuron.h>


using  namespace skelgenerator;

    int main(int , char **) {
        TFragment b1;
        b1.points = {{0,0,0},{0,-1,1},{0,0,2},{0,1,3}};
        TFragment b2;
        b2.points = {{0,0,0},{0,1,1},{0,2,0},{0,3,-1}};

        //auto bb1 = Neuron::getBB(b1);
        //auto bb2 = Neuron::getBB(b2);

         Eigen::Vector3d min1 (1,1,1);
         Eigen::Vector3d max1 (2,2,2);
         Eigen::Vector3d min2 ( 1,1,1);
         Eigen::Vector3d max2 (2,2,2);
         float result = Neuron::computeOverlap({min2,max2},{min1,max1});
         std::cout << result << std::endl;



//        QApplication a(argc, argv);
//        MainWindow w;
//        w.show();
//
//        return a.exec();
    }

