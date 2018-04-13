//
// Created by ivelascog on 13/04/18.
//

#include "SubDendrite.h"
namespace skelgenerator{

    std::ostream &operator<<(std::ostream &Str, const SubDendrite& dendrite) {
        Str << dendrite.segment;
        Str << "(" << std::endl;
        Str << dendrite.ramification1;
        Str << "Normal" << std::endl;
        Str << "|" << std::endl;
        Str << dendrite.ramification2;
        Str << "Normal" << std::endl;
        Str << ")\t;\t End of split";
        return Str;
    }
}
