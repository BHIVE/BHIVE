/*
 * @date June 5, 2010
 * @author Christopher Henry
 * DESCRIPTION: This file contains the database, generator, and action associated with the message 
 * sent by the world to the universe to in order to update the world's neighbors about the state of
 * the voxels located on the boundary of the world
 */

#ifndef WORLDFLUXMESSAGECLASSES_H_
#define WORLDFLUXMESSAGECLASSES_H_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {
	//This data structure holds data about the concentration and flux between a voxel on the world border and the neighbor across the border
	struct FluxConcData;//Defined in "universe.hh"

	//This is a data class holding information about the fluxes and concentrations on the world's borders
	class WorldFluxMessage : public Data {
		public:
			WorldFluxMessage(string name = "WorldFluxMessageToUniverse", string type = "WorldFluxMessageToUniverse"):Data(name, type) {};
			~WorldFluxMessage();
			void printContent(ostream& out) {};
			void packDataForMPI(char*& buffer, int& size);
			void unpackDataForMPI(char* buffer, int size);
			map<int,vector<FluxConcData*> >& getFluxConcMap() {return fluxConcMap;};
			//The following function loads the database from the list of boundary voxels
			void addFluxConcData(FluxConcData* inData,int inCPU) {fluxConcMap[inCPU].push_back(inData);};
		protected:
			//Mapping CPU ID to the flux data being sent to neighboring worlds
			map<int,vector<FluxConcData*> > fluxConcMap;
	};

	//This class generates a message containing information about fluxes and concentrations on the world's borders
	class GenerateWorldFluxMessageToUniverse : public MessageGenerator {
		public:
			GenerateWorldFluxMessageToUniverse(Agent *ag, int inCommunicationRound = 1) :
				MessageGenerator(ag, true, inCommunicationRound) {};
			void placeMessage(int destID,int node=0);
	};

	//This action receives data about flux between worlds from a child world
	class RecvFluxMessageFromWorld : public Action {
		public:
			void fire (Agent* ag, Data *d, int source);
	};
}

#endif /*WORLDFLUXMESSAGECLASSES_H_*/