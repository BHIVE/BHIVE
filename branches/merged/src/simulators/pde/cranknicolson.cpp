/*
 * cranknicolson.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: jentsch
 */

#include "cranknicolson.hh"
// #include "../../projects/transwell/transwellgeometryconstructor.hh"

using namespace Hive;


CrankNicolson::CrankNicolson() {
	agent   = NULL;
	lhs     = NULL;
	rhs     = NULL;
	precond = NULL;
}

CrankNicolson::~CrankNicolson() {
	delete lhs;
	lhs = NULL;
	delete rhs;
	rhs = NULL;
	delete precond;
	precond = NULL;
}

void CrankNicolson::setAgent(Agent *ag) {
	this->agent = ag;
	this->g     = (Grid *)agent->getDatabase()->getDataItem("world_agent_grid");
	this->initialise();
}

void CrankNicolson::step(double t) {
	// for the moment we just perform a single step dt and ignore the parameter given to us.
	// again, only interested in species 0
	int species = 0;
	this->synchroniseWithDatabase();
	int dim     = g->getNumberOfVoxels();
	VECTOR_double x(dim,0.0);
	MATRIX_double H(dim+1,dim,0.0);
	int maxit   = 100;   // how fast does that normally converge? is there any heuristic for choosing maxit?
	int restart = dim;   // what is restart
	double tol  = 1e-6;  // precision
	int result = GMRES(*lhs, x, *rhs, *precond, H, restart, maxit, tol);
	// error checking
	if (result != 0) {
		cerr << "SHIT: CrankNicolson::step did not converge. don't really know what to do so i just abort" << endl;
		exit(1);
	}
	// copy the result into the voxels
	for (int i=0; i<dim; i++) {
		g->getVoxel(i)->setConcentration(species, x[i]);
		g->getVoxel(i)->copyConcentration();
	}
}

void CrankNicolson::setTimeStep(double dt) {
	this->dt = dt;
}

void CrankNicolson::prepare() {
}

void CrankNicolson::initialise() {
	// right now the algorithm just operates on the species 0
	// WE WILL HAVE TO CHANGE THIS IN LATER VERSIONS
	double diff_coeff = g->getDiffusionCoeff(0) * this->dt;

	// number of voxels in the grid
	int num_voxels      = g->getNumberOfVoxels();
	int counter         = 0;
	// number of entries in the matrix
	int num_mat_entries = num_voxels + g->getTotalNumberOfNeighbors();
	// row counter for sparse matrix representation
	int row_counter     = 0;
	// values for sparse matrix representation
	double values[num_mat_entries];
	// column indices for sparse matrix representation
	int col_ind[num_mat_entries];
	// row pointer for sparse matrix representation
	int row_ptr[num_voxels+1];

	// pointer to voxel
	NVoxel *vptr;
	// diffusion rate
	double local_diffusion_coeff;
	// outward diffusion rate from voxel under consideration
	double outward;

	// iterate over the grid
	for (int i=0; i<num_voxels; i++) {
		// set the row_ptr
		row_ptr[i] = row_counter;
		// get the voxel
		vptr = g->getVoxel(i);
		// outward rate 1 - \sum rates
		outward = 1;
		// iterate over all the neighbors
		for (int j=0; j<vptr->getNumberNeighbors(); j++) {
			// calculate diffusion rate into neighbor from this voxel; the factor 2 is due to crank nicolson scheme(!)
			local_diffusion_coeff = vptr->getBaseDiffusionRate(j) * diff_coeff/2.;
			// set the value vector
			values[counter]       = -local_diffusion_coeff;
			// set in which column the value lives
			col_ind[counter++]  = vptr->getNeighborID(j);
			// add rate to outward current
			outward              += local_diffusion_coeff;
			row_counter += 1;
		}
		// set the diagonal entry
		col_ind[counter] = i;
		values[counter++] = outward;
		// increase row_counter by one
		row_counter          += 1;
	}

//	cerr << "FINALCOUNTER: " << counter << endl;
//	cerr << num_mat_entries << endl;
//	cerr << row_counter << endl;
//	cerr << num_voxels << endl;
//	exit(1);

	// the library requires that the last entry is the number of elements in the matrix
	row_ptr[num_voxels] = num_mat_entries;
	lhs = new CompRow_Mat_double(num_voxels, num_voxels, num_mat_entries, &values[0], &row_ptr[0], &col_ind[0]);

	// set up the LU-preconditioner.
	this->precond = new CompRow_ILUPreconditioner_double(*lhs);

	for (int i=0; i<num_voxels; i++)
		g->getVoxel(i)->copyConcentration();
}

