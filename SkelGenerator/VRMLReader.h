//
// Created by ivelascog on 13/04/18.
//

#ifndef SKELGENERATOR_VRMLREADER_H
#define SKELGENERATOR_VRMLREADER_H


#include <string>
#include "Types.h"
namespace skelgenerator {
    class VRMLReader {

        const int POINTS_PER_CIRCLE = 17;

    public:
        TDendrite readVrmlApical(std::string apiFile);
        std::vector<TDendrite> readBasalFile(std::string basalFile);

    private:
        TSegment parseFilament(const std::string &name, std::ifstream &file);

        TSpine parseSpine(const std::string &name, std::ifstream &file);
    };
}


#endif //SKELGENERATOR_VRMLREADER_H
