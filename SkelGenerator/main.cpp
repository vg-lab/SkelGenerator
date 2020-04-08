#include "MainWindow.h"
#include <QApplication>
#include <SkelGeneratorUtil/Neuron.h>

using  namespace skelgenerator;

    int main(int argc, char ** argv) {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec();
    }

