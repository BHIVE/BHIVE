/*
 * WorldFactory.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: Johnyboy
 */

#include "WorldFactory.hh"

using namespace Hive;
using namespace SmallLonelyWorld;

WorldFactory::WorldFactory() : AgentFactory()
	{
	cout << "Created a World Factory here: " <<this << endl;
	}

Agent* WorldFactory::createAgent()
	{
	Agent *World = new Agent();
	cout << "World Factory created a new World Agent here: " << World << endl;
	return World;
	}
