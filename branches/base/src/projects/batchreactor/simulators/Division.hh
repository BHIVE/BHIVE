/*
 * Division.hh
 *
 *  Created on:
 *      Author:
 */

#ifndef DIVISION_HH
#define DIVISION_HH

#include "../../hive.hh"

namespace BatchReactor
{
	//COMMENTS
	class Division : public Simulator {
	public:
		Division(Agent *a);
		~Division();
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

		static int biomassIndex = 0;
		static int substrateConcentrationArray = 1;
		static int maxBiomass  = 2;
		static int division = 3;
		static int cellVolume = 4;
	};
}

#endif /* DIVISION_HH */
