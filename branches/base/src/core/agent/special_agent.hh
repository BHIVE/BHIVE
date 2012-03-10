/**
 * @file special_agent.hh
 *
 *this file contains the declaration of the special agent class.
 *
 *@date Jan 19, 2010
 *@author Garrit Jentsch
 */

#ifndef SPECIAL_AGENT_HH_
#define SPECIAL_AGENT_HH_

#include "agent.hh"
#include "../exception/exception.hh"
#include "../simulators/simulator.hh"

#include <iostream>
#include <vector>
#include <queue>

using namespace std;

namespace Hive {

class Simulator;
class Agent;


class SpecialAgent : public Agent {
public:
	/// constructors
	SpecialAgent();
	//SpecialAgent(int agent_id);

	/// destructor
	virtual ~SpecialAgent();

	void setParent(int parent_id);

	void setOutputInterval(double d) {this->outputinterval = d; } ;


	void addChild(int child_id);

	void addSimulator(Simulator *s);

	int getParentId();

	Database* getMetaDatabase();

	int getNumOfChildren();

	int getChildAgentId(int childIndex);

	double getInternalTime() { return internalclock; };

	bool isChildAgent(int agentID);

	void propagate(double time);

	void updateInternalClock() { internalclock += outputinterval; };

	virtual void evaluateMessageQueue();

protected:
	// flag, now in agent class
	//bool is_special_agent;

private:
	Database *meta;

	double internalclock;
	double outputinterval;
};

}
#endif /* SPECIAL_AGENT_HH_ */
