/*
 *@file mixer.hh
 *
 * mixer of the transwell - for equilibrating the concentration every 
 * X hours / seconds
 */

#ifndef _MIXER_HH_
#define _MIXER_HH_

#include "../../../core/simulators/simulator.hh"
#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../data/grid/grid.hh"

using namespace std;
using namespace Hive;

namespace Transwell {

  class Mixer : public Simulator {
  public:
    Mixer();
    ~Mixer();

    void step(double t);
    
    // redundant but still in interface
    void prepare();

    void setAgent(Agent *ag);

    void setTimeStep(double);

    void synchroniseWithDatabase();

    void testing();

    void generateOutputForValidation();

  protected:
    void initialise();

  private:
    // grid of world agent
    Grid *g;
    // 
    double dt;
    //
    DoubleData *time;

    double next_mixing_time;
  };
}

#endif /* _MIXER_HH_ */
