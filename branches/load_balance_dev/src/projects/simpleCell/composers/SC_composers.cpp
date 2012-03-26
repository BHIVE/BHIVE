/**
 * SC_composers.cpp
 *
 *	@date Jan 29, 2010
 *	@author michael sneddon
 */

#include "SC_composers.hh"
#include "../simpleCell.hh"
#include "../../../input/xmlParser/XmlParser.hh"
#include "../../../util/xml/TinyXML/tinyxml.h"
#include "../../../util/rand/rand.hh"
#include "../../../communicator/communicator.hh"
#include "../../../parallel/parallel.hh"

#include <vector>
#include <string>
#include <fstream> // for ifstream, ofstream, ios_base
#include <iostream>// for cin, cout, endl
#include <exception>
#include <stdexcept>

using namespace std;
using namespace Hive;
using namespace SimpleCell;
using namespace Util;

extern void linearInterpolateGrid(CartesianGrid* grid); 


SimpleCellDefaultComposer::SimpleCellDefaultComposer() {
}

SimpleCellDefaultComposer::~SimpleCellDefaultComposer() {
	outputFile.flush();
	outputFile.close();

	cout << "deleting a SimpleCellDefaultComposer." << endl;
	delete communicator;
}

void SimpleCellDefaultComposer::compose(string inputFileName, string outputFileName) {
    XmlParser xmlParser;
    // World factory
    WorldFactory* worldFactory = new WorldFactory(0);
    xmlParser.addTagParser("World", worldFactory);
    // bactCell factory
    bactCellFactory* cellFactory = new bactCellFactory();
    xmlParser.addTagParser("Cell", cellFactory);
    // now parse file 
    xmlParser.parseFile(inputFileName.c_str(), this->maestro, this->orchestra);

    /*
    // with universe agent
    UniverseFactory* universeFactory = new UniverseFactory(0);
	Agent* universeAgent = universeFactory->createAgent();

    maestro = universeAgent;
    orchestra.push_back(universeAgent);
    universeAgent->setNodeID(0);
    universeAgent->setAgentID(0);

	orchestra.push_back(worldAgent);
	worldAgent->setNodeID(0);
	worldAgent->setAgentID(1);
    worldAgent->setParent(universeAgent->getAgentId());

    universeAgent->addChild(worldAgent->getAgentId());

    // need to replace arg 0 with concentration @ the cell's initial location
    // and replace the second arg with the cell's speed

    for (int i = 0; i < nCells; ++i) {
        Agent* cell = cellFactory->createAgent(0, 0, 0.1); // consumption, concentration, speed
        this->orchestra.push_back(cell);
        cell->setNodeID(0);
        cell->setAgentID(i + 2);

        worldAgent->addChild(cell->getAgentId());
        cell->setParent(worldAgent->getAgentId());
        
        double x = RANDOM(xLen);
        double y = RANDOM(yLen);
        double z = RANDOM(zLen);
        
        grid->addCell(cell->getAgentId(), x, y, z, 0.02);
    }
    */
}

