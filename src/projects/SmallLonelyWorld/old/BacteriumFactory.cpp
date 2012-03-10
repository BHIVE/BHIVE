/*
 * WorldFactory.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: Johnyboy
 */

#include "BacteriumFactory.hh"

using namespace Hive;
using namespace SmallLonelyWorld;

BacteriumFactory::BacteriumFactory() : AgentFactory()
	{
	cout << "Created a Bacterium Factory here: " <<this << endl;
	}

Agent* BacteriumFactory::createAgent()
	{
	Agent *Bact = new Agent();
	cout << "Bacterium Factory created a new Bacterium Agent here: " << Bact << endl;
	return Bact;
	}
