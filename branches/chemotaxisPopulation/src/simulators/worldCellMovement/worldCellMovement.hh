/**
 * @author Christopher Henry (chenry@mcs.anl.gov)
 * @modified 9/8/2010
 */

#ifndef WORLDMOVEMENTSIMULATOR_HH
#define WORLDMOVEMENTSIMULATOR_HH

#define GRID "Grid"

using namespace std;
using namespace Hive;

namespace worldMovementSimulator {
	class worldMovementSimulatorDB : public Data {
	  public:
		virtual int getNumberOfEntities();
		virtual vector<double> getEntityCoordinates(int);
		virtual void setEntityCoordinates(int,vector<double>);
		virtual vector<double> getEntityDiameter(int);
		virtual void setEntityDiameter(int,double);
		virtual vector<double> getEntityMovement(int);
		virtual void setEntityMovement(int,vector<double>);
		virtual vector<vector<double>> getOverlappingEntities(int);
	}

	class worldMovementSimulator : public Simulator {
	  public:
		worldMovementSimulator() {};
		~worldMovementSimulator() {};
		void step(Agent* inAgent,double t);
		static inline const double dt() { return 0.001; };
	  protected:
		void initialise(Agent* inAgent);
		//Data indecies
		int gridDataIndex;
	};
}

#endif /* WORLDMOVEMENTSIMULATOR_HH */
