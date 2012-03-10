/**
 * @file transwellAction.hh
 *
 *this file contains the interface to the actions that can be called in the transwell system
 *
 *@date Jan 14, 2010
 *@author garrit jentsch
 */

#ifndef TRANSWELLACTION_HH_
#define TRANSWELLACTION_HH_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "../arnie.hh"
#include "../../messageGenerator/transwellMessageGenerator/transwellMessageGenerator.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../data/template/tdata.hh"
#include "../../data/grid/grid.hh"
#include "../../../exception/exception.hh"
#include "../../special_agent.hh"

using namespace std;
using namespace Hive;

namespace Transwell {
	// -------------------------------
	// first the actions of the world
	// -------------------------------
	/**
	 * @brief world action to initialise the position of the cells
	 * @author garrit jentsch
	 * @date Jan 14, 2010
	 */
	class InitWorldCellPositionAction : public Action {
	public:
		InitWorldCellPositionAction();
		~InitWorldCellPositionAction();

		/// execute action
		void fire(Data *d);
	private:
	};

	/**
	 * @brief world action to update the il2 concentration
	 * @author garrit jentsch
	 * @date Jan 14, 2010
	 */
	class UpdateWorldSeenIL2Action : public Action {
	public:
		UpdateWorldSeenIL2Action();
		~UpdateWorldSeenIL2Action();

		/// execute action
		void fire(Data *d);
	private:
	};

	//-----------------------
	// next the cell actions
	//------------------------
	/**
	 * @brief cell action to update the seen il2 concentration
	 * @author garrit jentsch
	 * @date Jan 14, 2010
	 */
	class UpdateCellSeenIL2Action : public Action {
	public:
		UpdateCellSeenIL2Action();
		~UpdateCellSeenIL2Action();

		/// execute action
		void fire(Data *d);
	private:
	};

	/**
	 * @brief cell action to update the particle numbers from the nucleus
	 * @author garrit jentsch
	 * @date Jan 14, 2010
	 */
	class UpdateCellSeenParticlesAction : public Action {
	public:
		UpdateCellSeenParticlesAction();
		~UpdateCellSeenParticlesAction();

		/// execute action
		void fire(Data *d);
	private:
	};

	//-----------------------------
	// finally the nucleus action
	//-----------------------------
	/**
	 * @brief nucleus action to update the number of pstat5 molecules
	 * @author garrit jentsch
	 * @date Jan 14, 2010
	 */
	class UpdateNucleusSeenPSTAT5Action : public Action {
	public:
		UpdateNucleusSeenPSTAT5Action();
		~UpdateNucleusSeenPSTAT5Action();

		/// execute action
		void fire(Data *d);
	private:
	};

	//---------------------------------------------------------------
	// there is always one more action... the special agent actions
	//---------------------------------------------------------------
	/**
	 * @brief accumulate action of the special agent
	 * @author garrit jentsch
	 * @date Jan 19, 2010
	 */
	class SpecialAgentAccumulateDataAction : public Action {
	public:
		SpecialAgentAccumulateDataAction();
		~SpecialAgentAccumulateDataAction();

		/// execute action
		void fire(Data *d);

		/// prepares filestream for writing
		void prepareOutputfile(string);
	private:
		ofstream *out;
	};

}

#endif /* TRANSWELLACTION_HH_ */
