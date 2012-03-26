/*
 * @date June 5, 2010
 * @author Christopher Henry
 * DESCRIPTION: This file contains the database, generator, and action associated with the message 
 * sent by the world to the universe to in order to update the world's neighbors about the state of
 * the voxels located on the boundary of the world
 */

#include "worldCellTransferMessageClasses.h"

using namespace std;
using namespace Hive;
using namespace SimpleCell;

template<class T>
ostream& SimpleCell::vector_os(ostream& stream, vector<T>& vd) {
    stream << vd.size();
    for (unsigned int i = 0; i < vd.size(); ++i) {
        stream << " " << vd[i];
    }
    return stream;
}
  
template<class T>
void SimpleCell::vector_is(istream& stream, vector<T>& vd) {
    int size;
    stream >> size;
    vd.resize(size);
    for (int i = 0; i < size; ++i) {
        stream >> vd[i];
    }
}

template<class T>
ostream& SimpleCell::pvector_os(ostream& stream, vector<T*>& vd) {
    stream << vd.size() << endl;
    for (unsigned int i = 0; i < vd.size(); ++i) {
        stream  << *(vd[i]) << " ";
    }
    return stream;
}
  
template<class T>
void SimpleCell::pvector_is(istream& stream, vector<T*>& vd) {
    int size;
    stream >> size;
    vd.resize(size);
    for (int i = 0; i < size; ++i) {
        vd[i] = new T();
        stream >> *(vd[i]);
    }
}
  
ostream& SimpleCell::operator<<(ostream& stream, CellWorldData cellWorldData) {
    stream << cellWorldData.id        << endl;
    stream << cellWorldData.diameter  << endl;
    stream << cellWorldData.dDiameter << endl;

    vector_os<double>(stream, cellWorldData.coordinates)    << endl;
    vector_os<double>(stream, cellWorldData.speed)          << endl;
    vector_os<double>(stream, cellWorldData.excretionRates) << endl;

    return stream;
}

istream& SimpleCell::operator>>(istream& stream, CellWorldData& cellWorldData) {
    stream >> cellWorldData.id;
    stream >> cellWorldData.diameter;
    stream >> cellWorldData.dDiameter;

    vector_is<double>(stream, cellWorldData.coordinates);
    vector_is<double>(stream, cellWorldData.speed);
    vector_is<double>(stream, cellWorldData.excretionRates);

    return stream;
}

/*
 * WorldUniverseData
 * packages cells in limbo
 */
WorldUniverseData::~WorldUniverseData() {
    for (unsigned int i = 0; i < worldCellData.size(); ++i) {
        if (worldCellData[i]) delete worldCellData[i];
    }
    worldCellData.clear();
    for (unsigned int i = 0; i < singleCellData.size(); ++i) {
        if (singleCellData[i]) delete singleCellData[i];
    }
    singleCellData.clear();
}

void WorldUniverseData::clear_db_vectors() {
	worldCellData.clear();
	singleCellData.clear();
}

void WorldUniverseData::addCellInLimbo(CellWorldData* worldCell, BacteriaDatabase* singleCell) {
    worldCellData.push_back(worldCell);
    singleCellData.push_back(singleCell);
}

void WorldUniverseData::printContent(ostream& out) {
    out << "printContent" << endl;
    char* buffer;
    int size;
    packDataForMPI(buffer, size);
    out << "worldCellData = {\n" << buffer << "}" << endl;
    delete [] buffer;
}

void WorldUniverseData::packDataForMPI(char*& buffer, int& size) {
    stringstream ss;
	ss << int(worldCellData.size()) << endl;
	for (int i=0; i < int(worldCellData.size()); i++) {
		char* buff;
		int bsize;
		CellWorldData_packDataForMPI(worldCellData[i],buff,bsize);
		ss << bsize << endl;
		ss << buff;
		delete [] buff;
	}
	for (int i=0; i < int(singleCellData.size()); i++) {
		char* buff;
		int bsize;
		singleCellData[i]->packDataForMPI(buff,bsize);
		ss << bsize << endl;
		ss << buff;
		delete [] buff;
	}
	buffer = strdup(ss.str().c_str());
    size = strlen(buffer);
}

