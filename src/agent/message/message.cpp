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

using namespace Hive;



Message::Message() {}

Message::~Message() {}

int Message::getActionId() {
	return action_to_be_taken;
}

int Message::getDestinationID() {
	return destinationID;
}

void Message::setAction(int id) {
	action_to_be_taken = id;
}

void Message::setDestinationID(int id) {
	this->destinationID = id;
}

void Message::setArgument(Data *d) {
	parameter.push_back(d);
}

void Message::getArgument(Data *&d, int i) {
	d = parameter[i];
}
