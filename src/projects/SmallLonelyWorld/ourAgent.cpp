/*
 * ourAgent.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: Len
 */

#include "ourAgent.hh"

using namespace std;
using namespace Hive;

OurAgent::OurAgent(){
	cout << "OurAgent constructor called.\n";
}

OurAgent::OurAgent(OurSimulator *s){
	cout << "OurAgent constructor called.\n";
	this->simulator = s;
	this->simulator->setAgent(this);
}

OurAgent::OurAgent(OurSimulator *s, OurCommunicator *c){
	cout << "OurAgent constructor called.\n";
	this->simulator = s;
	this->simulator->setAgent(this);
	this->communicator = c;
	this->communicator->setAgent(this);
}

void OurAgent::addSimulator(OurSimulator *s){
	this->simulator = s;
}

void OurAgent::addCommunicator(OurCommunicator *c){
	this->communicator = c;
}
