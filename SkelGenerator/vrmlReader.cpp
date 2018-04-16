//
// Created by ivelascog on 16/11/17.
//


#include <regex>
#include <fstream>
#include <iostream>
#include "Types.h"




namespace skelgenerator {
    const int POINTS_PER_CIRCLE = 16;

    static TSegment parseFilament(const std::string &name, std::ifstream &file) {
        TSegment segment;
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

    static std::vector<TSegment> readVrmlApical(const std::string &path) {
        std::vector<TSegment> dentrite;
        std::string line;
        std::ifstream file(path);

        while (file >> line) {
            if (line.find("FilamentSegment6") != std::string::npos) {
                std::cout << line << std::endl;
                dentrite.push_back(parseFilament(line, file));
            } else if (line.find("FilamentSegment7") != std::string::npos) {
                std::cout << line << std::endl;
            }
        }
        std::cout << "------------------finish Apical Dendrite-----------------------------" << std::endl;
        return dentrite;
    }

    static std::vector<std::vector<TSegment>> readVrmlBasal(const std::string &path) {
        std::vector<std::vector<TSegment>> dentrites;
        std::string line;
        std::ifstream file(path);
        int actual = -1;
        int ant = -1;
        int index = 0;
        dentrites.emplace_back();
        while (file >> line) {
            if (line.find("FilamentSegment6") != std::string::npos) {
                std::cout << line << std::endl;
                ant = actual;
                actual = std::stoi(line.substr(20, line.size() - 1));
                if (actual != ant + 1) {
                    actual = 0;
                    ant = -1;
                    dentrites.emplace_back();
                    index++;
                    std::cout << "------------------finish Basal Dendrite-----------------------------" << std::endl;
                }
                dentrites[index].push_back(parseFilament(line, file));
            } else if (line.find("FilamentSegment7") != std::string::npos) {
                std::cout << line << std::endl;
            }
        }
        std::cout << "------------------finish-----------------------------" << std::endl;

        return dentrites;


    }
}
/*
int main(int argc, char *argv[]) {
    readVrml("test.vrml");
    auto fake = segments;
    return 0;
}*/

