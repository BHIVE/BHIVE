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
#include <iostream>
#include <vector>
#include <queue>

using namespace std;

namespace Hive {

class SpecialAgent : public Agent {
public:
	/// constructors
	SpecialAgent();
	SpecialAgent(int agent_id);

	/// destructor
	~SpecialAgent();

	void setParent(int parent_id);

	void addChild(int child_id);

	void addSimulator(Simulator *s);

	int getParentId();

	Database* getMetaDatabase();

	int getNumOfChildren();

	int getChildAgentId(int childIndex);

	bool isChildAgent(int agentID);

	void outputCurrentState(OutputWriter *ow);

	void propagate(double time);

protected:
	// flag
	bool is_special_agent;

private:
	Database *meta;
};

}
#endif /* SPECIAL_AGENT_HH_ */
