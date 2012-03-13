#include "mixer.hh"

using namespace Hive;
using namespace Transwell;

Mixer::Mixer() {
  this->agent = NULL;
  this->g     = NULL;
  this->time  = NULL;
}

Mixer::~Mixer() {
}

void Mixer::setAgent(Agent *ag) {
  this->agent = ag;
  this->g     = (Grid *) agent->getDatabase()->getDataItem("world_agent_grid");
  this->time  = (DoubleData*) agent->getDatabase()->getDataItem("time");
  this->initialise();
}

void Mixer::setTimeStep(double dt) {
  this->dt = dt;
}

void Mixer::prepare() {}

void Mixer::initialise() {
  this->next_mixing_time = this->time->getDouble() + dt;
}

void Mixer::synchroniseWithDatabase() {}

void Mixer::testing() {}

void Mixer::step(double t) {
  // only do something if it is the time to do so.
  if (next_mixing_time < this->time->getDouble()) {
    double total_conc = 0;
    // iterate over the voxels of the grid and check what the total il2
    // concentration is
    for (int i=0; i<g->getNumberOfVoxels(); i++)
      total_conc += g->getVoxel(i)->getConcentration(0);
    // do the mixing, averaging that is
    total_conc /= g->getNumberOfVoxels();
    // set the concentration to the average
    for (int i=0; i<g->getNumberOfVoxels(); i++) {
      g->getVoxel(i)->setConcentration(0,total_conc);
      // remember to update the old concentration as well. 
      g->getVoxel(i)->copyConcentration();
    }
    // update the time at which the nex
    next_mixing_time += dt;
  }
}
