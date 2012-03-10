/*
 * @file PDESimulator.hh
 *
 *	a PDE simulator
 *
 *	@author Michael Sneddon
 *
 *	@date October 15, 2009, last edited: October 15, 2009
 */

#ifndef FBASIMULATOR_HH_
#define FBASIMULATOR_HH_

#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../util/util.hh"
#include "../../agent/data/fba_data/fba_data.hh"

using namespace std;

namespace Hive {

//struct Element;
//class Chemotaxis::DiffData;

class FBASimulator : public Simulator {
public:
	/// Constructor
	FBASimulator();

	/// destructor
	~FBASimulator();

	/**
	 * integrate the system for a timestep
	 * @param t timestep
	 */
	void step(double simdt);

	/**
	 * prepare simulator for the first time, this will call
	 * the connectToDataBase and initialise methods
	 */
	void prepare();

	/**
	 * update simulator's internal variables before executing the integrate command
	 */
	void synchroniseWithDatabase();

	void setAgent(Agent* a);
protected:
	/**
	 *  initialise simulator's internal variables for the first time
	 */
	void initialise();

	double calculateRate(Reaction*);
private:
	double internalTime;
	double dt;
	FBAData *db; //Possible put in base simulator class?
};
}

#endif /* DUMMYSIMULATOR_HH_ */
