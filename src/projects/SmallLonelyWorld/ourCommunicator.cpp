/*
 * simpleCommunicator.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: Len
 */

#include "ourCommunicator.hh"

using namespace std;
using namespace Hive;

OurCommunicator::OurCommunicator(){
	cout << "OurCommunicator constructor called.\n";
}

void OurCommunicator::receiveMessages(){}

void OurCommunicator::sendMessage(Message *m, int dest){}

void OurCommunicator::propogateAgent(int agentId, double time){}

void OurCommunicator::setAgent(Agent *a){
	this->agent = a;
}