void SimpleCellDefaultComposer::parallelCompose(string inputFileName, string outputFileName) {
    XmlParser xmlParser;
    
	TiXmlDocument document(inputFileName);
	bool loadSucess = document.LoadFile();
	if(!loadSucess) {
		cerr << "Error in loading XML file into composer.";
		exit(1);
	}
    
	TiXmlElement* Root = document.RootElement();
	TiXmlElement* worldNode = 0;
	for(TiXmlNode* child = Root->IterateChildren(0); child;
				child = Root->IterateChildren(child)) {
		if(strcmp(child->Value(), "World") == 0) {
			worldNode = child->ToElement();
			break;
		}
	}
    if (worldNode == 0) {
        cerr << "World element not found." << endl;
    }
    
	int xLen, yLen, zLen;
    double blockSize;

	if ((worldNode->QueryIntAttribute("xLen", &xLen) != TIXML_SUCCESS) ||
        (worldNode->QueryIntAttribute("yLen", &yLen) != TIXML_SUCCESS) ||
        (worldNode->QueryIntAttribute("zLen", &zLen) != TIXML_SUCCESS) ||
        (worldNode->QueryDoubleAttribute("blockSize", &blockSize) != TIXML_SUCCESS)) {
		cerr << "Error in parsing world size into composer.";
		exit(1);
	}

    UniverseFactory* universeFactory = new UniverseFactory(0);
	Agent* universeAgent = universeFactory->createAgent();

    UniverseDatabase* univDB = (UniverseDatabase*)universeAgent->getDatabase()->getDataItem("univ");

    vector<int> myLens(3, 0);
    vector<double> startingPosition(3, .0);
    
    configureMyWorld(/* in  */ mpirank(), mpisize(), xLen, yLen, zLen, blockSize,
                     /* out */ myLens, startingPosition, univDB);
    
	WorldFactory* worldFactory = new WorldFactory(0);
	Agent* worldAgent = worldFactory->createAgent();
	CartesianGrid* grid = (SimpleCell::CartesianGrid*) worldAgent->getDatabase()->getDataItem(GRID);

	grid->createGrid(myLens[0], myLens[1], myLens[2], blockSize, startingPosition);
	grid->configureBoundaryCells(univDB);

    int nCells = 100;
    string cellLocations;
	map<string, int> compoundMap;
	vector<vector<vector<vector<double> > > > concentrationSet;

    // initialize walls - all edge sides are walls
    vector<vector<int> > wallCoords; 
    for (int i=0; i < 3; i++) { // do z=0 & z=Len(3)-1 walls first, then x, then y
        for (int j=0; j < grid->Len(i); j++) { // j ranges over Len of x, y, then z
            for (int k=0; k < grid->Len((i+1)%3); k++) { // k ranges over y, z, then x
                vector<int> wallCoord_1(3,0); 
                vector<int> wallCoord_2(3,0); 
                wallCoord_1[i] = j;
                wallCoord_1[(i+1) % 3] = k;
                wallCoord_1[(i+2) % 3] = 0; // (i+2) % 3 is 2, then 0, then 1
                wallCoord_2[i] = j;
                wallCoord_2[(i+1) % 3] = k;
                wallCoord_2[(i+2) % 3] = int(grid->Len((i+2) % 3)-1);
                wallCoords.push_back(wallCoord_1);
                wallCoords.push_back(wallCoord_2);
            }
        }
    }
    //grid->setWalls(wallCoords);

	// parse compound concentrations into vector
	int index = 0;
	for(TiXmlNode* node = worldNode->IterateChildren(0); node;
        node = worldNode->IterateChildren(node) ) {
		if (strcmp(node->Value(), "Cells") == 0) {
			TiXmlElement* cellsEl = node->ToElement();
			const char* locations = cellsEl->Attribute("locations");
            if (!locations || strcmp(locations, "Random") != 0) {
                cerr << "Only type of locations supported for now: Random" << endl;
                exit(-1);
            }
            cellLocations = locations;
            if (cellsEl->QueryIntAttribute("number", &nCells) != TIXML_SUCCESS) {
                cerr << "Error parsing number of cells" << endl;
                exit(-1);
            }
		} else if (strcmp(node->Value(), "Compound") == 0) {
			TiXmlElement* compoundEl = node->ToElement();
			const char* cpdName = compoundEl->Attribute("name");
			double diffusion = 1;
			double defaultconc = 0;
            if (compoundEl->Attribute("diffusion") != NULL) {
               diffusion = atof(compoundEl->Attribute("diffusion"));
            }
			if (compoundEl->Attribute("defaultconc") != NULL) {
               defaultconc = atof(compoundEl->Attribute("defaultconc"));
            }
			if (cpdName == NULL) {
				cerr << "Error in parsing compound!\n";
				continue;
			}
			if(compoundMap.count(cpdName) == 0) {
				compoundMap[cpdName] = grid->addCompound((string) cpdName,diffusion,defaultconc);
			}
			for(TiXmlNode* voxelNode = node->IterateChildren(0); voxelNode;
						voxelNode = node->IterateChildren(voxelNode) ) {
				if (strcmp(voxelNode->Value(), "Voxel") != 0) {
					continue;
				}
				TiXmlElement* voxel = voxelNode->ToElement();
				int x, y, z;
				if ((voxel->QueryIntAttribute("x", &x) != TIXML_SUCCESS) ||
                    (voxel->QueryIntAttribute("y", &y) != TIXML_SUCCESS) ||
                    (voxel->QueryIntAttribute("z", &z) != TIXML_SUCCESS) ) {
					cerr<< "Error parsing voxel data in compound: "<<cpdName<<"\n";
					continue;
				}
				for(TiXmlNode* concentrationNode = voxel->IterateChildren(0); concentrationNode;
							concentrationNode = voxel->IterateChildren(concentrationNode)) {
					if (strcmp(concentrationNode->Value(), "Concentration") != 0) {
						cerr << "Error in parsing concentration in compound: "<<cpdName<<"\n";
					}
					TiXmlElement* concentrationElement = concentrationNode->ToElement();
                    Voxel* voxel = grid->getVoxel(x,y,z);
                    grid->setGridConcentration(voxel, compoundMap[cpdName], atof(concentrationElement->GetText()));
				}
			}
		}
		index++;
	}
    

    //grid->printContent(cout);
    //linearInterpolateGrid(grid); 
    // grid->printContent(cout);


    // with universe agent
    maestro = universeAgent;
    orchestra.push_back(universeAgent);
    // universeAgent->setNodeID(mpirank());
    universeAgent->setAgentID(0);

	orchestra.push_back(worldAgent);
	// worldAgent->setNodeID(mpirank());
	worldAgent->setAgentID(1);
    worldAgent->setParent(universeAgent->getAgentId());

    universeAgent->addChild(worldAgent->getAgentId());

    bactCellFactory* cellFactory = new bactCellFactory();
    // need to replace arg 0 with concentration @ the cell's initial location
    // and replace the second arg with the cell's speed

    SEED_RANDOM(mpirank());

    for (int i = 0; i < nCells; ++i) {
        Agent* cell = cellFactory->createAgent(0, 0, 0.1+Util::RANDOM(0.5)); // consumption, concentration, speed
        this->orchestra.push_back(cell);

        worldAgent->addChild(cell->getAgentId());
        cell->setParent(worldAgent->getAgentId());

        double x = startingPosition[0] + RANDOM(myLens[0] * blockSize);
        double y = startingPosition[1] + RANDOM(myLens[1] * blockSize);
        double z = startingPosition[2] + RANDOM(myLens[2] * blockSize);
        
        grid->addCell(cell->getAgentId(), x, y, z, 0.000001);
    }
}

