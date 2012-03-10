/*
 * agentinitializer.hh
 *
 *  Created on: Jun 25, 2010
 *      Author: jentsch
 */

#ifndef AGENTINITIALIZER_HH_
#define AGENTINITIALIZER_HH_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../../agent/agent.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"

#include "../../exception/exception.hh"
#include "../../util/util.hh"

namespace Hive {

	/// abstract class parameter data
	class ParameterData {
	public:
		ParameterData(){};
		virtual ~ParameterData() {};
	protected:

	};

	/// abstract class for initialising agents.
	class AgentInitializer  {
	public:
		AgentInitializer(){};
		AgentInitializer(string filename);
		virtual ~AgentInitializer(){ };
		void setNumberOfCells(int i) { this->number_cells = i; };
		int getNumberCells() const { return number_cells; };
		virtual void parseFile(string filename)=0;
		virtual void setNextCellParameters(Agent *cell)=0;
	protected:
		int number_cells;
		vector<ParameterData* > cells;
		int currentCell;
	};

}



#endif /* AGENTINITIALIZER_HH_ */
