/*
 * dummySimulator.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: msneddon
 */


#include "dummySimulator.hh"

using namespace Hive;

/// Constructor
DummySimulator::DummySimulator(Agent *a) : Simulator()
{
	this->agent=a;
	this->db=agent->getDatabase();
	this->xPos=0;
	this->yPos=0;

	DoubleData *ddX=new DoubleData("xPos",xPos);
	db->addData("xPos",ddX);

	DoubleData *ddY=new DoubleData("yPos",yPos);
	db->addData("yPos",ddY);
}

/// destructor
DummySimulator::~DummySimulator()
{

}

void DummySimulator::setAgent(Agent *ag) {
	this->agent=ag;
}


void DummySimulator::step(double t)
{
	double dt=0.1; double mv=0.5;
	for(;this->internalTime<t;this->internalTime+=dt)
	{
		this->xPos+=Util::RANDOM_CLOSED()*mv;
		this->yPos+=Util::RANDOM_CLOSED()*mv;
	}

	//Update the database
	DoubleData *ddX = dynamic_cast<DoubleData *>(db->getDataItem("xPos"));
	ddX->setDouble(this->xPos);

	DoubleData *ddY = dynamic_cast<DoubleData *>(db->getDataItem("yPos"));
	ddY->setDouble(this->yPos);
}

void DummySimulator::prepare()
{
	cout<<"dummy simulator of agent: "<<this->agent->getAgentId()<<" is preparing."<<endl;
}

void DummySimulator::synchroniseWithDatabase()
{
	DoubleData *ddX = dynamic_cast<DoubleData *>(db->getDataItem("xPos"));
	this->xPos = ddX->getDouble();

	DoubleData *ddY = dynamic_cast<DoubleData *>(db->getDataItem("yPos"));
	this->yPos = ddY->getDouble();

	cout<<"dummy simulator of agent: "<<this->agent->getAgentId()<<" is synching with database."<<endl;
}



void DummySimulator::initialise()
{

}