void SimpleCellDefaultComposer::configureMyWorld(
    /* in  */ int rank, int size, int xLen, int yLen, int zLen, double blockSize,
    /* out */ vector<int>& myLens, vector<double>& startingPosition, UniverseDatabase* univDB) 
{
    myLens.resize(3);
 
    myLens[0] = xLen;
    myLens[1] = yLen;
    myLens[2] = zLen;

    univDB->sizes.clear();
    for (unsigned int i = 0; i < 3; ++i) {
        univDB->sizes.push_back(myLens[i] * blockSize);
    }
    
    vector<int> chunks(3, 1);
    // int temp = 32;
    int temp = size;
    while (temp > 1) {
        if (temp > 1 && (temp & 1) == 1) {
            parallel_abort(-1, "Error: MPI size should be a power of 2");
        }
        temp >>= 1;
        int i = 0;
        if (xLen * 1./chunks[0] < yLen * 1./chunks[1] || xLen * 1./chunks[0] < zLen * 1./chunks[2]) {
            i = (yLen * 1./chunks[1] >= zLen * 1./chunks[2]) ? 1 : 2;
        }
        chunks[i] <<= 1;
    }

    vector<int> coord(3);          
    coord[2] =   rank / (chunks[0] * chunks[1]);
    coord[1] =  (rank % (chunks[0] * chunks[1])) / chunks[0];
    coord[0] =  (rank % (chunks[0] * chunks[1])) % chunks[0];

    setmpicoord(0, coord[0]);
    setmpicoord(1, coord[1]);
    setmpicoord(2, coord[2]);
    
    vector<int> nBlocks(3);
    for (int i = 0; i < 3; ++i) {
        // nBlocks[i] = int((myLens[i] - 1e-8) / blockSize) + 1;
        nBlocks[i] = myLens[i];
        myLens[i]  = (nBlocks[i] - 1) / chunks[i] + 1;
        startingPosition[i] = coord[i] * myLens[i] * blockSize;
        if (coord[i] == chunks[i] - 1 && nBlocks[i] % chunks[i] > 0)
            myLens[i] = nBlocks[i] % ((nBlocks[i] - 1) / chunks[i] + 1);
    }

    if (rank == 0) {
        cout << "universe chunks = " << chunks[0]  << "," << chunks[1]  << "," << chunks[2]  << endl;
        // cout << "universe blocks = " << nBlocks[0] << "," << nBlocks[1] << "," << nBlocks[2] << endl;
    }
    parallel_barrier();
    
    univDB->worldStartingPositions = startingPosition;
    univDB->worldSizes.clear();
    for (unsigned int i = 0; i < 3; ++i) {
        univDB->worldSizes.push_back(myLens[i] * blockSize);
    }

    cout << "world [" << rank << "] coord = " << coord[0]   << "," << coord[1]   << "," << coord[2]   << endl;
    cout << "world [" << rank << "] sizes  = " << myLens[0]*blockSize  << "," << myLens[1]*blockSize  << "," << myLens[2]*blockSize  << endl;
    cout << "world [" << rank << "] start = " << startingPosition[0] << "," << startingPosition[1] << "," << startingPosition[2] << endl;

    univDB->neighbors.clear();
    int d[] = {-1, 0, 1};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                vector<int> nc(3); // neighbor coord
                nc[0] = coord[0] + d[k];
                nc[1] = coord[1] + d[j];
                nc[2] = coord[2] + d[i];
                for (int c = 0; c < 3; ++c) {
                    if (nc[c] < 0) nc[c] += chunks[c];
                    if (nc[c] == chunks[c]) nc[c] = 0;
                }
                int index = (nc[2] * chunks[0] * chunks[1]) + (nc[1] * chunks[0]) + nc[0];
				univDB->neighborMap[i][j][k][0] = index;
				
				//We need to preserve the size of the neighboring blocks as well
				for (int m = 0; m < 3; ++m) {
					univDB->neighborMap[i][j][k][m+1] = (nBlocks[m] - 1) / chunks[m] + 1;
					if (nc[m] == chunks[m] - 1 && nBlocks[m] % chunks[m] > 0)
						univDB->neighborMap[i][j][k][m+1] = nBlocks[m] % ((nBlocks[m] - 1) / chunks[m] + 1);
				}

                univDB->neighbors.push_back(index);
                printf("world [%d] neighbor[%2d] = (%2d,%2d,%2d) [%d %d %d]: %d\n",
                       rank, univDB->neighbors.size()-1, d[k], d[j], d[i], nc[0], nc[1], nc[2], index);
                // cout << "world [" << rank << "] neighbors (" << d[i] << "," << d[j] << "," << d[k] << ") = " <<  nc[0] <<" "<< nc[1] <<" "<< nc[2] << ": " << index << endl;
            }
        }
    }
}

