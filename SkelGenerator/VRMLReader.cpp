//
// Created by ivelascog on 13/04/18.
//

#include <fstream>
#include "VRMLReader.h"
#include "Types.h"
namespace skelgenerator {

   TFragment VRMLReader::parseFilament(const std::string &name, std::ifstream &file) {
        TFragment segment;
        std::string line;
        std::vector<Eigen::Vector3f> points;
        std::setlocale(LC_NUMERIC, "en_US.UTF-8");
        while (file >> line) {
            if (line.find("point") != std::string::npos) {
                file >> line; // Eliminamos el [ del atributo point
                float point[3];// extructura auxiliar par ir alamacenando los puntos
                int nCoords = 0;

                while (file >> line) {
                    if (line.find(']') ==
                        std::string::npos) { // si encontramos el el final de los puntos construimos el segmento y salimos de los bucles
                        if (line.at(line.length() - 1) == ',') {
                            line = line.substr(0, line.length() - 1);
                        }
                        point[nCoords] = std::stof(line);
                        nCoords++;
                        if (nCoords == 3) {
                            Eigen::Vector3f vec(point[0], point[1], point[2]);
                            points.emplace_back(vec);
                            nCoords = 0;
                        }
                    } else {
                        segment = {name, points, (unsigned int) points.size() / POINTS_PER_CIRCLE};
                        break;
                    }
                }
                break;
            }
        }
        return segment;
    }

    TSpine VRMLReader::parseSpine(const std::string &name, std::ifstream &file) {
        TSpine spine;
        std::string line;
        std::vector<Eigen::Vector3f> points;
        std::setlocale(LC_NUMERIC, "en_US.UTF-8");
        while (file >> line) {
            if (line.find("point") != std::string::npos) {
                file >> line; // Eliminamos el [ del atributo point
                float point[3];// extructura auxiliar par ir alamacenando los puntos
                int nCoords = 0;

                while (file >> line) {
                    if (line.find(']') ==
                        std::string::npos) { // si encontramos el el final de los puntos construimos el segmento y salimos de los bucles
                        if (line.at(line.length() - 1) == ',') {
                            line = line.substr(0, line.length() - 1);
                        }
                        point[nCoords] = std::stof(line);
                        nCoords++;
                        if (nCoords == 3) {
                            Eigen::Vector3f vec(point[0], point[1], point[2]);
                            points.emplace_back(vec);
                            nCoords = 0;
                        }
                    } else {
                        spine = {name, points, (unsigned int) points.size() / POINTS_PER_CIRCLE};
                        break;
                    }
                }
                break;
            }
        }
        return spine;
    }



    TDendrite VRMLReader::readVrmlApical(const std::string& apiFile) {
        TDendrite dendrite = TDendrite();
        std::string line;
        std::ifstream file(apiFile);

        if(file.is_open()) {

        }


        while (file >> line) {
            if (line.find("FilamentSegment6") != std::string::npos) {
                dendrite.fragments.push_back(parseFilament(line, file));
            } else if (line.find("FilamentSegment7") != std::string::npos) {
                dendrite.spines.push_back(parseSpine(line,file));
            }
        }
        return dendrite;
    }

    std::vector<skelgenerator::TDendrite> VRMLReader::readBasalFile(const std::string& basalFile) {
        std::vector<TDendrite> dendrites;
        std::string line;
        std::ifstream file(basalFile);
        int actual = -1;
        int ant = -1;
        int index = 0;
        dendrites.emplace_back();
        while (file >> line) {
            if (line.find("FilamentSegment6") != std::string::npos) {
                ant = actual;
                actual = std::stoi(line.substr(20, line.size() - 1));
                if (actual != ant + 1) {
                    actual = 0;
                    ant = -1;
                    dendrites.emplace_back();
                    index++;
                }
                dendrites[index].fragments.push_back(parseFilament(line, file));
            } else if (line.find("FilamentSegment7") != std::string::npos) {
                dendrites[index].spines.push_back(parseSpine(line,file));
            }
        }
        return dendrites;
    }


}
