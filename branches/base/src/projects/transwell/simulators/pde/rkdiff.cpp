#include "rkdiff.hh"

using namespace Hive;

RKDiff::RKDiff() {
  agent = NULL;
  g     = NULL;
  vptr  = NULL;

  num_voxels = 0;

}

RKDiff::~RKDiff() {
  free(k1);
  free(k2);
  free(k3);
  free(k4);
}

void RKDiff::setAgent(Agent *ag) {
  this->agent = ag;
}

void RKDiff::step(double t) { 
  int i,j;
  // calculate k1
  // iterate over the grid
  for (i=0; i<num_voxels; i++) {
    vptr  = g->getVoxel(i);
    k1[i] = 0; 
    local_conc = vptr->getOldConcentration(0);
    for (j=0; j<vptr->getNumberNeighbors(); j++) {
      k1[i] -= vptr->getBaseDiffusionRate(j)*diff_coeff*local_conc;
      k1[i] += vptr->getBaseDiffusionRate(j)*diff_coeff*
	g->getVoxel(vptr->getNeighborID(j))->getOldConcentration(0);
    }  
    k1[i] *= dt;
  }
  // calculate k2
  for (i=0; i<num_voxels; i++) {
    vptr  = g->getVoxel(i);
    k2[i] = 0;
    local_conc = vptr->getOldConcentration(0)+0.5*k1[i];
    for (j=0; j<vptr->getNumberNeighbors(); j++) {
      k2[i] -= vptr->getBaseDiffusionRate(j)*diff_coeff*local_conc;
      k2[i] += vptr->getBaseDiffusionRate(j)*diff_coeff*
	(g->getVoxel(vptr->getNeighborID(j))->getOldConcentration(0) + 
	 0.5*k1[vptr->getNeighborID(j)]);
    }
    k2[i] *= dt;
  }
  // calculate k3
  for (i=0; i<num_voxels; i++) {
    vptr  = g->getVoxel(i);
    k3[i] = 0;
    local_conc = vptr->getOldConcentration(0)+0.5*k2[i];
    for (j=0; j<vptr->getNumberNeighbors(); j++) {
      k3[i] -= vptr->getBaseDiffusionRate(j)*diff_coeff*local_conc;
      k3[i] += vptr->getBaseDiffusionRate(j)*diff_coeff*
	(g->getVoxel(vptr->getNeighborID(j))->getOldConcentration(0) + 
	 0.5*k2[vptr->getNeighborID(j)]);
    }
    k3[i] *= dt;
  }
  // calculate k4
  for (i=0; i<num_voxels; i++) {
    vptr  = g->getVoxel(i);
    k4[i] = 0;
    local_conc = vptr->getOldConcentration(0)+k3[i];
    for (j=0; j<vptr->getNumberNeighbors(); j++) {
      k4[i] -= vptr->getBaseDiffusionRate(j)*diff_coeff*local_conc;
      k4[i] += vptr->getBaseDiffusionRate(j)*diff_coeff*
	(g->getVoxel(vptr->getNeighborID(j))->getOldConcentration(0) + 
	 k3[vptr->getNeighborID(j)]);
    }
    k4[i] *= dt;
  }
  
  // construct the solution
  for (i=0; i<num_voxels; i++) {
    vptr = g->getVoxel(i);
    vptr->setConcentration(0, vptr->getConcentration(0)+(1./6)*
			   (k1[i]+2.*k2[i]+2.*k3[i]+k4[i]));
    if (vptr->getConcentration(0) < 0)
      vptr->setConcentration(0,0);
    vptr->copyConcentration();
  }

}

void RKDiff::setTimeStep(double dt) {
  this->dt = dt;
}

void RKDiff::prepare() {
}

void RKDiff::initialise() {
  // initialise the temporary stores
  this->num_voxels = g->getNumberOfVoxels();
  k1 = (double *) malloc(num_voxels*sizeof(double));
  k2 = (double *) malloc(num_voxels*sizeof(double));
  k3 = (double *) malloc(num_voxels*sizeof(double));
  k4 = (double *) malloc(num_voxels*sizeof(double));

  delta = (double *) malloc(num_voxels*sizeof(double));

  diff_coeff = g->getDiffusionCoeff(0);

}

void RKDiff::synchroniseWithDatabase() { 
}

void RKDiff::testing() { 
  cerr << "# running test of RKDiff Method" << endl;

  double x1, x2;
  Coordinate c;

  // constructing a plane
  // we are using the planegeometry constructor from the transwell project
  PlaneGeometryConstructor *pgc = new PlaneGeometryConstructor();
  pgc->setLength(1.0);
  pgc->setResolution(0.025);
  this->g = pgc->returnGrid("testgrid");
  g->addSpecies("hans", 1.0);

  // initialising concentration
  for (int i=0; i<g->getNumberOfVoxels(); i++) {
    vptr = g->getVoxel(i);
    //  vptr->printCenter();
    vptr->getCoordinate(0,c);
    x1   = c.getX();
    //    c.printContent(cerr);
    vptr->getCoordinate(1,c);
    x2   = c.getX();
    // c.printContent(cerr);
    
    vptr->setNumberSpecies(1);
    //    cerr << "setting conc to " << (x1+x2)/2 << endl;
    vptr->setConcentration(0,(x1+x2)/2.);
  }
 
  
  // initialising algorithm
  this->initialise();
  this->setTimeStep(0.00001);

  // time stepping for some time
  for (int i=0; i<4000; i++)
      this->step(0);
 
  // output result
  vector<double> res((int)(1./0.025),0);
  int index;
  for (int i=0; i<g->getNumberOfVoxels(); i++) {
    vptr = g->getVoxel(i);
    vptr->getCoordinate(0,c);
    index = (int)(c.getX()/0.025);
    res[index] += vptr->getConcentration(0);
  }

  // compare with analytical result 
  ofstream testres("testres.dat");
  for (unsigned int i=0; i<res.size(); i++)
    testres << i*0.025+0.025/2 << " " << res[i]/res.size() << endl;
  testres.close();
}

void RKDiff::generateOutputForValidation() {
}
