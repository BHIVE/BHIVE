/*
 * ourSimulator.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: Len
 */

#include "ourSimulator.hh"

using namespace std;
using namespace Hive;

OurSimulator::OurSimulator(){
	cout << "OurSimulator constructor called.\n";
}

void OurSimulator::setAgent(Agent *a){
	this->agent = a;
}
