//
// Created by ivelascog on 13/04/18.
//


#include "Neuron.h"
#include "Section.h"
#include "Types.h"
#include "Spine.h"
#include <Eigen/Dense>
#include <set>
#include <unordered_set>
#include <iostream>
#include <boost/filesystem.hpp>
#include <iomanip>
#include <Eigen/SVD>

#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

namespace skelgenerator {

    Neuron::Neuron(const std::string &apiFile_, const std::vector<std::string> &basalFiles_, const std::string &imarisFile_,
                   const std::string &longsFile_,
                   float connectionThreshold_) {
        //std::cout << "Conection Treshold " << connectionThreshold << std::endl;
        this->apiFile = apiFile_;
        this->imarisFile = imarisFile_;
        this->basalFiles = basalFiles_;
        this->incorrectConecctions = false;
        this->connectionThreshold = connectionThreshold_;
        this->apical = nullptr;
        this->numDendrites = 0;
        TDendrite apiDendrite = {};
        if (!apiFile_.empty()) {
            apiDendrite = VRMLReader::readVrmlApical(apiFile_);
            this->numDendrites++;
        }

        std::vector<TDendrite> basalDendrites;
        for (const auto &basalFile : basalFiles_) {
            auto thisBasal = VRMLReader::readBasalFile(basalFile);
            basalDendrites.insert(basalDendrites.end(), thisBasal.begin(), thisBasal.end());
        }

        this->numDendrites += basalDendrites.size();

        if (!imarisFile_.empty()) {
            this->imarisSpines = VRMLReader::readImarisSpines(imarisFile_);
        }

        procesSkel(apiDendrite, basalDendrites);
        removeDuplicates();
        generateSoma();

        if (!longsFile_.empty()) {
            addSpinesLongs(longsFile_);

            //Añadimos la geometria de las espinas de Filament.
            if (!apiDendrite.fragments.empty()) {
                auto apiSpines = generateSpines(apiDendrite);
                this->spines.insert(apiSpines.begin(), apiSpines.end());
            }

            std::vector<spineSet> basalsSpines;
            for (const auto &basal:basalDendrites) {
                auto spineSet_ = generateSpines(basal);
                this->spines.insert(spineSet_.begin(), spineSet_.end());
            }

        } else {
            procesSpinesFilament(apiDendrite, basalDendrites);
        }

    }

    void Neuron::addSpinesLongs(const std::string &longsFile_) {
        this->longsSpines = VRMLReader::readImarisSpinesLong(longsFile_);
        procesSpinesLongs();
    }

    std::vector<Section *> Neuron::generateFragments(TDendrite dendrite) {
        std::vector<Section *> fragments;
        for (const auto &fragment : dendrite.fragments) {
            fragments.push_back(getFragment(fragment));
        }

        return fragments;

    }

    Section *Neuron::getFragment(const TFragment &fragment) {
        auto sectionSkel = new Section(fragment.nombre);
        for (unsigned int cir = 3; cir < fragment.nCircles; cir++) {
            auto medio = Eigen::Vector3d(0, 0, 0);
            for (int i = 0; i < 17; i++) {
                medio += fragment.points[cir * 17 + i];
            }
            medio = medio / 17;
            float radius = -1;
            for (int i = 0; i < 17; i++) {
                auto r = (medio - fragment.points[cir * 17 + i]).norm();
                radius = radius < r ? r : radius;
            }
            sectionSkel->addPoint(medio, radius);
        }
        return sectionSkel;
    }

    std::tuple<SubDendrite *, int> Neuron::computeDendrite(std::vector<Section *> fragments) {
        std::set<Section *> reaminFragments;
        for (size_t i = 0; i < fragments.size(); i++) {
            reaminFragments.insert(fragments[i]);
        }

        auto subDendrite = computeSubDendrite(fragments[0], 0, reaminFragments);

        return std::make_tuple(subDendrite, static_cast<int>(reaminFragments.size()));;

    }


