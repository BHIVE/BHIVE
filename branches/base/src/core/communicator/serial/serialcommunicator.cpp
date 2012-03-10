/**
 * @file serialcommunicator.cpp
 *
 *implemts serialcommunicator.hh
 *
 * @date Oct 14, 2009
 * @author garrit jentsch
 */

#include "serialcommunicator.hh"

using namespace Hive;

SerialCommunicator::SerialCommunicator() {}

SerialCommunicator::~SerialCommunicator() {}

void SerialCommunicator::receiveMessages() {}

void SerialCommunicator::sendMessage(Message* m, int dest) {
	// new way that uses registrar
	Registrar::getSystemRegistrar()->getAgent(dest)->placeMessageOnMessageQueue(m);

	// original way that has internal connection map
	//connection_map[dest]->placeMessageOnMessageQueue(m);
}

void SerialCommunicator::addAgent(Agent *a) {

	// original way that has internal connection map
	//connection_map[a->getAgentId()] = a;
}

void SerialCommunicator::setupCommunicationMap(map<int, Agent* >& m) {

	//original way that saves internal connection map, now does nothing
	//connection_map = m;
}

void SerialCommunicator::propogateAgent(int agentId, double time)
{
	// new way that uses registrar
	Registrar::getSystemRegistrar()->getAgent(agentId)->propagate(time);

	//original way that required internal connection map
	//connection_map[agentId]->propagate(time);
}

