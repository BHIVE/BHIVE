/*
 *@file dummymessagegenerator.cpp
 *
 *implementation of the dummymessagegenerator
 *
 * @author garrit jentsch
 *
 * @date  Oct 15, 2009 last edited 10-15-2009
 *
 */

#include "dummymessagegenerator.hh"


using namespace Hive;
using namespace Dummy;

//DummyMessageGenerator::DummyMessageGenerator() {}

DummyMessageGenerator::DummyMessageGenerator(Agent *ag, bool isParentMessage) : MessageGenerator(ag, isParentMessage) {}

DummyMessageGenerator::~DummyMessageGenerator() {}

void DummyMessageGenerator::placeMessage(int destID) {
	Message *m = new Message();
	m->setAction(0);
	IntegerData *dat = new IntegerData("blup",5);
	m->setArgument(dat);
	source->placeMessageOnMessageQueue(m);
}
