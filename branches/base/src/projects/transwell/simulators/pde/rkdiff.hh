/*
 *
 *
 */

#ifndef RKDIFF_HH_
#define RKDIFF_HH_

#include <map>
#include <fstream>
#include <iostream>
#include <vector>

#include "../../../../core/simulators/simulator.hh"
#include "../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../data/grid/grid.hh"
#include "../../data/grid/nvoxel.hh"
#include "../../data/grid/nvoxel.hh"

#include "../../core/planegeometryconstructor.hh"

using namespace Hive;
using namespace Transwell;
using namespace std;

class RKDiff : public Simulator {
public:

  RKDiff();
  
  ~RKDiff();

  void step(double t);
  
  void prepare();
  
  void setAgent(Agent *ag);
  
  void setTimeStep(double);

  void synchroniseWithDatabase();
  
  /// testing routine
  void testing();
  
  /// method for generating debugging output
  void generateOutputForValidation();
  
protected:
  /// here we will set up the lefthandside of the system
  void initialise();
  
private:
  /// time
  DoubleData* time;
  
  /// timestep
  double dt;
  
  /// grid of the world agent.
  Grid *g;

  int num_voxels;

  NVoxel *vptr;
  
  double local_conc;
  double diff_coeff;

  /// for the temporary results
  double *k1;
  double *k2;
  double *k3;
  double *k4;
  double *delta;
};

#endif /* RKDIFF_HH_ */
