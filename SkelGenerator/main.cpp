#include "MainWindow.h"
#include "VRMLReader.h"
#include "NeuronRepresentation/Neuron.h"
#include <QApplication>
#include <iostream>


using  namespace skelgenerator;

    int main(int argc, char *argv[]) {

        std::vector<std::string> basal = {"basal2.vrml"};
        std::string apiFile = {"api2.vrml"};


        Neuron neuron(apiFile,basal);

        std::ofstream skelFile;
        skelFile.open("test.asc",std::ios::out);

        skelFile << neuron.to_asc();
        skelFile.close();



        /*QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec(); */
    }

