#include "worldMovementSimulator.hh"

void worldMovementSimulator::step(Agent* inAgent,double t) {
	worldMovementSimulatorDB* db = (worldMovementSimulatorDB*)inAgent->getDatabase()->getDataItem(GridDataIndex);
	//Calculating the number of timesteps required such that the internal time step of the simulator is not exceeded
	int numSteps = int(t/dt()+1);
	//Calculating the internal timestep size that ensures the internal time step is not exceeded
	double dt = double(t/numSteps);
	//Running the timesteps
	for (int i=0; i < numSteps; i++) {
		//Calling the child simulator
		if (child != NULL) {
			child->step(inAgent,dt);
		}
		//Iterating over all entities in the system moving them one time step
		for (int j=0; j < g->getNumberOfEntities(); j++) {
			//Getting cell location and direction
			vector<double> coord = db->getEntityCoordinates(j);
			vector<double> dir = db->getEntityMovement(j)
			vector<double> diam = db->getEntityDiameter(j);
			if (diam.size() > 1) {
				diam[0] += diam[1];
				db->setEntityDiameter(j,diam[0]);
			}
			//Getting list of overlapping entities
			vector<vector<double>> overlap = db->getOverlappingEntities(j);
			//Recalculating movement vector "dir" base on repulsion from overlapping entities
			for (int k=0; k < int(overlap.size()); k++) {
				double magnitude = 0;
				for (int m=0; m < int(coord.size()); m++) {
					magnitude += (coord[m]-overlap[k][m])*(coord[m]-overlap[k][m]);
				}
				magnitude = sqrt(magnitude);
				magnitude = 1/(0.1+magnitude/(diam[0]+overlap[k][4]));
				for (int m=0; m < int(coord.size()); m++) {
					dir[k] += magnitude*(coord[m]-overlap[k][m]);
				}
			}
			//Adjusting coordinates based on direction
			for (int k=0; k < int(coord.size()); k++) {
				coord[k] += dt*dir[k];
			}
		}
	}
}

void worldMovementSimulator::initialise(Agent* agentTemplate) {
	gridDataIndex = agentTemplate->getDatabase()->getDataIndex(GRID);
};