    SubDendrite *Neuron::computeSubDendrite(Section *fragment, int initPoint, std::set<Section *> &reamingFragments) {
        //std::cout << "Processing fragment: " << fragment->getName() << "\n" << std::flush;
        reamingFragments.erase(fragment);
        std::vector<TConn> conns;
        for (auto anotherFragment : reamingFragments) {
            float minDistance = 1000;
            int minPoint1 = 0;
            int minPoint2 = 0;
            for (int i = 0; i < fragment->size(); i++) {
                auto p1 = (*fragment)[i]->getPoint();
                auto p1r = (*fragment)[i]->getRadius();
                for (int j = 0; j < anotherFragment->size(); j++) {
                    auto p2 = (*anotherFragment)[j]->getPoint();
                    auto p2r = (*anotherFragment)[j]->getRadius();
                    float dist = (p1 - p2).norm();
                    dist = dist - p1r - p2r;

                    if (dist < minDistance) {
                        minDistance = dist;
                        minPoint1 = i;
                        minPoint2 = j;
                    }
                }
            }


            if (minDistance < connectionThreshold) {
                std::cout << minDistance << std::endl;
                TConn conn{fragment, minPoint1, anotherFragment, minPoint2};
                conns.push_back(conn);
            }
        }

        //Le damos la vuelta a fragmentos al reves
        if (initPoint > fragment->size() / 2) {
            std::cout << "Rama del reves" << std::endl;
            fragment->reverse();

            initPoint = (fragment->size() - initPoint);
            for (auto &conn : conns) {
                conn.p1 = (fragment->size() - conn.p1);
            }
        }


        fragment->trim(initPoint);
        for (auto conn = conns.begin(); conn < conns.end(); conn++) {
            conn->p1 -= initPoint;
            /* Si alguna conexion es negativa indica que se ha realizado una mala conexion de segmentos anteriores por
             * un connectionTreshold demasiado elevado, se opta por eliminar las conexiones problematicas y proseguir
             * con una solucion , Notese que estas conexiones eliminadas pueden volver a ser conectadas por otros fragmentos*/
            if (conn->p1 < 0) {
                //std::cout << "error de conexion" << std::endl;
                conns.erase(conn);
                this->incorrectConecctions = true;
            }
        }

        if (conns.empty()) {
            return new SubDendrite(fragment);
        } else {

            if (conns.size() == 2) { //Seccion perfecta con 2 ramificaciones
                auto con1 = conns[0];
                auto con2 = conns[1];
                int dist = std::abs(con1.p1 - con2.p1);
                if (dist < 2 && (con1.p1 == fragment->size() - 1 || con2.p1 == fragment->size() - 1)) {
                    //std::cout << "Seccion" << std::endl;
                    auto subDendrite = new SubDendrite(fragment);
                    reamingFragments.erase(con1.fragment2);
                    reamingFragments.erase(con2.fragment2);
                    subDendrite->setRamification1(computeSubDendrite(con1.fragment2, con1.p2, reamingFragments));
                    subDendrite->setRamification2(computeSubDendrite(con2.fragment2, con2.p2, reamingFragments));
                    return subDendrite;
                }
            }


            TConn *firstCoon = nullptr;
            int min = fragment->size();
            for (auto &coon : conns) {
                if (coon.p1 <= min) {
                    firstCoon = &coon;
                    min = coon.p1;
                }
            }


            if (conns.size() == 1 && (conns[0].p1 == fragment->size() - 1 || conns[0].p1 ==
                                                                             0)) { // Si solo tiene una conexion se unen el segmento actual con el encontrado
                auto con = conns[0];
                std::cout << "Segmento suelto " << con.p1 << " = " << fragment->size() - 1 << std::endl;
                reamingFragments.erase(fragment);
                reamingFragments.erase(con.fragment2);
                //Comprobamos que el segmento con el que vamos a unir no este del reves
                if (con.p2 > con.fragment2->size() / 2) {
                    std::cout << "Rama del reves" << std::endl;
                    con.fragment2->reverse();

                    con.p2 = (con.fragment2->size() - con.p2);
                }
                if (con.p1 == 0) {
                    auto unionSegment = Section::unionSection(con.fragment2, fragment);
                    return computeSubDendrite(unionSegment, con.p2, reamingFragments);
                } else {
                    auto unionSegment = Section::unionSection(fragment, con.fragment2);
                    return computeSubDendrite(unionSegment, initPoint, reamingFragments);
                }
            }

            std::vector<TConn> connsInSamePoint;
            connsInSamePoint.push_back(*firstCoon);

            for (auto &con : conns) {
                if (&con != firstCoon) {
                    if (con.p1 == firstCoon->p1) {
                        connsInSamePoint.push_back(con);
                    }
                }
            }

            if (connsInSamePoint.size() > 1) {
                std::cout << "more than 1 conection in one point :" << connsInSamePoint.size() << std::endl;

                int splitPoint = std::max(1, firstCoon->p1 - static_cast<int>(connsInSamePoint.size()));

                auto segmentSplit = fragment->split(splitPoint);
                auto segment1 = std::get<0>(segmentSplit);
                auto segment2 = std::get<1>(segmentSplit);

                reamingFragments.erase(firstCoon->fragment2);
                auto subDendrite = new SubDendrite(segment1);
                subDendrite->setRamification1(computeSubDendrite(segment2, 0, reamingFragments));
                subDendrite->setRamification2(
                        computeSubDendrite(firstCoon->fragment2, firstCoon->p2, reamingFragments));
                return subDendrite;
            }

            auto segmentSplit = fragment->split(firstCoon->p1);
            auto segment1 = std::get<0>(segmentSplit);
            auto segment2 = std::get<1>(segmentSplit);


            reamingFragments.erase(firstCoon->fragment2);
            auto subDendrite = new SubDendrite(segment1);
            subDendrite->setRamification1(computeSubDendrite(segment2, 0, reamingFragments));
            subDendrite->setRamification2(computeSubDendrite(firstCoon->fragment2, firstCoon->p2, reamingFragments));
            return subDendrite;

        }
    }

