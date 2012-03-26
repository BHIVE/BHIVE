/*
 * cranknicolson.cpp
 *
 *  Created on: Dec 10, 2009
 *      Author: jentsch
 */

#include "cranknicolson.hh"
#include "../../core/transwellgeometryconstructor.hh"
#include "../../core/planegeometryconstructor.hh"

using namespace Hive;
using namespace Transwell;

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
	this->time  = (DoubleData *)agent->getDatabase()->getDataItem("time");
	this->initialise();
}

void CrankNicolson::step(double t) {
	// <------------------------ if the simulator should NOT do anything ...
/*
	double initial_time = this->time->getDouble();

	this->time->setDouble(initial_time + dt);


	// for the moment we just perform a single step dt and ignore the parameter given to us.
	// again, only interested in species 0
	for (int i=0; i<g->getNumberOfVoxels(); i++)
		g->getVoxel(i)->copyConcentration();
*/
	// <----------------------- end

	// <------------------- if the simulator has to work!
	double initial_time = this->time->getDouble();
	//	for (int i=0; i<g->getNumberOfVoxels(); i++)
	//	g->getVoxel(i)->copyConcentration();

	int species = 0;
	this->synchroniseWithDatabase();
	int dim     = g->getNumberOfVoxels();
	int restart = 32;   // what is restart?

	VECTOR_double x(dim,0.0);
	MATRIX_double H(restart+1, restart, 0.0); 
	int maxit   = 150;   // how fast does that normally converge? is there any heuristic for choosing maxit?

	double tol  = 1e-6;  // precision
	int result = GMRES(*lhs, x, *rhs, *precond, H, restart, maxit, tol);
	// error checking
	if (result != 0) {
		cerr << "SHIT: CrankNicolson::step did not converge. don't really know what to do so i just abort" << endl;
		exit(1);
	}
	// copy the result into the voxels
	double delta;
	for (int i=0; i<dim; i++) {
	  delta = g->getVoxel(i)->getOldConcentration(species) - g->getVoxel(i)->getConcentration(species);
	  g->getVoxel(i)->setConcentration(species, x[i]-delta);
	  if (x[i] - delta < 0)
	    cerr << "shit" << " " << x[i] << " " << delta << " " <<  g->getVoxel(i)->getOldConcentration(species) <<  endl;
		g->getVoxel(i)->copyConcentration();
	}
	this->time->setDouble(initial_time + dt);
	// <------------------- end

}

void CrankNicolson::setTimeStep(double dt) {
	this->dt = dt;
}

void CrankNicolson::prepare() {
}

void CrankNicolson::initialise() {
	cerr << "# inside CrankNicolson::initialise" << endl;
	// right now the algorithm just operates on the species 0
	// WE WILL HAVE TO CHANGE THIS IN LATER VERSIONS !!!
	double diff_coeff = g->getDiffusionCoeff(0) * this->dt;
	// number of voxels in the grid
	int num_voxels      = g->getNumberOfVoxels();
	int counter         = 0;
	// number of entries in the matrix
	int num_mat_entries = num_voxels + g->getTotalNumberOfNeighbors();
	// row counter for sparse matrix representation
	int row_counter     = 0;
	// values for sparse matrix representation
	double *values = (double *) malloc(num_mat_entries * sizeof(double));
	// column indices for sparse matrix representation
	int *col_ind = (int *) malloc(num_mat_entries * sizeof(int));
	// row pointer for sparse matrix representation
	int *row_ptr = (int *) malloc((num_voxels+1)*sizeof(int));
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

	// the library requires that the last entry is the number of elements in the matrix
	row_ptr[num_voxels] = num_mat_entries;
	lhs = new CompRow_Mat_double(num_voxels, num_voxels, num_mat_entries, values, row_ptr, col_ind);

	// set up the LU-preconditioner.
	this->precond = new CompRow_ILUPreconditioner_double(*lhs);

	for (int i=0; i<num_voxels; i++)
		g->getVoxel(i)->copyConcentration();

	// get rid of the temporary variables!
	free(values);
	free(row_ptr);
	free(col_ind);

	cerr << "# done CrankNicolson::initialise" << endl;
}


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
		tmp = vptr->getOldConcentration(species);
		// remember this concentration
		value = tmp;
		// iterate over
		for (int j=0; j<vptr->getNumberNeighbors(); j++) {
			// calc diffusion rate
			basic_rate = vptr->getBaseDiffusionRate(j) * diff_coeff/2.;
			// subtract
			value -= tmp*basic_rate;
			// add
			value += g->getVoxel(vptr->getNeighborID(j))->getOldConcentration(species)*basic_rate;
		}
		// set rhs-value
		(*rhs)[i] = value;
	}
}




