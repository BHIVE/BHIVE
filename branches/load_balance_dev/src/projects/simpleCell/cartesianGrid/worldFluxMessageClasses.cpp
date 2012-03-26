/*
 * @date June 5, 2010
 * @author Christopher Henry
 * DESCRIPTION: This file contains the database, generator, and action associated with the message 
 * sent by the world to the universe to in order to update the world's neighbors about the state of
 * the voxels located on the boundary of the world
 */

#include "worldFluxMessageClasses.h" 

using namespace std;
using namespace Hive;
using namespace SimpleCell;

WorldFluxMessage::~WorldFluxMessage() {
	//Note we are specifically NOT deleting the FluxConcData items since they are needed elsewhere
	//This data WILL be deleted by the UniverseDatabase::clearFluxConcMap() function when RecvFluxMessageFromWorld::fire is called next
};

void WorldFluxMessage::packDataForMPI(char*& buffer, int& size) {
	stringstream ss;
	ss << fluxConcMap.size() << endl;
	for (map<int,vector<FluxConcData*> >::iterator IT = fluxConcMap.begin(); IT != fluxConcMap.end(); IT++) {
		ss << IT->first << endl;
		ss << IT->second.size() << endl;
		for (unsigned int i=0; i < IT->second.size(); i++) {
			char* buff;
			int bsize;
			FluxConcData_packDataForMPI(IT->second[i],buff,bsize);
			ss << bsize << endl;
			ss << buff;
			delete [] buff;
		}
	}
	//Creating the buffer from the stringstream
	buffer = strdup(ss.str().c_str());
    size = strlen(buffer);
};

void WorldFluxMessage::unpackDataForMPI(char* buffer, int size) {
	stringstream ss(buffer);
	int vsize;
	ss >> vsize;
	for (int i=0; i < vsize; i++) {
		int cpu;
		ss >> cpu;
		int numdata;
		ss >> numdata;
		for (int i=0; i < numdata; i++) {
			int bsize;
			ss >> bsize;
			char* buff = new char[bsize+1];		
			string one;
			string two;
			while (int(one.length()) < bsize) {
				ss >> two;
				one.append(two);
				one.append("\n");
			}
			strcpy(buff,one.data());
			FluxConcData* d = FluxConcData_unpackDataForMPI(buff,bsize);
			fluxConcMap[cpu].push_back(d);
			delete [] buff;
		}
	}
};

//This class generates the message that transfers information about flux between worlds to the universe
void GenerateWorldFluxMessageToUniverse::placeMessage(int destID, int node) {
	//Creating message and setting standard message data
	Message *msg = new Message();
	msg->setAction(1);
	msg->setDestination(destID,node);
	msg->setSourceID(source->getAgentId());
	//Instantiating the message database
	WorldFluxMessage* msgDB = new WorldFluxMessage();
	//Obtaining a pointer to CartesianGrid database
	CartesianGrid* worldDB = (CartesianGrid*)source->getDatabase()->getDataItem(GRID);
	//Getting a list of all boundary voxels and bundling them into a message
	vector<Voxel*> edgeVoxels = worldDB->getEdgeVoxels();
	for (int i=0; i < int(edgeVoxels.size()); i++) {
		for (int j=0; j < 3; j++) {
			if (edgeVoxels[i]->coordinates[j] == 0 || edgeVoxels[i]->coordinates[j] == (worldDB->Size(j)-1)) {
				vector<int> coord = edgeVoxels[i]->coordinates;
				if (edgeVoxels[i]->coordinates[j] == 0) {
					coord[j] = -1;
				} else {
					coord[j]++;
				}
				Voxel* v = worldDB->getVoxel(coord);
				if (v->bv != NULL && v->bv->cpu != -1) {
					FluxConcData* newData = new FluxConcData;
					newData->sourceID = worldDB->getVoxelID(edgeVoxels[i]);
					newData->sourceConc = edgeVoxels[i]->compoundConcentrations;
					newData->sourceCPU = source->getAgentNode();
					newData->destID = v->bv->id;
					int destCPU = v->bv->cpu;
					newData->fluxToDest = v->bv->netFlux;
					msgDB->addFluxConcData(newData,destCPU);
				}
			}
		}
	}
	//Packing the message just to ensure that the packaging is working
	char* temp = NULL;
	int size;
	msgDB->packDataForMPI(temp, size);
	RawBufferData* rbd = new RawBufferData("WorldFluxData", temp, size);
    msg->setArgument(rbd);
    free(temp);//match strdup in packDataForMPI
	//msg->setArgument((Data*)msgDB);
	source->placeMessageInOutbox(msg);
};

void RecvFluxMessageFromWorld::fire (Agent* ag, Data *d, int source) {
    WorldFluxMessage* data;
    if (d->getType().compare("RawBufferData") == 0) {
		RawBufferData *rbd = (RawBufferData *)d;
		data = new WorldFluxMessage();
		data->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
	} else {
		data = (WorldFluxMessage*)d;
	}
	UniverseDatabase* universeDB = (UniverseDatabase*)ag->getDatabase()->getDataItem("univ");
	universeDB->clearFluxConcMap();
	universeDB->fluxConcMap = data->getFluxConcMap();
};