    std::string Neuron::to_asc() {
        std::string tab;
        std::stringstream ss;
        ss << "(\"CellBody\"" << std::endl;
        tab += "\t";
        ss << tab << "(Color RGB (255, 128, 64))" << std::endl;
        ss << tab << "(CellBody)" << std::endl;

        //Calculamos los puntos de la rodaja 2D a partir del centro y del radio del soma

        auto centerX = this->soma.getPoint()[0];
        auto centerY = this->soma.getPoint()[1];

        for (int i = 0; i < 360; i += 10) {
            auto x = centerX + this->soma.getRadius() * cos(degreesToRadians(i));
            auto y = centerY + this->soma.getRadius() * sin(degreesToRadians(i));
            Eigen::Vector3d point(x, y, this->soma.getPoint()[2]);
            SamplePoint pointr(point, 0.15f);
            ss << pointr.to_asc(tab) << std::endl;
        }

        ss << ")" << std::endl;
        if (this->apical != nullptr)
            ss << this->apical->to_asc(tab, 0);
        for (auto basal:this->basals) {
            ss << basal->to_asc(tab, 0);
        }
        return ss.str();
    }

    std::string
    Neuron::to_asc(const std::vector<std::vector<Eigen::Vector3f>> &contours, const Eigen::Vector3f &displacement) {
        std::string tab;
        std::stringstream ss;

        for (const auto &contour: contours) {
            ss << "(\"Soma\" " << std::endl;
            ss << "\t" << "(Closed)" << std::endl;
            ss << "\t" << "(FillDensity 0)" << std::endl;
            ss << "\t" << "(MBFObjectType 5)" << std::endl;
            for (const auto &point: contour) {
                ss << "\t" << "(\t" << point[0] + displacement[0] << "\t" << point[1] + displacement[1] << "\t"
                   << point[2] + displacement[2] << ")" << std::endl;
            }
            ss << ")" << std::endl;
        }

        if (this->apical != nullptr)
            ss << this->apical->to_asc(tab, 0);
        for (auto basal:this->basals) {
            ss << basal->to_asc(tab, 0);
        }
        return ss.str();
    }

    void Neuron::procesSkel(const TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites) {
        int reamingFragments = 0;
        if (!apiDendrite.fragments.empty()) {
            auto apiFragments = generateFragments(apiDendrite);
            auto apiDendriteSkel = new Dendrite();

            auto resultApi = computeDendrite(apiFragments);
            apiDendriteSkel->setDendrite(std::get<0>(resultApi));
            apiDendriteSkel->setDendtype(APICAL);
            this->apical = apiDendriteSkel;

            reamingFragments += std::get<1>(resultApi);
        }

        std::vector<std::vector<Section *>> basalsFragments;
        for (const auto &basalDedrite: basalDendrites) {
            basalsFragments.push_back(generateFragments(basalDedrite));
        }


        for (const auto &basalFragments: basalsFragments) {
            auto basalDend = new Dendrite();
            auto result = computeDendrite(basalFragments);
            basalDend->setDendtype(BASAL);
            basalDend->setDendrite(std::get<0>(result));
            reamingFragments += std::get<1>(result);
            this->basals.push_back(basalDend);
        }
        this->reamingSegments = reamingFragments;
    }

