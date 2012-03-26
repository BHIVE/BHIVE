/**
 * @file grkode.hh garrit's runge kutta simulator
 *
 * @date Dec 14, 2009
 * @author jentsch
 */

#ifndef GRKODE_HH_
#define GRKODE_HH_


#include "../../../../core/simulators/simulator.hh"
#include "../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../core/agent/data/template/tdata.hh"
#include "../../data/rate/grate.hh"

#include "../pde/cranknicolson.hh"

#include <stdlib.h>

using namespace Hive;
using namespace std;

/**
 * @brief quick and dirty implementation of the runge kutta algorithm for integrating simple ode systems
 *
 * @todo while this will work for a lot of systems we will need some more sophisticated methods for stepsize control and
 * for stiff system. for the stoichiometric matrix it may be worth thinking about using a more sophisticated data structure.
 * this matrix will again be sparse.
 *
 * @author garrit
 *
 * @date Dec 14th, 2009 last edited 12-14-2009 by garrit
 *
 */


class GRKOde : public Simulator {
public:
	GRKOde();

	~GRKOde();

	// parameter is ignored
	void singleStep(double t);

	void step(double t);

	void prepare();

	void setAgent(Agent *ag);

	/// set the internal timestep of the method
	void setTimeStep(double);

	/// we do not need to synchronise anything with the database
	void synchroniseWithDatabase();

	/// for testing
	void testing();

	/// generates some debugging output
	void generateOutputForValidation();

protected:
	/// we do not really have to initialise anything here
	void initialise();

	/// performs a single sub step of the runge kutta method
	void singleSubStep(vector<double>& input, vector<double>& output);

private:
	Agent *ag;

	/// clock/time of the agent
	DoubleData *time;

	DoubleData *propagate_until;

	/// timestep
	double dt;

	/// number of species
	int num_species;

	/// number of reactions
	int num_reactions;

	/// pointer to the rate vector
	TVectorData<GRate*> *rate_ptrs;

	/// stoichiometric matrix
	TVectorData<TVectorData<int> > *stoch_matrix;

	/// chemical species
	TVectorData<double> *conc;

	// some variables needed for the calculations
	int i, j;
	vector<double> output;
	vector<double> input;
	vector<double> rate;
	vector<double> conc_0;
	vector<double> conc_1;
	vector<double> conc_2;
	vector<double> conc_3;
	vector<double> conc_4;

	Coord_Mat_double *stoch;
	int num_entries ;
	vector<int> row_index;
	vector<int> col_index;
	vector<int> values;
};

#endif /* GRKODE_HH_ */
