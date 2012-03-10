/*
 * @date June 5, 2010
 * @author Christopher Henry
 * DESCRIPTION: This file contains the database, generator, and action associated with the message 
 * sent by the universe to the worlds to update the concentrations in the boundary layer of voxels
 */

#ifndef UNIVERSEFLUXMESSAGECLASSES_H_
#define UNIVERSEFLUXMESSAGECLASSES_H_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {
	//This data structure holds data about the concentration and flux between a voxel on the world border and the neighbor across the border
	struct FluxConcData;//Defined in "universe.hh"

	//This class generates the flux messages to a world telling it the conditions on its borders
	class GenerateUniverseFluxMessageToWorld : public MessageGenerator {
		public:
			GenerateUniverseFluxMessageToWorld(Agent *ag, int inCommunicationRound = 0) : 
				MessageGenerator(ag, false, inCommunicationRound) {};
			void placeMessage(int destID,int node);
	};

	//This class is the action used to integrate flux messages from the universe into the world
	class RecvUniverseFluxMessage : public Action {
		public:
			void fire (Agent* ag,Data *d,int source);
	};
}

#endif /*UNIVERSEFLUXMESSAGECLASSES_H_*/