    void Neuron::procesSpinesLongs() {
        spineSet longSpinesSet;
        for (const auto spine: this->longsSpines) {
            longSpinesSet.insert(new Spine(spine));
        }

        if (apical != nullptr) {
            addSpines(apical, longSpinesSet);
        }

        for (size_t i = 0; i < basals.size(); i++) {
            addSpines(basals[i], longSpinesSet);
        }
    }


    void Neuron::procesSpinesFilament(TDendrite &apiDendrite, const std::vector<TDendrite> &basalDendrites) {
        if (!apiDendrite.fragments.empty()) {
            auto apiSpines = generateSpines(apiDendrite);
            this->spines.insert(apiSpines.begin(), apiSpines.end());
            addSpines(apical, apiSpines);
        }

        std::vector<spineSet> basalsSpines;
        for (const auto &basal:basalDendrites) {
            auto spineSet_ = generateSpines(basal);
            this->spines.insert(spineSet_.begin(), spineSet_.end());
            basalsSpines.push_back(spineSet_);
        }


        for (size_t i = 0; i < basalsSpines.size(); i++) {
            addSpines(basals[i], basalsSpines[i]);
        }
    }

    spineSet Neuron::generateSpines(const TDendrite &dendrite) {
        spineSet spines_;
        for (const auto &spine:dendrite.spines) {
            auto spineSkel = new Spine(spine);
            spines_.insert(spineSkel);
        }
        return spines_;

    }


    void Neuron::addSpines(Dendrite *dendrite, spineSet &spines_) {
        std::vector<std::tuple<Spine *, Section *, int, float>> spinesToInsert;
        for (auto spine :spines_) {
            auto result = getPosSpine(dendrite->getDendrite(), spine);
            auto sec = std::get<0>(result);
            auto pos = std::get<1>(result);
            auto min = std::get<2>(result);

            if (min < connectionThreshold) {
                if (pos == 0) {
                    sec->addPoint(spine, 1);
                } else {
                    sec->addPoint(spine, pos);
                }
            } else {
                std::cout << "Error espina no conectada: Dist " << min << " id: " << spine->getId() << std::endl;
            }
        }
    }

    std::tuple<Section *, int, float> Neuron::getPosSpine(SubDendrite *subDendrite, Spine *spine) {
        const auto &insertPoint = spine->getInsertPoint();
        auto sec = subDendrite->getSec();
        float min = 1000.0f;
        int mini = 0;
        for (int i = 0; i < sec->size(); i++) {
            auto skelSample = (*sec)[i];
            if (!skelSample->isSpine()) {
                float dist = std::abs((insertPoint - (*sec)[i]->getPoint()).norm() - (*sec)[i]->getRadius());


                if (dist < min) {
                    min = dist;
                    mini = i;
                }
            }
        }

        if (subDendrite->getRamification1() == nullptr && subDendrite->getRamification2() == nullptr) {
            return std::make_tuple(sec, mini, min);
        } else {
            std::tuple<Section *, int, float> result1;
            std::tuple<Section *, int, float> result2;
            float minDist1 = 1000.0f;
            float minDist2 = 1000.0f;

            if (subDendrite->getRamification1() != nullptr) {
                result1 = getPosSpine(subDendrite->getRamification1(), spine);
                minDist1 = std::get<2>(result1);
            }

            if (subDendrite->getRamification2() != nullptr) {
                result2 = getPosSpine(subDendrite->getRamification2(), spine);
                minDist2 = std::get<2>(result2);
            }


            if (min <= minDist1 && min <= minDist2) {
                return std::make_tuple(sec, mini, min);
            } else if (minDist1 <= min && minDist1 <= minDist2) {
                return result1;
            } else if (minDist2 <= min && minDist2 <= minDist1) {
                return result2;
            }
        }

        // WAR to avoid warnings (TODO: fix this)
        // This statemets should never be reached
        assert(false);
        return std::make_tuple(nullptr, (unsigned int) 0, 0.0f);

    }

