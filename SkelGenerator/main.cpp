#include "MainWindow.h"
#include "VRMLReader.h"
#include <QApplication>


using  namespace skelgenerator;

    int main(int argc, char *argv[]) {

        VRMLReader reader;
        auto api = reader.readVrmlApical("api.wrl");
        auto ba =reader.readBasalFile("basal.wrl");
        /*QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec(); */
    }

