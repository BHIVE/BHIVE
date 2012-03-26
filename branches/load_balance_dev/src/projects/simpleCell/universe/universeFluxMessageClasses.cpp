/*
 * @date June 5, 2010
 * @author Christopher Henry
 * DESCRIPTION: This file contains the database, generator, and action associated with the message 
 * sent by the universe to the worlds to update the concentrations in the boundary layer of voxels
 */

#include "universeFluxMessageClasses.h"

using namespace std;
using namespace Hive;
using namespace SimpleCell;

void GenerateUniverseFluxMessageToWorld::placeMessage(int destID, int node) {
	UniverseDatabase* univDB = (UniverseDatabase*)source->getDatabase()->getDataItem("univ");
	if (univDB->fluxConcMap[node].size() > 0) {
		WorldFluxMessage* uniFluxMsg = new WorldFluxMessage();
		for (unsigned int i=0; i < univDB->fluxConcMap[node].size(); i++) {
			uniFluxMsg->addFluxConcData(univDB->fluxConcMap[node][i],node);
		}
		Message *msg = new Message();
		msg->setAction(2);
		msg->setDestination(destID, node);
		msg->setSourceID(source->getAgentId());
		msg->setArgument(uniFluxMsg);
		source->placeMessageInOutbox(msg);
	}
}

void RecvUniverseFluxMessage::fire (Agent* ag, Data *d, int source) {
    //Translating the data into a world flux message
	WorldFluxMessage* data;
	if (d->getType().compare("RawBufferData") == 0) {
		RawBufferData *rbd = (RawBufferData *)d;
		data = new WorldFluxMessage();
		data->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
	} else {
		data = (WorldFluxMessage*)d;
	}
	//Getting the cartesian grid
	CartesianGrid* grid = (CartesianGrid*) ag->getDatabase()->getDataItem(GRID);
	map<int,vector<FluxConcData*> > mp =  data->getFluxConcMap();
	vector<FluxConcData*> fluxConcData = mp.begin()->second;
	for (int i=0; i < int(fluxConcData.size()); i++) {
		FluxConcData* current = fluxConcData[i];
		Voxel* vDest = grid->getVoxelByID(current->destID,ag->getAgentNode());
		Voxel* vSource = grid->getVoxelByID(current->sourceID,current->sourceCPU,true);
		if (vSource->bv != NULL) {
			for (int j=0; j < int(current->fluxToDest.size()); j++) {
				//Adjusting the concentration in the border voxel by the original flux
				vDest->compoundConcentrations[j] += vSource->bv->netFlux[j]/grid->getVoxelVolume();
				//Adjusting the concentration in the external voxel by the original external flux
				current->sourceConc[j] += current->fluxToDest[j]/grid->getVoxelVolume();
				//Calculating the average flux
				vSource->bv->netFlux[j] = (vSource->bv->netFlux[j]-current->fluxToDest[j])/2;
				//Adjusing the border and external voxel concentrations by the averaged flux
				vDest->compoundConcentrations[j] += -vSource->bv->netFlux[j]/grid->getVoxelVolume();
				current->sourceConc[j] += vSource->bv->netFlux[j]/grid->getVoxelVolume();
				//Saving the original concentration prior to overwriting
				vSource->bv->interpolatedFlux[j] = vSource->bv->originalConcentration[j];
				//Overwriting the original concentration by the concentration sent from the other processor
				vSource->bv->interpolatedFlux[j] = grid->getVoxelVolume()*(current->sourceConc[j] - vSource->bv->originalConcentration[j])-vSource->bv->netFlux[j];
				vSource->bv->originalConcentration[j] = current->sourceConc[j];
			}
		}
	}
}
