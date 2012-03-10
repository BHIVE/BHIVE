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

#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/exception/exception.hh"
#include "../../../core/util/util.hh"

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
