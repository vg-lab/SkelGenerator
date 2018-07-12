#include "MainWindow.h"
#include <QApplication>
#include <iostream>


using  namespace skelgenerator;

    int main(int argc, char *argv[]) {
    /*
         std::vector<std::string> basal = {"basal.wrl"};
         std::string apiFile = {"apical.wrl"};


         Neuron neuron(apiFile,basal);

         std::ofstream skelFile;
         skelFile.open("neuron1.swc", std::ios::out);
         skelFile << neuron.to_swc(false);
         skelFile.close();
         */


        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec();
    }

