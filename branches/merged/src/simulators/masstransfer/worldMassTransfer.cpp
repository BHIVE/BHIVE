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
			//Getting list of overlapping entities

			//Recalculating movement vector "dir" base on repulsion from overlapping entities
			
			for (int k=0; k < int(coord.size()); k++) {
				coord[k] += dt*dir[k];
			}
			//Resetting the cell location vector
			setEntityCoordinates(j,coord);
		}
		
			
			
			
			
			void setEntityCoordinates(int,vector<double>);
			vector<double> getEntityDiameter(int);
			void setEntityDiameter(int,double);
			vector<double> getEntityMovement(int);
			void setEntityMovement(int,vector<double>);
			bool coordinateInBounds(vector<double>);
			vector<vector<double>> getOverlappingEntities(int);



			CellWorldData* currentCell = g->getCellByIndex(j);
			// First check that the cell lies within the world. If it does not, it is not simulated
			if (g->cellIsInBounds(currentCell)) {
				Voxel* oldVoxel = g->getVoxel(currentCell->coordinates);
				// Adjusting grid concentraions based on cell excretions
				for (int k=0; k < g->numberOfCompounds(); k++) {
					double amount = oldVoxel->compoundConcentrations[k]*g->getVoxelVolume();
					amount += currentCell->excretionRates[k]*dt;
					oldVoxel->compoundConcentrations[k] = amount / ( 
						g->getBlockSize() * g->getBlockSize() * g->getBlockSize());
				}
				// Changing coordinates and diameters
				for (int i=0; i < 3; i++) {
					currentCell->coordinates[i] += dt * currentCell->speed[i];
				}
				// Check if this move causes the cell to run into a wall or another cell
				bool collision = false;
				Voxel* newVoxel = g->getVoxel(currentCell->coordinates);
				if(newVoxel->wall || g->checkCellCollision(currentCell)) {
					// if it does, move the cell back to it's old location
					// FIXME: this is not physically accurate
					for (int i=0; i < 3; i++) {
						currentCell->coordinates[i] -= dt * currentCell->speed[i];
					}
					collision = true;
				}
				currentCell->diameter += currentCell->dDiameter;
				// Check if the cell is moving into a different voxel
				bool different = false;
				for (int i=0; i < 3; i++) {
					if (newVoxel->coordinates[i] != oldVoxel->coordinates[i]) {
						different = true;
					}
				}
				if (different && ! collision) {
					// Removing the cell from its previous voxel
					for (list<CellWorldData*>::iterator IT = oldVoxel->cells.begin(); 
						IT != oldVoxel->cells.end(); IT++) {
						if ((*IT) == currentCell) {
							oldVoxel->cells.erase(IT);
							break;
						}
					}
					// Adding cell to its new voxel
					newVoxel->cells.push_back(currentCell);
					// Adding new voxel to history
					CellHistory* newHistory = new CellHistory;
					newHistory->voxelCoord = newVoxel->coordinates;
					newHistory->enterTime = get_propagator()->getCurrentTime();
					currentCell->history.push_back(newHistory);
				}
			}
		}
		if (subSim != NULL) {
			subSim->step(inAgent,dt);
		}
	}
	g->printCellLocations();
}

void CellMovement::initialise(Agent* agentTemplate) {
	


};
