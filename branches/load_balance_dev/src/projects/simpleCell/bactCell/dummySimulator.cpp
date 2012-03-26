#include "bactCell.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;

const int Dummy_Simulator::X = 0;
const int Dummy_Simulator::Y = 1;
const int Dummy_Simulator::Z = 2;


Dummy_Simulator::Dummy_Simulator()
{
	double DefaultGammaAlpha = 4.;
	double DefaultGammaBeta = 1./18.32045567939674;
	double DefaultGammaOffset = -4.606176605609249;

}
Dummy_Simulator::~Dummy_Simulator()
{
}

void Dummy_Simulator::step(Agent* inAgent,double t)
{

	BacteriaDatabase* db = (BacteriaDatabase*)inAgent->getDatabase()->getDataItem(BACTERIADATABASE);

	vector<double> direction = db->getDirection();
	double speed = db->getSpeed();

    if (mpisize() > 1 && Util::RANDOM(1.0) < 0.8) {

        
		direction[0] = mpicoord(0) + 0.1 - Util::RANDOM(0.2);
		direction[1] = mpicoord(1) + 0.1 - Util::RANDOM(0.2);
		direction[2] = mpicoord(2) + 0.1 - Util::RANDOM(0.2);
    } else {
		direction[0] = 1.0 - Util::RANDOM(2.0);
		direction[1] = 1.0 - Util::RANDOM(2.0);
		direction[2] = 1.0 - Util::RANDOM(2.0);
    }
    
    double fraction  = 1.0 / sqrt((direction[0]*direction[0]) + (direction[1]*direction[1]) + (direction[2]*direction[2])) * speed;
    direction[0] = direction[0] * fraction ;
    direction[1] = direction[1] * fraction ;
    direction[2] = direction[2] * fraction ;
    
    
    db->setDirection(direction);

}


void Dummy_Simulator::initialise() {}









