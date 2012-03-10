/*
 * Division.hh
 *
 *  Created on:
 *      Author:
 */

#ifndef MASSBALANCE_HH
#define MASSBALANCE_HH

#include "../../hive.hh"

namespace BatchReactor
{
	//COMMENTS
	class MassBalance : public Simulator {
	public:
		MassBalance(Agent *a);
		~MassBalance();
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
		//TODO
		static int boundaryIndecies = 0;
		static int boundaryArray  = 1;
		static int substrateConcentrationArray  = 2;
		static int cellVolume  = 3;
	};
}

#endif /* MASSBALANCE_HH */
