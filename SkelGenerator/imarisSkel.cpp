
#include <set>
#include "vrmlReader.cpp"


using namespace std;



namespace skelgenerator {

#define BELONGS_TO_SET(set, elem) set.find(elem) != set.end()


    int ComputeSoma(vector <TSegment> apical, vector <vector<TSegment>> basal, Eigen::Vector3f &somaCenter,
                    float &somaRadius) {

        int nbBasals = static_cast<int>(basal.size());


        vector <Eigen::Vector3f> listOfBasals;

        //ADDED: Acumulador del primer punto de todos los segmentos basales
        Eigen::Vector3f somaBasal(0, 0, 0);


        // ADDED: Guarda los 3 primeros puntos de todos los segmentos basales y los guarda en listOfBasal , ademas los acumula en somaBasal
        for (int i = 0; i < nbBasals; i++) {
            somaBasal += basal[i][0].points[0];
            listOfBasals.push_back(basal[i][0].points[0]);
        }

        somaRadius = 1000000;

        if (!apical.empty()) {
            Eigen::Vector3f somaApical = apical[0].points[0];


            //ADDED: Suma el primer punto de los apicales con la acumulacion de los puntos de los basales y divide entre el numero de segmetos
            somaCenter = (somaApical + somaBasal) / (1 + nbBasals);

            //ADDED: si la distancia del segmento apical al centro del soma es menor que el actual actualiza el valor
            if ((somaApical - somaCenter).norm() < somaRadius)
                somaRadius = (somaApical - somaCenter).norm();
        } else {
            somaCenter = somaBasal / nbBasals;
        }


        //ADDED: Igual que el anterior pero con todos los elementos basales
        for (int i = 0; i < listOfBasals.size(); i++) {
            if ((listOfBasals[i] - somaCenter).norm() < somaRadius)
                somaRadius = (listOfBasals[i] - somaCenter).norm();
        }

    }

