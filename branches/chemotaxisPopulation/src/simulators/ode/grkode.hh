/**
 * @file grkode.hh garrit's runge kutta simulator
 *
 * @date Dec 14, 2009
 * @author jentsch
 */

#ifndef GRKODE_HH_
#define GRKODE_HH_


#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/rate/grate.hh"


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

	// timestep
	double dt;

	// number of species
	int num_species;

	// number of reactions
	int num_reactions;

	// pointer to the rate vector
	TVectorData<GRate*> *rate_ptrs;

	// stoichiometric matrix
	TVectorData<TVectorData<int> > *stoch_matrix;

	// chemical species
	TVectorData<double> *conc;
};

#endif /* GRKODE_HH_ */
