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
	connection_map[dest]->placeMessageOnMessageQueue(m);
}

void SerialCommunicator::addAgent(Agent *a) {
	connection_map[a->getAgentId()] = a;
}

void SerialCommunicator::setupCommunicationMap(map<int, Agent* >& m) {
	connection_map = m;
}

void SerialCommunicator::propogateAgent(int agentId, double time)
{
	connection_map[agentId]->propagate(time);
}

