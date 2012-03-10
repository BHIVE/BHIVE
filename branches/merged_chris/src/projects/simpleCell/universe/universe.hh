/**
 * This file contains the universe agent class 
 * @date March 18, 2010
 * @author Christopher Henry,Fangfang Xia
 */

#ifndef UNIVERSE_HH_
#define UNIVERSE_HH_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {
	struct CellWorldData;
	class BacteriaDatabase;
	class WorldUniverseData;

	//This data structure holds data about the concentration and flux between a voxel on the world border and the neighbor across the border
	struct FluxConcData {
		int sourceID;
		int destID;
		int sourceCPU;
		vector<double> sourceConc;
		vector<double> fluxToDest;
	};

	void FluxConcData_packDataForMPI(FluxConcData* data, char*& buffer, int& size);
	FluxConcData* FluxConcData_unpackDataForMPI(char* buffer, int size);

	/*
	 * Universe Database
	 */
	class UniverseDatabase: public Data {
	  public:
		UniverseDatabase(string name="UniverseDatabase", string type="UniverseDatabase"):Data(name, type){
			neighborMap.resize(3);
			for (int i=0; i < 3; i++) {
				neighborMap[i].resize(3);
				for (int j=0; j < 3; j++) {
					neighborMap[i][j].resize(3);
					for (int k=0; k < 3; k++) {
						neighborMap[i][j][k].resize(4);
					}
				}
			}
		};
		~UniverseDatabase() { clearFluxConcMap(); };
		void printContent(ostream& out) {};
		void printDBObservables(OutputWriter *ow) {};
		void setDBObservables(OutputWriter *ow) {};
		void unpackDataForMPI(char* buffer,int size) {};
		int getCellDestinationId(CellWorldData* cell);
		int getCellDestinationNode(CellWorldData* cell);
		void addCellsInLimbo(WorldUniverseData* worldData);
		void clearCellsMap(bool deleteContents = false);
		void clearFluxConcMap();

		map<int, vector<CellWorldData*> > worldCellsMap;
		map<int, vector<BacteriaDatabase*> > singleCellsMap;
		map<int, vector<FluxConcData*> > fluxConcMap;

		vector<vector<vector<vector<int> > > > neighborMap;

		vector<int> neighbors;      // 27 neighboring worlds including itself
		vector<double> sizes;
		vector<double> worldSizes;
		vector<double> worldStartingPositions;
	  protected:  
	};

	/*
	 * Universe Simulator
	 *
	 */	
	class UniverseSimulator : public Simulator {
	  public:
		UniverseSimulator();
		~UniverseSimulator();
		void step(Agent *ag, double t);
	  protected:
		void initialise();
	  private:   
	};

	/*
	 * Universe Factory
	 *
	 */
	class UniverseFactory : public AgentFactory {
	  public:
		UniverseFactory();
		UniverseFactory(const int type);
		UniverseFactory(InputSystemReader *isr);
		~UniverseFactory();
		Agent *createAgent();
		Agent *createAgent(Action **ac);
		Agent *duplicateAgent(Agent *ag);
		void finalise();
	  protected:
		void addActionsToAgentsActionSet(Agent *ag);
		void addSimulatorToAgent(Agent *ag);
	  private:
		int simulatorType;
	};
}
#endif /* UNIVERSE_HH_ */
