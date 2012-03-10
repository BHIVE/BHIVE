/*
 * MichealisMenten.hh
 *
 *  Created on:
 *      Author:
 */

#ifndef MICHEALISMENTEN_HH
#define MICHEALISMENTEN_HH

#include "../../hive.hh"

namespace BatchReactor
{
	//COMMENTS
	class MichealisMenten : public Simulator {
	public:
		MichealisMenten(Agent *a);
		~MichealisMenten();
		void step(double t);
		void prepare();
		void synchroniseWithDatabase();
		void setAgent(Agent *ag);
		//TODO: should be in core
		Data* getDataFromAgent(int dataIndex);
		//TODO: should be in core
		void setLinksToAgentDB(vector<string> dataNamesInput);
		double michealisMentenEquation(double ks,double vmax,double substrate);
	protected:
		void initialise();
	private:
		//TODO: should be in core
		vector<string> dataNames;
		//TODO: should be in core
		vector<int> dataIndecies;
		static int ksArray = 0;
		static int vmaxArray = 1;
		static int substrateArray = 2;
		static int rateArray = 3;
	};
}

#endif /* MICHEALISMENTEN_HH */
