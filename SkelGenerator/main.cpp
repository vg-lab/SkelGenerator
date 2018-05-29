#include "MainWindow.h"
#include "VRMLReader.h"
#include "NeuronRepresentation/Neuron.h"
#include <QApplication>
#include <iostream>


using  namespace skelgenerator;

    int main(int argc, char *argv[]) {

         std::vector<std::string> basal = {"basal.wrl"};
         std::string apiFile = {"api.wrl"};


         Neuron neuron(apiFile,basal);

         std::ofstream skelFile;
         skelFile.open("neuron1.swc", std::ios::out);
         skelFile << std::get<0>(neuron.to_neuronize());
         skelFile.close();

        neuron.spines_to_obj("testSpines");


    /*
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec(); */
    }

