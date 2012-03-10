/*
 * @date June 5, 2010
 * @author Christopher Henry,Scott Devoid,Fangfang Xia
 * DESCRIPTION: 
 */

#ifndef WORLDCELLTRANSFERMESSAGECLASSES_H_
#define WORLDCELLTRANSFERMESSAGECLASSES_H_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;

namespace SimpleCell {
	struct CellWorldData;
	class BacteriaDatabase;
	
	class WorldUniverseData : public Data {
	  public:
		WorldUniverseData(string name = "WorldUniverseData", string type = "WorldUniverseData"):Data(name, type) {}
		~WorldUniverseData();
		void printContent(ostream& out);
		void printDBObservables(OutputWriter *ow) {};
		void setDBObservables(OutputWriter *ow) {};
		void packDataForMPI(char*& buffer, int& size);
		void unpackDataForMPI(char* buffer, int size);
		// void setCellsInLimbo(vector<CellWorldData*>& cells);
		void clear_db_vectors();
		void addCellInLimbo(CellWorldData* worldCell, BacteriaDatabase* singleCell);
		vector<CellWorldData*>& getWorldCellData() { return worldCellData; }
		vector<BacteriaDatabase*>& getSingleCellData() { return singleCellData; }
	  protected:
		vector<CellWorldData*>    worldCellData;
		vector<BacteriaDatabase*> singleCellData;
	};
	
	class GenerateWorldMessageToUniverse : public MessageGenerator {
		public:
		GenerateWorldMessageToUniverse(Agent *ag, int inCommunicationRound = 1) :
			MessageGenerator(ag, true, inCommunicationRound) {}
		void placeMessage(int destID,int node=0);
	};

	class SyncWorldToUniverse : public Action {//chris,ff
		public:
			void fire (Agent* ag,Data *d,int source);
	};

	/*
	 * Universe Message Generator
	 *
	 */
	class GenerateUniverseMessageToWorld : public MessageGenerator {
		public:
		GenerateUniverseMessageToWorld(Agent *ag, int inCommunicationRound = 0) : 
			MessageGenerator(ag, false, inCommunicationRound) {}
		void placeMessage(int destID,int node);
	};

	//This action receives data about cells moving from one world to another
	//This will be universe action "0"
	class SyncUniverseToWorld : public Action {//ff
		public:
			void fire (Agent* ag, Data *d, int source);
	};
}

#endif /*WORLDCELLTRANSFERMESSAGECLASSES_H_*/