    int Neuron::getReamingSegments() const {
        return reamingSegments;
    }

    int Neuron::getReamingSpines() const {
        return reamingSpines;
    }

    void Neuron::generateSoma() {
        if (numDendrites == 1) { //Special case only 1 dendrite not have any information thus the soma is a placeholder.
            auto firstSec = this->basals[0]->getDendrite()->getSec();
            SamplePoint *firstPoint = (*firstSec)[0];
            SamplePoint *secondPoint = (*firstSec)[1];
            Eigen::Vector3d dir = firstPoint->getPoint() - secondPoint->getPoint();
            dir.normalize();
            Eigen::Vector3d somaCenter = firstPoint->getPoint() + dir;
            this->soma = SamplePoint(somaCenter, 0.9);
        } else {
            Eigen::Vector3d somaBasal(0, 0, 0);
            for (auto &basal: this->basals) {
                auto sec = basal->getDendrite()->getSec();
                somaBasal += (*sec)[0]->getPoint();
            }

            auto Zplane = 0;

            auto somaRadius = 1000000;
            Eigen::Vector3d somaCenter(0, 0, 0);

            if (apical != nullptr) {
                auto secApical = apical->getDendrite()->getSec();
                Eigen::Vector3d somaApical = (*secApical)[0]->getPoint();
                Zplane += somaApical[2];

                somaCenter = (somaApical + somaBasal) / (1 + this->basals.size());

                if ((somaApical - somaCenter).norm() < somaRadius) {
                    somaRadius = (somaApical - somaCenter).norm();
                }

            } else {
                somaCenter = somaBasal / this->basals.size();
            }

            for (auto &basal:this->basals) {
                Eigen::Vector3d firstPoint = (*basal->getDendrite()->getSec())[0]->getPoint();
                Zplane += firstPoint[2];
                if ((firstPoint - somaCenter).norm() < somaRadius)
                    somaRadius = (firstPoint - somaCenter).norm();
            }
            this->soma = SamplePoint(somaCenter, somaRadius);
        }


    }

    std::string Neuron::to_swc(bool spines_) {
        int counter = 1;
        std::stringstream ssSkel;
        std::stringstream ssSpines;
        ssSkel << this->soma.to_swc(counter, -1, 1) << std::endl;
        if (this->apical != nullptr) {
            ssSkel << this->apical->to_swc(counter, spines_);
        }
        for (const auto &basal: this->basals) {
            ssSkel << basal->to_swc(counter, spines_);
        }
        return ssSkel.str();

    }

    void Neuron::spines_to_obj_with_base(std::string dirPath) {
        int i = 0;

        if (!boost::filesystem::exists(dirPath)) {
            boost::filesystem::create_directory(dirPath);
        } else {
            boost::filesystem::remove_all(dirPath);
            boost::filesystem::create_directory(dirPath);
        }
        for (const auto &spine: this->spines) {
            spine->to_obj(dirPath, i);
            i++;
        }

    }

    bool Neuron::isIncorrectConecctions() const {
        return incorrectConecctions;
    }

    void Neuron::spines_to_obj_without_base(std::string dirPath) {
        int i = 0;

        if (!boost::filesystem::exists(dirPath)) {
            boost::filesystem::create_directory(dirPath);
        } else {
            boost::filesystem::remove_all(dirPath);
            boost::filesystem::create_directory(dirPath);
        }
        for (const auto &spine: this->spines) {
            spine->to_obj_without_base(dirPath, i);
            i++;
        }
    }

    void Neuron::removeDuplicates(float threshold) {
        if (this->apical != nullptr) {
            this->apical->removeDuplication(threshold);
        }
        for (const auto &basal: basals) {
            basal->removeDuplication(threshold);
        }
    }

    const spineSet &Neuron::getSpines() const {
        return spines;
    }

    bool Neuron::hasImarisSpines() const {
        return !imarisSpines.empty();
    }

