/*
 * @file agent_factory.hh
 *
 *	this file contains the interface to the agent factory
 *
 *	@author Garrit Jentsch
 *
 *	@date 13th of October, 2009 last edited: 10-14-2009 by Garrit and Michael
 *
 *  Created on: Oct 13, 2009
 *      Author: jentsch
 */

#ifndef AGENT_FACTORY_HH_
#define AGENT_FACTORY_HH_

#include <string>

#include "../agent.hh"
#include "../../input/systemParser/inputsystemreader.hh"
#include "../../input/xmlParser/XmlParser.hh"
#include "../action.hh"
#include "../../simulators/simulator.hh"
#include "../../input/system/system.hh"

using namespace std;


namespace Hive
{

	/**
	 *  @brief agent factory
	 *
	 *  agent factory for constructing agents.
	 *
	 *  @todo clean up addActionToAgentsActionSet and addSimulatorToAgent
	 *  @todo included a bit finalised that is checked before creating agents.
	 *
	 *  @bug who know's?
	 *
	 *  @author Garrit Jentsch
	 *
	 *  @date 13th of October, 2009 last edited: 10-14-2009 by Garrit and Michael
	 *
	 */
    struct xmlParsedNode;

	class AgentFactory {
	public:
		/// default constructor
		AgentFactory() {};

		/**
		 * constructor
		 * @param *isr pointer to inputsystemreader
		 */
		AgentFactory(InputSystemReader *isr) {this->isr = isr;};

		/// destructor
		virtual ~AgentFactory() {};

		/**
		 * create agent of a certain type. encapsulates everything that has to be done, i.e.
		 * it adds actions, simulator, messagegenerators, and communicator to the agent. the simulator is also told
		 * to connect to the database and initialize itself.
		 * @return pointer to constructed agent
		 */
		virtual Agent* createAgent()=0;

		/**
		 * create an agent and provide actionset as an argument
		 * @param as pointer to actionset
		 */
		virtual Agent* createAgent(Action ** as)=0;
		virtual Agent* createAgent(xmlParsedNode* node){return 0;};
		virtual void addChild(Agent* parent, Agent* child, xmlParsedNode* config){};
    

		/**
		 * copies an agent
		 * @param *ag pointer to agent to be copied
		 * @return pointer to copy
		 */
		virtual Agent* duplicateAgent(Agent *ag)=0;

		/**
		 * method that needs to be called before agents can be created. this is
		 * where factory specific initialisations are done.
		 */
		virtual void finalise()=0;

		/**
		 * sets up inputsystemreader
		 * @param *isr inputsystemreader for this factory
		 */
		void setInputSystemReader(InputSystemReader *isr) { this->isr = isr; }

	protected:
		/**
		 * adds an action to the agents action set
		 * @param *ag pointer to agent
		 */
		virtual void addActionsToAgentsActionSet(Agent *ag)=0;

		/**
		 * adds a simulator to the agents simulator set
		 * @param *s pointer to simulator
		 */
		virtual void addSimulatorToAgent(Agent *ag)=0;

		/// system
		System *sys;

		/// input_reader
		InputSystemReader *isr;

	private:
	};
}

#endif /* AGENT_FACTORY_HH_ */
