/*
 * Division.hh
 *
 *  Created on:
 *      Author:
 */

#ifndef FBA_HH
#define FBA_HH

#include "../../hive.hh"

namespace BatchReactor
{
	//COMMENTS
	class FBA : public Simulator {
	public:
		FBA(Agent *a);
		~FBA();
		void step(double t);
		void prepare();
		void synchroniseWithDatabase();
		void setAgent(Agent *ag);
		//TODO: should be in core
		Data* getDataFromAgent(int dataIndex);
		//TODO: should be in core
		void setLinksToAgentDB(vector<string> dataNamesInput);
	protected:
		void initialise();
	private:
		//TODO: should be in core
		vector<string> dataNames;
		//TODO: should be in core
		vector<int> dataIndecies;
		static int adjBoundary = 0;
		static int boundaryLower  = 1;
		static int boundaryUpper  = 2;
		static int adjReactions = 3;
		static int reactionLower  = 4;
		static int reactionUpper  = 5;
		static int reactionFlux = 6;
		static int boundaryFlux  = 7;
		static int objective  = 8;
	};
}

#endif /* FBA_HH */