///////// testing methods to follow ... 


// here we set up a large cylinder. put some stuff into the middle and simulate for some time.
void CrankNicolson::testing() {

	cerr << "#entering the CrankNicolson::testing() routine" << endl;
	// the stepping method expects a time
	this->time = new DoubleData("time",0.0);

	// set up the geometry
	Transwell::TranswellGeometryConstructor builder;
	double max_height = 1;
	builder.setHeights(max_height,max_height);
	builder.setRadii(0.025,0.025);
	double reso = 1./80;
	builder.setResolution(reso);
	this->g = builder.returnGrid("test_grid");

	// add an input distribution
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		NVoxel *n = g->getVoxel(i);
		Coordinate c;
		n->setNumberSpecies(1);
		n->getCoordinate(0,c);
		double z = c.getZ();
		n->setConcentration(0,(z+reso/2.)/max_height);
	}
	g->addSpecies("IL2",1.0);

	// run the simulation
	this->setTimeStep(0.001);
	this->initialise();
	for (int i=0; i<100; i++)
		this->step(0.001);

	// get the result
	map<double,double> conc;
	for (int i=0; i<g->getNumberOfVoxels(); i++) {
		NVoxel *n = g->getVoxel(i);
		Coordinate c;
		n->getCoordinate(0,c);
		double z = c.getZ();
		double co = n->getConcentration(0);
		conc[(z+reso/2.)/max_height] += co;
	}

	// output the numerical solution
	ofstream out("crank_nicolson_test_output.dat");
	map<double,double>::iterator iter;
	for (iter = conc.begin(); iter!=conc.end(); iter++)
		out << iter->first << " " << iter->second/4. << endl;

	// calculate the analytical solution.
	map<double,double> ana_sol;
	for (iter = conc.begin(); iter!= conc.end(); iter++)
		ana_sol[iter->first] = 0.5;
	for (int i=1; i<=10000; i++)
		for (iter = conc.begin(); iter!= conc.end(); iter++)
			ana_sol[iter->first] -= 4./(M_PI*M_PI)*(1./(i*i))*exp(-i*i*M_PI*M_PI*0.1)*cos(i*M_PI*iter->first);

	// output the analytic solution
	out << endl << endl;
	for (iter = ana_sol.begin(); iter != ana_sol.end(); iter++)
		out << iter->first << " " << iter->second << endl;

	// calculate the mean relative error
	map<double,double>::iterator iter2;
	iter = ana_sol.begin();
	double error = 0;
	int numele = 0;
	for (iter2 =conc.begin(); iter2!=conc.end(); iter2++ ) {
		error += (iter2->second/4. - iter->second)*(iter2->second/4.-iter->second)/(iter->second*iter->second);
		iter++; numele += 1;
	}
	out << "# mean rel. error: " << error / numele << endl;
	out.close();

	delete g;

	cerr << "# leaving the CrankNicolson::testing() routine" << endl;
}


void CrankNicolson::testing2() {
  cerr << "~ running test2 of CrankNicolson" << endl;

  double x1, x2;
  Coordinate c;

  PlaneGeometryConstructor *pgc = new PlaneGeometryConstructor();
  pgc->setLength(1.0);
  pgc->setResolution(0.05);
  this->g = pgc->returnGrid("testgrid");
  g->addSpecies("hans",1.0);

  NVoxel *vptr;
  for (int i=0; i<g->getNumberOfVoxels(); i++) {
    vptr = g->getVoxel(i);
    vptr->getCoordinate(0,c);
    x1 = c.getX();
    vptr->getCoordinate(1,c);
    x2 = c.getX();
    vptr->setNumberSpecies(1);
    vptr->setConcentration(0,(x1+x2)/2.);
  }
  this->setTimeStep(0.00001);
  this->initialise();
  this->setTimeStep(0.00001);
  
  for (int i=0; i<4000; i++)
    this->step(0);

  vector<double> res((int)(1/0.05),0);
  int index =0 ;
  for (int i=0; i<g->getNumberOfVoxels(); i++)  {
    vptr = g->getVoxel(i);
    vptr->getCoordinate(0,c);
    index = (int)(c.getX()/0.05);
    res[index] += vptr->getConcentration(0);
  }

  // compare with analytical result 
  ofstream testres("testresCN.dat");
  for (unsigned int i=0; i<res.size(); i++)
    testres << i*0.05+0.05/2 << " " << res[i]/res.size() << endl;
  testres.close();
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