    std::tuple<int, int>
    ComputeSkel(vector <TSegment> segments, int initCounter, int dendriteType, std::ofstream &skelFile,
                float connectionTreshold) {

        // Guarda el padre donde se tendran que conectar los segmentos (seg->punto->padre)
        map<int, map<int, int>> parentsConns;
        int nbSegs = static_cast<int>(segments.size());
        double x, y, z, vol;

        Eigen::Vector3f medioAnt(0, 0, 0);
        Eigen::Vector3f medio(0, 0, 0);



        //ADDED: Contiene el punto medio de cada circulo, para cada segmento
        vector <Eigen::Vector3f> skel[nbSegs];
        //ADDED: Contiene el radio de cada circulo para cada segmento
        vector<float> radius[nbSegs];

        int nbConnPerSeg[nbSegs];
        for (int i = 0; i < nbSegs; i++)
            nbConnPerSeg[i] = 0;

        //   cout << "Nuevo fichero" << endl;

        //ADDED: Rellena los arrays anteriores
        for (int f = 0; f < nbSegs; f++) {

            //ADDED: Añade los puntos medios de cada circulo de un segmento al array
            for (int cir = 0; cir < segments[f].nCircles; cir++) {
                // cout << "Cir: " << cir << " ";
                medio = Eigen::Vector3f(0, 0, 0);
                //ADDED: Suma todos los puntos para despues obtener el medio a partir de esta suma.
                for (int i = 0; i < 17; i++) {
                    medio += segments[f].points[cir * 17 + i];
                }

                medio = medio / 17;
                //      cout << medio[0] << " " << medio[1] << " " << medio[2] << endl;

                //ADDED: Añade el medio del circulo al segmento que se estra procesando
                skel[f].push_back(Eigen::Vector3f(medio[0], medio[1], medio[2]));
                //ADDED: Añade el radio
                radius[f].push_back(
                        (Eigen::Vector3f(medio[0], medio[1], medio[2]) -
                         segments[f].points[cir * 17 + 1]).norm());


                medioAnt = medio;
            }
        }


        //ADDED:TsegToProcess tiene seg que es el numero del segmento,initPoint e idx que es el identificador del padre

        stack <TSegToProcess> segsToProcess;

        // Se inserta un segmento inicial
        segsToProcess.push(TSegToProcess{0, 0, 0});

        /*ADDED: Tcons contiene p1,s1,p2,s2 donde p1 y p2 son los numeros de los segmentos involucrados y s1 es el indice del primer circulo que conecta del
        * segmento(No se usa para nada) y s2 es el circulo que conecta del otro segmento */
        vector <TConn> conns;
        set<int> processedSegs;
        processedSegs.insert(0);
        int counter = initCounter;

        // Se procesan los segmentos mientras no quede ninguno en la pila
        while (!segsToProcess.empty()) {

            TSegToProcess stp = segsToProcess.top();
            //ADDED: Numero de segmento
            int seg = stp.seg;
            segsToProcess.pop();
            // cout << " eltos por procesar " << segsToProcess.size();
            // cout << "Mirando segmento " << s << endl;

            int commonMini = -1;

            // Se calcula la distancia minima con el resto de segmentos
            for (int f = 0; f < nbSegs; f++) {
                if (f == seg)
                    continue;
                float min = 10000;
                int minf = 0;
                int minj = 0;
                int mini = 0;
                //ADDED: Compara la distancia de todos los medios de un segmento con el otro
                for (int i = 0; i < skel[seg].size(); i++) {
                    for (int j = 0; j < skel[f].size(); j++) {

                        float d = (skel[f][j] - skel[seg][i]).norm();
                        //ADDED: Si es la distancia mas pequeña guarda los indices de los medios mas proximos
                        if (d < min) {
                            min = d;
                            mini = i;
                            minj = j;
                        }
                    }
                }

                // Si la distancia es menor que un umbral y el segmento no ha
                // sido procesado previamente se considera que hay conexion
                if (min < connectionTreshold) {
                    //ADDED: Comprueba si el segmento esta en la lista de procesados y lo añade sino.
                    if (!(BELONGS_TO_SET(processedSegs, f))) {
                        if (commonMini == -1)
                            commonMini = mini;
                        // cout << " Posible conexion " << s << "-" << f
                        //      << "  (" << s << "," << mini << "-" << commonMini << ") contra
                        //      ("
                        //      << f << "," << minj << ") d = " << min << endl;
                        //    conns.push_back(TConn({s,commonMini,f,minj+1}));
                        // if (nbConnPerSeg[s] > 2)  {
                        //   cout << "Segmento " << s << " con " << nbConnPerSeg[s] << "
                        //   conexiones "
                        //   << "(" << s << "," << mini << ") "
                        //   << "(" << f << "," << minj+1 << ")" << endl;
                        //   mini++;
                        // }
                        conns.push_back(TConn({seg, commonMini, f, minj + 1, mini}));
                        nbConnPerSeg[seg]++;
                        processedSegs.insert(f);
                    }
                }
            }

            skelFile << "# Segmento " << seg << " file: " << segments[seg].nombre << endl;

            // Si la rama esta del "reves" le damos la vuelta
            //if(stp.initPoint > conns[0].p1){
            if (stp.initPoint > skel[seg].size() / 2) {
                reverse(skel[seg].begin(), skel[seg].end());
                unsigned long size = skel[seg].size();
                stp.initPoint = static_cast<int>(size - stp.initPoint);
                for (auto &conn : conns) {
                    conn.p1 = static_cast<int>(size - conn.p1);
                    conn.index = static_cast<int>(size - conn.index);

                }

                if (parentsConns.find(seg) != parentsConns.end()) {
                    map<int, int> auxMap;
                    for (auto entry : parentsConns.at(seg)) {
                        auxMap.emplace(size - entry.first, entry.second);
                    }
                    parentsConns.at(seg).swap(auxMap);

                }


            }
            // Si la rama no tiene conexiones es que es terminal
            // y se imprimen sus nodos
            if (conns.empty()) {
                // cout << s << " es rama terminal" << endl;
                // cout << "Desde " << counter << "(" << s << "," << stp.initPoint << ")
                // hasta ";
                cout << "Segmento " << seg << "(" << stp.initPoint << ")"
                     << " SI terminal: desde " << counter + 1 << " hasta "
                     << counter + skel[seg].size() - stp.initPoint << endl;
                for (int p = stp.initPoint; p < skel[seg].size(); p++) {
                    counter++;

                    int parent = ((p == stp.initPoint) ? ((stp.idx == 0) ? 1 : stp.idx)
                                                       : ((counter == 1) ? 1 : counter - 1));

                    // Si tiene un padre asignado debido a una conexion se sobreescribe el padre
                    if (parentsConns.find(seg) != parentsConns.end()) {
                        if (parentsConns.at(seg).find(p) != parentsConns.at(seg).end()) {
                            parent = parentsConns.at(seg).at(p);
                        }
                    }

                    //    if (p == stp.initPoint || p == (skel[s].size()-1))
                    skelFile << counter << " " << dendriteType << " " << skel[seg][p][0]
                             << " " << skel[seg][p][1] << " " << skel[seg][p][2] << " "
                             << radius[seg][p] << " "
                             << parent
                             << endl;
                }
                // cout << counter << endl;
                // cout << "Desde " << counter << "(" << s << "," << skel[s].size() << ")"
                // << endl;
            } else {
                // Si la rama no es terminal y tiene conexiones

                cout << "Segmento " << seg << "(" << stp.initPoint << ")"
                     << " NO terminal: desde " << counter + 1 << " hasta "
                     << counter + conns[0].p1 - stp.initPoint << endl;

                for (int p = stp.initPoint; p < skel[seg].size(); p++) {
                    counter++;
                    int parent = ((p == stp.initPoint) ? ((stp.idx == 0) ? 1 : stp.idx)
                                                       : ((counter == 1) ? 1 : counter - 1));


                    for (auto &i : conns) {
                        if (p == i.index) {
                            auto it = parentsConns.find(i.s2);
                            if (it == parentsConns.end()) {
                                parentsConns.emplace(i.s2, map<int, int>());
                            }
                            parentsConns.find(i.s2)->second.emplace(i.p2, counter - 1);

                        }

                    }



                    // Si en la tabla de padres este segemntos con el punto actual tiene un padre predefinido se sobreescribe
                    if (parentsConns.find(seg) != parentsConns.end()) {
                        if (parentsConns.at(seg).find(p) != parentsConns.at(seg).end()) {
                            parent = parentsConns.at(seg).at(p);
                        }
                    }

                    skelFile << counter << " " << dendriteType << " " << skel[seg][p][0]
                             << " " << skel[seg][p][1] << " " << skel[seg][p][2] << " "
                             << radius[seg][p] << " "
                             << parent
                             << endl;

                }

                for (int c = conns.size() - 1; c >= 0; c--) {
                    segsToProcess.push(
                            TSegToProcess{conns[c].s2, conns[c].p2, counter});

                }

            }
            conns.clear();
        }

        return std::make_tuple(counter, nbSegs - processedSegs.size());
    }