void SimpleCellDefaultComposer::parseInputFile(string inputFileName)
{
}

void SimpleCellDefaultComposer::outputNow(double currentTime)
{
	/*// output format:
	// cellNumber, time, cellState, CheYp, motState[0], x,y,z


	//Create all of the SimpleCells
	for(unsigned int k=1; k<orchestra.size(); k++) {
		Agent *cellAgent = orchestra.at(k);
		Database *db = cellAgent->getDatabase();

		// cellNumber
		double kDouble = (double)k;
		outputFile.write((char *)&kDouble,sizeof(double));

		// time
		outputFile.write((char *)&currentTime,sizeof(double));


		//cell state
		double cellState = (double)((IntegerData *)db->getDataItem("CurrentCellState"))->getInteger();
		outputFile.write((char *)&cellState, sizeof(double));


		//cheYp
		double ypConc = ((DoubleVectorData *)db->getDataItem("CheYp"))->getDouble(0);
		outputFile.write((char *)&ypConc, sizeof(double));

		//cheYp
		double motState = (double)((IntVectorData *)db->getDataItem("MotorStates"))->getInt(0);
		outputFile.write((char *)&motState, sizeof(double));

		DoubleVectorData *curPos = ((DoubleVectorData *)db->getDataItem("Position"));
		double x = curPos->getDouble(1);
		double y = curPos->getDouble(2);
		double z = curPos->getDouble(3);

		outputFile.write((char *)&x, sizeof(double));
		outputFile.write((char *)&y, sizeof(double));
		outputFile.write((char *)&z, sizeof(double));


		double lig = ((DoubleVectorData *)db->getDataItem("Chemical"))->getDouble(0);
		outputFile.write((char *)&lig, sizeof(double));
	}
    */
}

void SimpleCellDefaultComposer::updateAllChemicalConc(double c)
{
	cout<<"In SimpleCellDefaultComposer, updating all concentrations to : "<<c<<'M'<<endl;
	for (unsigned int i=1; i<this->orchestra.size(); i++)
	{
		Hive::Message *m = new Message();
		DoubleVectorData *dvd = new DoubleVectorData("NewChemical",1);
		dvd->setDouble(c,0);
		m->setArgument(dvd);
		m->setAction(0);
		m->setDestination(i);

		communicator->sendMessage(m);
	}
	cout<<"all messages sent.\n";
}


// Not called for the SimpleCellDefaultComposer
void SimpleCellDefaultComposer::initializeAgentFactories() { }


void SimpleCellDefaultComposer::setupAgentHierarchy()
{
	//Create all of the SimpleCells
	for(unsigned int k=1; k<orchestra.size(); k++) {
		Agent *cellAgent = orchestra.at(k);
		maestro->addChild(cellAgent->getAgentId());
		cellAgent->setParent(maestro->getAgentId());
	}
}


Agent* SimpleCellDefaultComposer::createAgent(AgentFactory& af) {
	cout<<"for now, do not use: call SimpleCellDefaultComposer::createAgent(AgentFactory& af)"<<endl;
	exit(1);
	return af.createAgent();
}


void SimpleCellDefaultComposer::addCommunicator()
{
    Communicator* comm = new Communicator();
	communicator = comm;
}

void SimpleCellDefaultComposer::setupSimulation()
{
    Agent* agent = getTopLevelAgent();
    if (agent == NULL) {
        cerr << "Error: setupCommunicator() called when universe is not set up";
        return;
    }
    UniverseDatabase* univDB = (UniverseDatabase*)agent->getDatabase()->getDataItem("univ");
    Communicator* comm = getCommunicator();
    comm->setupAgentMap(agent->getPropagator()->getAgentMapPointer());
    comm->setupNeighbors(univDB->neighbors);
}