    void Neuron::imarisSpinesToObj(std::string dirPath) {
        if (!boost::filesystem::exists(dirPath)) {
            boost::filesystem::create_directory(dirPath);
        } else {
            boost::filesystem::remove_all(dirPath);
            boost::filesystem::create_directory(dirPath);
        }

        for (size_t i = 0; i < imarisSpines.size(); ++i) {
            const TSpineImaris &imarisSpine = imarisSpines[i];
            std::ofstream file;
            std::string path = dirPath + "/" + std::to_string(i) + ".obj";
            file.open(path, std::ios::out);

            file << "#" << imarisSpine.points.size() << std::endl;
            file << "#" << imarisSpine.faces.size() << std::endl;

            for (const auto &v : imarisSpine.points) {
                file << std::setprecision(5) << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
            }

            for (const auto &face: imarisSpine.faces) {
                file << "f ";
                for (size_t j = 0; j < face.size(); j++) {
                    file << face[j] + 1 << " ";
                }
                file << std::endl;
            }

            file.close();
        }
    }

    const std::vector<TSpineImaris> &Neuron::getImarisSpines() const {
        return imarisSpines;
    }

    const std::string &Neuron::getApiFile() const {
        return apiFile;
        return apiFile;
    }

    const std::string &Neuron::getImarisFile() const {
        return imarisFile;
    }

    const std::vector<std::string> &Neuron::getBasalFiles() const {
        return basalFiles;
    }

    bool Neuron::hasFilamentSpines() const {
        return !spines.empty();
    }

    std::pair<Eigen::Vector3d, Eigen::Vector3d> Neuron::getBB(const TFragment &fragment) {
        Eigen::Vector3d center;
        for (const auto &point: fragment.points) {
            center += point;
        }
        center /= fragment.points.size();

        Eigen::MatrixXd pointMatrix(fragment.points.size(), 3);
        for (size_t i = 0; i < fragment.points.size(); i++) {
            Eigen::Vector3d centeredPoint = fragment.points[i] - center;
            pointMatrix.row(i) = centeredPoint;
        }
        std::cout << pointMatrix << std::endl;
        auto covarianzeMatrix = pointMatrix.transpose() * pointMatrix;
        std::cout << covarianzeMatrix << std::endl;

        Eigen::BDCSVD<Eigen::MatrixXd> svd(covarianzeMatrix, Eigen::ComputeFullU | Eigen::ComputeFullV);
        auto u = svd.matrixU();
        std::cout << u << std::endl;

        Eigen::Matrix4d rotMatrix;
        for (size_t i = 0; i < 3; i++) {
            for (size_t j = 0; j < 3; j++) {
                rotMatrix(i, j) = u(i, j);
            }
            rotMatrix(3, i) = 0;
        }

        rotMatrix(3, 0) = 0.0d;
        rotMatrix(3, 1) = 0.0d;
        rotMatrix(3, 2) = 0.0d;

        rotMatrix(3, 3) = 1;

        std::cout << rotMatrix << std::endl;

        Eigen::Vector3d min = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                               std::numeric_limits<double>::max()};
        Eigen::Vector3d max = {-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
                               -std::numeric_limits<double>::max()};

        for (const auto &point: fragment.points) {
            auto auxPoint = point - center;
            Eigen::Vector4d transformedPoint(auxPoint[0], auxPoint[1], auxPoint[2], 1);
            transformedPoint = rotMatrix * transformedPoint;

            min = min.cwiseMin(transformedPoint.head(3));
            max = max.cwiseMax(transformedPoint.head(3));
        }

        return {min,max};
    }

    float Neuron::computeOverlap(const std::pair<Eigen::Vector3d, Eigen::Vector3d> &BB1,
                                 const std::pair<Eigen::Vector3d, Eigen::Vector3d> &BB2) {

        if (BB1.first[0] > BB2.second[0] || BB1.second[0] < BB2.first[0]){
            return 0;
        }

        if (BB1.first[1] > BB2.second[1] || BB1.second[1] < BB2.first[1]) {
            return 0;
        }

        if (BB1.first[2] > BB2.second[2] || BB1.second[2] < BB2.first[2]){
            return 0;
        }

        auto min = BB1.second.cwiseMin(BB2.second);
        auto max = BB1.first.cwiseMax(BB2.first);

        auto interVolume = (min - max).prod();

        double volume1 = (BB1.second - BB1.first).prod();
       // double volume2 = (BB2.second - BB1.first).prod();
        return interVolume/volume1;
    }
}


















