//
// Created by ivelascog on 13/04/18.
//

#include "Dendrite.h"
namespace skelgenerator {

    std::ostream &operator<<(std::ostream &Str, const Dendrite& d) {
        std::string color = d.dendtype==APICAL ? "Green": "Blue";
        std::string type = d.dendtype == APICAL ? "Apical" : "Dendrite";
        Str << "(\t(Color " << color << ")" << std::endl;
        Str << "(" << type << ")" << std::endl;
        Str << d.dendrite;
        Str << ") ; End of Tree";
        return Str;
    }
}