// this will look a little like the method above
void CrankNicolson::synchroniseWithDatabase() {
	int species = 0;

	double diff_coeff = g->getDiffusionCoeff(species) * this->dt;
	int num_voxels    = g->getNumberOfVoxels();
	if (rhs != NULL)
		delete rhs;
	rhs               = new VECTOR_double(num_voxels, 0.0);
	double value;
	double tmp;
	double basic_rate;
	NVoxel *vptr;
	// iterate over the voxels
	for (int i=0; i<num_voxels; i++) {
		// get current voxel
		vptr  = g->getVoxel(i);
		// get concentration of the current voxel
		tmp = vptr->getConcentration(species);
		// remember this concentration
		value = tmp;
		// iterate over
		for (int j=0; j<vptr->getNumberNeighbors(); j++) {
			// calc diffusion rate
			basic_rate = vptr->getBaseDiffusionRate(j) * diff_coeff/2.;
			// subtract
			value -= tmp*basic_rate;
			// add
			value += g->getVoxel(vptr->getNeighborID(j))->getConcentration(species)*basic_rate;
		}
		// set rhs-value
		(*rhs)[i] = value;
	}
}

// here we set up a large cylinder. put some stuff into the middle and simulate for some time.
void CrankNicolson::testing() {
	cerr << "# entering the CrankNicolson::testing() routine" << endl;
	/*
	// set up a grid. use the transwellgeometryconstructor for this
	Transwell::TranswellGeometryConstructor builder;
	builder.setHeights(1,0.9);
	builder.setRadii(0.1, 1);
	builder.setResolution(0.05);
	this->g = builder.returnGrid("hans");

	// add chemical species to the grid
	for (int i=0; i<g->getNumberOfVoxels(); i++)
		g->getVoxel(i)->setNumberSpecies(1);
	g->addSpecies("IL-2",0.1);

	// set particle number in the middle of the grid
	int middle = g->insideVoxel(0.,0.,0.5);
	int distance = g->insideVoxel(0.,0.225, 0.5);
	g->getVoxel(middle)->setConcentration(0,10.0);

	// set the time_step
	this->setTimeStep(0.005);
	cerr << "# initialising" << endl;
	this->initialise();
	cerr << "# done initialising" << endl;
	ofstream out("crank_nicolson_test_output.txt");
	out << 0 << " " << g->getVoxel(middle)->getConcentration(0) << " " << g->getVoxel(distance)->getConcentration(0) << endl;
	for (int i=0; i<10/0.005; i++) {
		this->step(0.005);
		out << (i+1)*0.005 << " " << g->getVoxel(middle)->getConcentration(0) << " "
		<< g->getVoxel(distance)->getConcentration(0) << endl;
	}

	// remove the grid
	delete g;
	 */
	cerr << "# nothing done" << endl;
	cerr << "# leaving the CrankNicolson::testing() routine" << endl;
}

void CrankNicolson::generateOutputForValidation() {
	cerr << "# in CrankNicolson::generateOutputForValidation() " << endl;
	cerr << "# outputting left-hand-side matrix " << endl;
	cerr << "# (note that Fortran style indices are used for the output in Sparselib++)" << endl;
	cerr << *lhs << endl;

	cerr << endl << endl;
	cerr << "# outputting the right-hand side vector" << endl;
	this->synchroniseWithDatabase();
	cerr << *rhs << endl;

	// do some stepping and check the results.
	for (int i=0; i<100000; i++)
		this->step(0.1);

	cerr << *rhs << endl;
}