void WorldUniverseData::unpackDataForMPI(char* buffer, int size) {
	stringstream ss(buffer);
    int numCells = 0;
	ss >> numCells;
	for (int i=0; i < numCells; i++) {
		int bsize = 0;
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
		CellWorldData* d = CellWorldData_unpackDataForMPI(buff,bsize);
		worldCellData.push_back(d);
		delete [] buff;
	}
	for (int i=0; i < numCells; i++) {
		int bsize = 0;
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
		BacteriaDatabase* d = new BacteriaDatabase();
		d->unpackDataForMPI(buff,bsize);
		singleCellData.push_back(d);
		delete [] buff;
	}
}

void GenerateWorldMessageToUniverse::placeMessage(int destID, int node) {
	Message *msg = new Message();
	msg->setAction(0);
	msg->setDestination(destID,node);
	msg->setSourceID(source->getAgentId());
	CartesianGrid* worldDB = (CartesianGrid*)source->getDatabase()->getDataItem(GRID);
    vector<CellWorldData*> cells = worldDB->getEdgeCells();
    if (cells.size() > 0) {
        // cout << "[" << mpirank() << "] # edgeCells = " << cells.size() << endl;
        for (unsigned int i = 0; i < cells.size(); ++i) {
            fprintf(stderr, "[%d] cell %d exiting\n", mpirank(), cells[i]->id);
            fprintf(stderr, "cell [%d] coordinates size = %d\n", i, cells[i]->coordinates.size());
            
            // cout << "cell " << i << " = {\n" << *(cells[i]) << "}" << endl;
        }
        PropagatorClass* propagator = get_propagator();
        WorldUniverseData* worldData = new WorldUniverseData();
        for (unsigned int i = 0; i < cells.size(); ++i) {
            Agent* bact = propagator->getAgent(cells[i]->id);
            // BacteriaDatabase* dbi = (BacteriaDatabase*)((bact->getDatabase())->getDataItem(BACTERIADATABASE));
            // cout << "db " << i << " = {\n" << *(dbi) << "}" << endl;
            
            BacteriaDatabase* db = (BacteriaDatabase*)(bact->getDatabase())->getDataItem(BACTERIADATABASE);
            worldData->addCellInLimbo(cells[i], db);
        }
        // worldData->setCellsInLimbo(cells);
        char* temp = NULL;
        int size;
        worldData->packDataForMPI(temp, size);
        // cerr << " packed buffer = { " << endl << temp << "}" << endl;

        msg->setAction(0);
        msg->setDestination(destID, node);
        msg->setSourceID(source->getAgentId());
        RawBufferData* rbd = new RawBufferData("WorldUniverseData", temp, size);
        msg->setArgument(rbd);
        //msg->setArgument(worldData);
        free(temp);//match strdup in packDataForMPI
        source->placeMessageInOutbox(msg);
        for (unsigned int i = 0; i < cells.size(); ++i) {
            int id = cells[i]->id; // record before removal
            source->removeChild(id);
            worldDB->removeCell(id);
            propagator->removeAgent(id);
        }
    }
}

void SyncWorldToUniverse::fire (Agent* ag, Data *d, int source) {
    // gets report from world about cells exiting the world
    WorldUniverseData* data;
    // cout << "d->getType() = " << d->getType() << endl;
    if (d->getType().compare("RawBufferData") == 0) {
        // cout << "raw" << endl;
		RawBufferData *rbd = (RawBufferData *)d;
		data = new WorldUniverseData();
        // fprintf(stderr, "rbd = {%s}\n", rbd->getBuffer());
		data->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
        // data->printContent(cout);
	} else {
		data = (WorldUniverseData*)d;
	}

    UniverseDatabase* univDB = (UniverseDatabase*)ag->getDatabase()->getDataItem("univ");
    univDB->addCellsInLimbo(data);
    //data->clear_db_vectors();
	// univDB->messagesToRelay.push_back(message);
}

