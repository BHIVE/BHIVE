/*
 * universe.cpp - Universe Agent
 * 
 * Author: fangfang
 */

#include "universe.hh"
#include "../../../parallel/parallel.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;

void SimpleCell::FluxConcData_packDataForMPI(FluxConcData* data, char*& buffer, int& size) {
	stringstream ss;
	ss << data->sourceID << endl;
	ss << data->destID << endl;
	ss << data->sourceCPU << endl;
	ss << int(data->sourceConc.size()) << endl;
	for (int i=0; i < int(data->sourceConc.size()); i++) {
		ss << data->sourceConc[i] << endl;
		ss << data->fluxToDest[i] << endl;
	}
	//Creating the buffer from the stringstream
	buffer = strdup(ss.str().c_str());
    size = strlen(buffer);
}

FluxConcData* SimpleCell::FluxConcData_unpackDataForMPI(char* buffer, int size) {
	stringstream ss(buffer);
    FluxConcData* data = new FluxConcData;
	ss >> data->sourceID;
	ss >> data->destID;
	ss >> data->sourceCPU;
	int vsize = 0;
	ss >> vsize;
	data->sourceConc.resize(vsize);
	data->fluxToDest.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> data->sourceConc[i];
		ss >> data->fluxToDest[i];
	}
	return data;
}

/*
 * Universe Database
 *
 */
void UniverseDatabase::addCellsInLimbo(WorldUniverseData* worldData) {
    vector<CellWorldData*> cells = worldData->getWorldCellData();
    vector<BacteriaDatabase*> dbs   = worldData->getSingleCellData();

    for (unsigned int i = 0; i < cells.size(); ++i) {
        
        int destNode = getCellDestinationNode(cells[i]);
        worldCellsMap[destNode].push_back(cells[i]);
        singleCellsMap[destNode].push_back(dbs[i]);
    }
}

void UniverseDatabase::clearCellsMap(bool deleteContents) {
	if (deleteContents) {
		for (map<int, vector<CellWorldData*> >::iterator it = worldCellsMap.begin(); it != worldCellsMap.end(); ++it) {
			for (int i=0; i < int(it->second.size()); i++) {
				delete it->second[i];
			}
		}
		for (map<int, vector<BacteriaDatabase*> >::iterator it = singleCellsMap.begin(); it != singleCellsMap.end(); ++it) {
			for (int i=0; i < int(it->second.size()); i++) {
				delete it->second[i];
			}
		}
	}
    worldCellsMap.clear();
    
    singleCellsMap.clear();
}

void UniverseDatabase::clearFluxConcMap() {
	for (map<int, vector<FluxConcData*> >::iterator it = fluxConcMap.begin(); it != fluxConcMap.end(); ++it) {
		for (unsigned int i=0; i < it->second.size(); i++) {
			delete it->second[i];
			it->second.clear();
		}
    }
}

int UniverseDatabase::getCellDestinationId(CellWorldData* cell) {
    return 1;
} 

int UniverseDatabase::getCellDestinationNode(CellWorldData* cell) {
    int index = 0;
    for (int i = 2; i >= 0; --i) {
        double& c = cell->coordinates[i];
        double size = worldSizes[i];
        int destCoord = 1;
        if (c <= worldStartingPositions[i]) {
            c -= 1e-5;          // avoids floating point problems
            destCoord = 0;
            if (c <= .0) c += sizes[i];
        } else if (c >= worldStartingPositions[i] + size) {
            c += 1e-5;          // avoids floating point problems
            destCoord = 2;
            if (c >= sizes[i]) c -= sizes[i];
        }
        index = index * 3 + destCoord;
    }
    // printf("getCellDestinationNode [ index = %d ]  =  %d\n", index, neighbors[index]);
    return neighbors[index];
} 

/*
 * Universe Simulators
 *
 */
UniverseSimulator::UniverseSimulator() {};
UniverseSimulator::~UniverseSimulator() {};

void UniverseSimulator::initialise() {};
void UniverseSimulator::step(Agent *ag, double t) {};

/*
 * Universe AgentFactory
 *
 */
UniverseFactory::UniverseFactory(const int type):AgentFactory() {
	simulatorType = type;
}

UniverseFactory::UniverseFactory(InputSystemReader *isr):AgentFactory(isr) {}

UniverseFactory::~UniverseFactory() {}

Agent *UniverseFactory::createAgent() {
	Agent *a = new Agent();
	GenerateUniverseMessageToWorld* msgGen = new GenerateUniverseMessageToWorld(a);
	a->addMessageGenerator(msgGen);
	GenerateUniverseFluxMessageToWorld* msgGenTwo = new GenerateUniverseFluxMessageToWorld(a);
	a->addMessageGenerator(msgGenTwo);
	UniverseDatabase* universityDB = new UniverseDatabase();
	a->getDatabase()->addData("univ", universityDB);
	addSimulatorToAgent(a);	
	SyncWorldToUniverse* universeAction = new SyncWorldToUniverse();
	a->addAction(universeAction);
	RecvFluxMessageFromWorld* fluxAction = new RecvFluxMessageFromWorld();
	a->addAction(fluxAction);
	return a;
};

void UniverseFactory::addActionsToAgentsActionSet(Agent *ag)
{
	SyncWorldToCell* newAction = new SyncWorldToCell();
	ag->addAction(newAction);
	RecvFluxMessageFromWorld* fluxAction = new RecvFluxMessageFromWorld();
	ag->addAction(fluxAction);
}

void UniverseFactory::addSimulatorToAgent(Agent *ag) {
	UniverseSimulator* univSim = new UniverseSimulator();
	ag->addSimulator(univSim);
};

Agent *UniverseFactory::createAgent(Action **ac) {
	return NULL;
};

Agent *UniverseFactory::duplicateAgent(Agent *ag) {
	return 0;
};

void UniverseFactory::finalise() {};