    int processSkel(std::string apiFile, std::vector <std::string> basalFile, std::string resultFile,
                    float connectionThreshold) {
        std::ofstream skelFile;
        int counter = 1;
        Eigen::Vector3f somaPos;
        float somaRadius;

        std::vector <TSegment> segmentsApical;
        if (apiFile != "") {
            segmentsApical = readVrmlApical(apiFile);
        }

        auto segmentsBasal = readVrmlBasal(basalFile[0]);

        cout << "------------------------ LEYENDO -------------------------" << std::flush;

        for (int i = 1; i < basalFile.size(); i++) {
            auto currentBasal = readVrmlBasal(basalFile[i]);
            segmentsBasal.insert(segmentsBasal.end(), currentBasal.begin(), currentBasal.end());
        }

        ComputeSoma(segmentsApical, segmentsBasal, somaPos, somaRadius);
#define SWC_SOMA 1
#define SWC_APICAL 4
#define SWC_BASAL 3

        skelFile.open(resultFile, std::ios::out);
        skelFile << "1 " << SWC_SOMA << " "
                 << somaPos[0] << " "
                 << somaPos[1] << " "
                 << somaPos[2] << " "
                 << somaRadius
                 << " -1" << endl;


        int sobrantes = 0;
        int auxSobrantes = 0;
        std::tuple<int, int> tuple;


        if (!segmentsApical.empty()) {
            tuple = ComputeSkel(segmentsApical, 1, SWC_APICAL, skelFile, connectionThreshold);
            counter = std::get<0>(tuple);
            sobrantes = std::get<1>(tuple);
        }



        //counter = ComputeSkel(segmentsApical2,counter,SWC_APICAL);



        for (auto &i : segmentsBasal) {
            tuple = ComputeSkel(i, counter, SWC_BASAL, skelFile, connectionThreshold);
            counter = std::get<0>(tuple);
            auxSobrantes = std::get<1>(tuple);
            sobrantes += auxSobrantes;
        }


        skelFile.close();
        cout << "------------------------------------------------ " << sobrantes
             << " --------------------------------------" << std::flush;
        return sobrantes;

    }
}