void GenerateUniverseMessageToWorld::placeMessage(int destID, int node) {
    /*
    if (mpisize() == 1) {
        // there might be multiple worlds associated with the universe
        UniverseDatabase* univDB = (UniverseDatabase*)source->getDatabase()->getDataItem("univ");

        if (univDB->worldCellsMap.find(destID) != univDB->worldCellsMap.end()) {
            vector<CellWorldData*> cells;
            for (unsigned int i = 0; i < univDB->worldCellsMap[destID]->size(); ++i) {
                cells.push_back(&(*(univDB->worldCellsMap[destID]))[i]);
                univDB->getCellDestinationNode(cells[i]);
            }

            WorldUniverseData* worldData = new WorldUniverseData();
            worldData->setCellsInLimbo(cells);

            char* temp = NULL;
            int size;
            worldData->packDataForMPI(temp, size);

            delete univDB->worldCellsMap[destID];
            univDB->worldCellsMap.erase(destID);

            Message *msg = new Message();

            msg->setAction(1);
            msg->setDestination(destID, node);
            msg->setSourceID(source->getAgentId());

            msg->setArgument(new RawBufferData("WorldUniverseData", temp, size));
        
            source->placeMessageInOutbox(msg);
        }
        delete univDB->worldCellsMap[destID];
        univDB->worldCellsMap.erase(destID);

    } else
    */
    // remote message
    // assuming there is only one world on each node 
    // invoked once for universe's only child world
    UniverseDatabase* univDB = (UniverseDatabase*)source->getDatabase()->getDataItem("univ");
    map<int, vector<CellWorldData*> >::iterator it;
    for (it = univDB->worldCellsMap.begin(); it != univDB->worldCellsMap.end(); ++it) {
		if (it->second.size() > 0) {
			Message *msg = new Message();
			WorldUniverseData* worldData = new WorldUniverseData();
			int destNode = it->first;
			int destID;
			for (unsigned int i = 0; i < it->second.size(); ++i) {
				CellWorldData* cell  = it->second[i];
				if (i==0) {
					destID = univDB->getCellDestinationId(cell);
				}
				BacteriaDatabase* db = univDB->singleCellsMap[it->first][i];
				worldData->addCellInLimbo(cell, db);
			}
			msg->setArgument(worldData);
			source->placeMessageInOutbox(msg);
			msg->setAction(1);
			msg->setDestination(destID, destNode);
			msg->setSourceID(source->getAgentId());
		}
    }
    univDB->worldCellsMap.clear();
    univDB->singleCellsMap.clear();
    // data pointed by worldData will be cleaned by message sender
}

/*
 * Universe Actions
 *
 */
void SyncUniverseToWorld::fire (Agent* ag, Data *d, int source) {
    // check for the cells that just entered this world
    // WorldMessageToUniverse* message;
    WorldUniverseData* data;
    // cout << "d->getType() = " << d->getType() << endl;
    // cout << mpirank() << " SyncUniverseToWorld::fire()" << endl;
    
    if (d->getType().compare("RawBufferData") == 0) {
        // cout << "RAW" << endl;
		RawBufferData *rbd = (RawBufferData *)d;
		data = new WorldUniverseData();
		data->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
        // data->printContent(cout);
	} else {
		data = (WorldUniverseData*)d;
	}
    
    // by reference
    vector<CellWorldData*>&    newCells = data->getWorldCellData();
    vector<BacteriaDatabase*>& newDbs   = data->getSingleCellData();
    if (newCells.size() > 0) {
        bactCellFactory cellFactory;
        CartesianGrid* grid = (SimpleCell::CartesianGrid*)ag->getDatabase()->getDataItem(GRID);
        for (unsigned int i = 0; i < newCells.size(); ++i) {
            CellWorldData* newCell = newCells[i];
            if (newCell->coordinates.size() == 0)  {
                cerr << "newCell->coordinates.size() == 0" << endl;
                exit(-1);
            }
            
            BacteriaDatabase* newDb = newDbs[i];
            // cout << "newCells[" << i << "] = {\n" << (*newCell) << "}" << endl;
            // cout << "newDbs[" << i << "] = {\n" << (*newDb) << "}" << endl;
            Agent* cell = cellFactory.createAgent(newDb);
            ag->addChild(cell->getAgentId());
            cell->setParent(ag->getAgentId());
            newCell->id = cell->getAgentId();
            grid->addCell(newCell);
            newDb->setCurrentConcentrations(grid->getCellConcentrations(cell->getAgentId()));
            ag->getPropagator()->addAgent(cell);
            fprintf(stderr, "[%d] cell %d entering\n", mpirank(), cell->getAgentId());
        }
		data->clear_db_vectors();
    }
}
