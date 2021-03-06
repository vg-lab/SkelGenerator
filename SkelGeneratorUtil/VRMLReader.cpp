//
// Created by ivelascog on 13/04/18.
//

#include <fstream>
#include <clocale>
#include "VRMLReader.h"
#include "Types.h"
#include "Spine.h"

namespace skelgenerator {

    TFragment VRMLReader::parseFilament(const std::string &name, std::ifstream &file) {
        TFragment segment;
        std::string line;
        std::vector<Eigen::Vector3d> points;
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
                            Eigen::Vector3d vec(point[0], point[1], point[2]);
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
        std::string line;
        TSpine spine;
        std::vector<TShape> shapes;
        while (file >> line) {
            if (line.find("DEF") != std::string::npos) {
                break;
            } else if (line.find("Shape") != std::string::npos) {
                shapes.push_back(parseShape(file));
            }
        }
        spine = {name, shapes};
        return spine;
    }

    TShape VRMLReader::parseShape(std::ifstream &file) {
        TShape shape;
        std::string line;
        std::vector<Eigen::Vector3d> points;
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
                            Eigen::Vector3d vec(point[0], point[1], point[2]);
                            points.emplace_back(vec);
                            nCoords = 0;
                        }
                    } else {
                        break;
                    }
                }
                break;
            }
        }

        while (file >> line) {
            if (line.find("coordIndex") != std::string::npos) {
                file >> line; // ELiminamos el [ de la entrada
                std::vector<std::vector<int>> faces;
                std::vector<int> currentFace;
                while (file >> line) {
                    if (line.find(']') != std::string::npos) {
                        shape = {points, (unsigned int) points.size() / POINTS_PER_CIRCLE, faces};
                        break;

                    } else if (line.find("-1") != std::string::npos) {
                        faces.push_back(currentFace);
                        currentFace.clear();
                    } else {
                        std::string coord = line.substr(0, line.size() - 1);
                        currentFace.push_back(std::stoi(coord));
                    }
                }
                break;
            }
        }

        return shape;
    }


    TDendrite VRMLReader::readVrmlApical(const std::string &apiFile) {
        TDendrite dendrite = TDendrite();
        std::string line;
        std::ifstream file(apiFile);

        if (!file.is_open()) {
            throw "Can't open apical file: " + apiFile;
        }


        while (file >> line) {
            if (line.find("FilamentSegment6") != std::string::npos) {
                dendrite.fragments.push_back(parseFilament(line, file));
            } else if (line.find("FilamentSegment7") != std::string::npos) {
              auto spine = parseSpine(line,file);
              if (!spine.shapes.empty())
              {
                dendrite.spines.push_back( spine );
              }
            }
        }
        return dendrite;
    }

    std::vector<skelgenerator::TDendrite> VRMLReader::readBasalFile(const std::string &basalFile) {
        std::vector<TDendrite> dendrites;
        std::string line;
        std::ifstream file(basalFile);

        if (!file.is_open()) {
            throw std::runtime_error("Can't open basal file: " + basalFile);
        }

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
              auto spine = parseSpine(line,file);
              if (!spine.shapes.empty())
              {
                dendrites[ index ].spines.push_back( spine );
              }
            }
        }
        return dendrites;
    }

    std::vector<skelgenerator::TSpineImaris> VRMLReader::readImarisSpines(const std::string &imarisFile) {
        std::vector<skelgenerator::TSpineImaris> spines;
        std::string line;
        std::ifstream file(imarisFile);

        if (!file.is_open()) {
            throw std::runtime_error("Can't open file: " + imarisFile);
        }

        return parseImarisSpine(file);
    }


    std::vector<TSpineImaris> VRMLReader::parseImarisSpine(std::ifstream &file) {
        std::string line;
        std::vector<Eigen::Vector3d> points;
        std::vector<TSpineImaris> spines;
        std::setlocale(LC_NUMERIC, "en_US.UTF-8");
        while (file >> line) {
            if (line.find("point") != std::string::npos) {
                points.clear();
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
                            Eigen::Vector3d vec(point[0], point[1], point[2]);
                            points.emplace_back(vec);
                            nCoords = 0;
                        }
                    } else {
                        break;
                    }
                }
            } else if (line.find("coordIndex") != std::string::npos) {
                file >> line; // ELiminamos el [ de la entrada
                std::vector<std::vector<int>> faces;
                std::vector<int> currentFace;
                while (file >> line) {
                    if (line.find(']') != std::string::npos) {
                        spines.push_back({points, faces});
                        break;
                    } else if (line.find("-1") != std::string::npos) {
                        faces.push_back(currentFace);
                        currentFace.clear();
                    } else {
                        std::string coord = line.substr(0, line.size() - 1);
                        currentFace.push_back(std::stoi(coord));
                    }
                }
            }
        }
        return spines;
    }

    std::vector<std::vector<skelgenerator::SamplePoint>>
    VRMLReader::readImarisSpinesLong(const std::string &longsFile) {
        std::vector<std::vector<skelgenerator::SamplePoint>> spines;
        std::vector<skelgenerator::SamplePoint> spine;
        std::ifstream file(longsFile);
        Eigen::Vector3d point;
        float radius;
        std::string line;
        std::string previus;

        while (file >> line) {
            if (line.find("bpMeasurementPointsInventor") != std::string::npos) {
                break;
            }
        }


        while (file >> line) {
            if (line.find("translation") != std::string::npos) {
                file >> point[0];
                file >> point[1];
                file >> point[2];
            } else if (line.find("radius") != std::string::npos) {
                file >> radius;
                spine.emplace_back(point, radius);
            } else if (previus.find("USE")!= std::string::npos && line.find("bpMeasurementPointsShapeInventor") != std::string::npos) {
                spine.emplace_back(point,radius);
            } else if (line.find("bpMeasurementPointsInventor") != std::string::npos) {
                spines.push_back(spine);
                spine.clear();
            }
            previus = line;
        }

        spines.push_back(spine);
        return spines;
    }
}




