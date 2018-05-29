//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_VRMLREADER_H
#define SKELGENERATOR_VRMLREADER_H


#include <string>
#include "Types.h"
namespace skelgenerator {
    class VRMLReader {

        static const int POINTS_PER_CIRCLE = 17;

    public:
        static TDendrite readVrmlApical(const std::string& apiFile);
        static std::vector<TDendrite> readBasalFile(const std::string& basalFile);

    private:
        static TFragment parseFilament(const std::string &name, std::ifstream &file);

        static TSpine parseSpine(const std::string &name, std::ifstream &file);

        static TShape parseShape(std::ifstream &file);
    };
}


#endif //SKELGENERATOR_VRMLREADER_H
