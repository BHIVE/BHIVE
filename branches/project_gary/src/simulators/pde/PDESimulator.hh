/*
 * @file PDESimulator.hh
 *
 *	a PDE simulator
 *
 *	@author Michael Sneddon
 *
 *	@date October 15, 2009, last edited: October 15, 2009
 */

#ifndef PDESIMULATOR_HH_
#define PDESIMULATOR_HH_

#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../util/util.hh"
#include "../../agent/data/diff_data/diffusion_data.hh"


namespace Hive {

class PDESimulator : public Simulator {
public:
	/// Constructor
	PDESimulator();

	/// destructor
	~PDESimulator();

	/**
	 * integrate the system for a timestep
	 * @param t timestep
	 */
	void step(double simdt);

	virtual void stepVoxel(Voxel* currentVoxel, int index, double simdt);
	void swapVoxel(Voxel* currentVoxel);

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



private:
	double internalTime;
	double dt;
	DiffData *db; //Possible put in base simulator class?
};
}

#endif /* DUMMYSIMULATOR_HH_ */
