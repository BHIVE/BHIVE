/*
 * @file cranknicholson.hh
 *
 *  @author garrit
 *
 *  @date December 10th, 2009
 *   */

#ifndef CRANKNICOLSON_HH_
#define CRANKNICOLSON_HH_

#include "../../util/sparselib++/compcol_double.h"
#include "../../util/sparselib++/comprow_double.h"
#include "../../util/sparselib++/coord_double.h"
#include "../../util/sparselib++/ilupre_double.h"
#include "../../util/sparselib++/gmres.h"
#include "../../util/sparselib++/mvm.h"
#include "../../util/sparselib++/mvv.h"
#include "../../util/sparselib++/vecdefs.h"

#include "../simulator.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/grid/grid.hh"
#include "../../agent/data/grid/nvoxel.hh"
#include "../../agent/data/grid/nvoxel.hh"

using namespace Hive;
using namespace std;

/**
 * @brief simple straightforward implementation of the crank-nicholson scheme for integrating pdes making use of
 * the sparselib++ library
 *
 * @author Garrit
 * @date December 10th, 2009 last edited: 12-10-09
 *
 */
class CrankNicolson : public Simulator {
public:

	CrankNicolson();

	~CrankNicolson();

	void step(double t);

	/// seems redundant to me
	void prepare();

	/// why doesn't the base
	void setAgent(Agent *ag);

	void setNameOfDataStructureToOperateOn(string name) { this->gridname = name; };

	void setTimeStep(double);

	/// we will use this method to update the right handside of the system
	void synchroniseWithDatabase();

	/// testing routine
	void testing();

	/// method for generating debugging output
	void generateOutputForValidation();

protected:
	/// here we will set up the lefthandside of the system
	void initialise();

private:
	/// timestep
	double dt;
	/// grid of the world agent.
	Grid *g;

	/// grid name in database
	string gridname;

	// lhs of the system
	CompRow_Mat_double *lhs;
	// rhs of the problem
	VECTOR_double *rhs;
	// preconditioner
	CompRow_ILUPreconditioner_double *precond;
};

#endif /* CRANKNICHOLSON_HH_ */
