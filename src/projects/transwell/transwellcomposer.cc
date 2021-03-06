/*
 * transwellcomposer.cc
 *
 *  Created on: Dec 10, 2009
 *      Author: jentsch
 */

#include "transwellcomposer.hh"

using namespace Hive;
using namespace Transwell;

TranswellComposer::TranswellComposer() {
	cerr << "# hello, i am the transwellcomposer" << endl;
	this->number_of_cells = 0;
}


TranswellComposer::~TranswellComposer() {
	delete post;
}

void TranswellComposer::setupNumberOfCells(int i) {
	this->number_of_cells = i;
}

void TranswellComposer::setupSimulation() {
	// set up agent factories
	this->initializeAgentFactories();

	// set number of cells in the worldagent factory
	((WorldAgentFactory *) factories.at(0))->setNumberCells(this->number_of_cells);
	// set number of cells in the outputagent factory
	((OutputAgentFactory *) factories.at(3))->setNumberCells(this->number_of_cells);

	// create the world
	maestro = this->createAgent(*factories[0]);
	orchestra.push_back(maestro);

	// create cells
	for (int i=0; i<this->number_of_cells; i++) {
		Agent *cell = factories.at(1)->createAgent();
		orchestra.push_back(cell);
	}

	// create nuclei
	for (int i=0; i<this->number_of_cells; i++) {
		Agent *nucl = factories.at(2)->createAgent();
		orchestra.push_back(nucl);
	}

	// create special output agent
	SpecialAgent *fox = (SpecialAgent* ) factories.at(3)->createAgent();
	special_agents.push_back(fox);

	// set up hierarchy
	this->setupAgentHierarchy();

	// add communicator to simulation
	this->addSerialCommunicator();

	// send initialisation messages if possible!
	for (unsigned int i=0; i<this->orchestra.size(); i++) {
		this->orchestra[i]->init();
	}
}


void TranswellComposer::initializeAgentFactories() {
	/// World = transwell
	// create the world-agent-factory
	factories.push_back(new WorldAgentFactory());
	// prepare the worldagentfactory for producing cells
	factories.at(factories.size()-1)->finalise();

	// Tcells
	// creat tcellagentfactory
	factories.push_back(new TCellAgentFactory());
	// prepare the tcellagentfactory for producing cells
	factories.at(factories.size()-1)->finalise();

	// nucleus
	// create nucleusagentfactory
	factories.push_back(new NucleusAgentFactory());
	// prepare the nucleusagentfactory for producing nuclei
	factories.at(factories.size()-1)->finalise();

	// special agent
	// creat outputagentfactory
	factories.push_back(new OutputAgentFactory());
	factories.at(factories.size()-1)->finalise();
}


void TranswellComposer::setupAgentHierarchy() {
	// set children of the maestro
	for (int i=1; i<= this->number_of_cells; i++) {
		Agent *cell = orchestra.at(i);
		Agent *nucl = orchestra.at(i+this->number_of_cells);

		// add relation between maestro and cell agents
		maestro->addChild(cell->getAgentId());
		cell->setParent(maestro->getAgentId());

		// add relation between cell and nucleus agents
		cell->addChild(nucl->getAgentId());
		nucl->setParent(cell->getAgentId());

		// add relation between cell and output agent
		cell->addSpecialAgent(this->special_agents[0]->getAgentId());
	}
}


Agent* TranswellComposer::createAgent(AgentFactory& af) {
	return af.createAgent();
}


void TranswellComposer::addSerialCommunicator() {
	post = new SerialCommunicator();
	// add serial-communicator to the orchestra
	for (unsigned int i=0; i<this->orchestra.size(); i++) {
		post->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(post);
	}
	// connect the special agents with the communicator
	for (unsigned int i=0; i<this->special_agents.size(); i++) {
		post->addAgent(special_agents[i]);
		special_agents[i]->addCommunicator(post);
	}
}
