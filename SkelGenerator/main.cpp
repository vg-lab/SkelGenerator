#include "MainWindow.h"
#include <QApplication>
#include <iostream>
#include <SkelGeneratorUtil/Neuron.h>


using  namespace skelgenerator;

    int main(int , char **) {
         std::vector<std::string> basal = {"basal.vrml"};
         std::string apiFile = {};
         std::string imarisFile = {"imaris.vrml"};


         Neuron neuron(apiFile,basal,imarisFile);

         neuron.imarisSpinesToObj("test");

         std::ofstream skelFile;
         skelFile.open("neuron1.swc", std::ios::out);
         skelFile << neuron.to_swc(false);
         skelFile.close();



//        QApplication a(argc, argv);
//        MainWindow w;
//        w.show();
//
//        return a.exec();
    }

