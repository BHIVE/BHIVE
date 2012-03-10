/*
 * @file message.cpp
 *
 * implements message.hh
 *
 * @author garrit jentsch
 *
 * @date Oct 14, 2009
 *
 */

#include "message.hh"
#include "../../parallel/parallel.hh"


using namespace Hive;



Message::Message() {}

Message::~Message() {
	/*
	for (int i=0; i<this->parameter.size(); i++)
		if (parameter[i] != NULL)
			delete parameter[i];
			*/
}

int Message::getActionId() {
	return action_to_be_taken;
}

int Message::getDestinationID() {
	return destinationID;
}

int Message::getSourceID() {
	return sourceID;
}

int Message::getDestinationNode() {
	return this->destinationNode;
}


void Message::setAction(int id) {
	action_to_be_taken = id;
}

void Message::setDestination(int id, int node) {
	this->destinationID = id;
	this->destinationNode = node;
}

void Message::setDestination(int id) {
	this->destinationID = id;
	this->destinationNode = mpirank();
}

void Message::setSourceID(int id, int node) {
	this->sourceID = id;
	this->sourceNode = node;
}

void Message::setArgument(Data *d) {
	parameter.push_back(d);
}

void Message::getArgument(Data *&d, int i) {
	d = parameter[i];
}
