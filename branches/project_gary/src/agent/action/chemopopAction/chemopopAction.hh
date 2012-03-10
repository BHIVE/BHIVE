/*
 * chemopopAction.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef CHEMOPOPACTION_HH_
#define CHEMOPOPACTION_HH_

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "../arnie.hh"
#include "../../messageGenerator/chemopopMessageGenerator/chemopopMessageGenerator.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../data/template/tdata.hh"
#include "../../data/mapdata/mapintintdata.hh"
#include "../../data/mathfunction/mathfunctiondata.hh"
#include "../../../exception/exception.hh"
#include "../../special_agent.hh"
#include "../../../util/util.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	/// action which updates the swimming state of the cell inside the world
	class UpdateWorldSeenCellsSwimmingStateAction : public Action {
	public:
		UpdateWorldSeenCellsSwimmingStateAction();
		~UpdateWorldSeenCellsSwimmingStateAction();

		void fire(Data *d);
		void prepare();
	private:
		MapIntIntData *local_index_map;
		TVectorData<int> *swimstates;
		TVectorData<int> *oldstates;

	};

	/// action which updates the speed of cells inside the world
	class UpdateWorldSeenCellSpeedAction : public Action {
	public:
		UpdateWorldSeenCellSpeedAction();
		~UpdateWorldSeenCellSpeedAction();

		void fire(Data *d);
		void prepare();
	private:

		MapIntIntData *local_index_map;
		TVectorData<double> *cellspeeds;
	};

	/// action which updates the next turning angle inside the world
	class UpdateWorldSeenBlindAgentsAnglesAction : public Action {
	public:
		UpdateWorldSeenBlindAgentsAnglesAction();
		~UpdateWorldSeenBlindAgentsAnglesAction();

		void fire(Data *d);
		void prepare();
	private:
		MapIntIntData *local_index_map;
		TVectorData<double> *nextTurnAngles;
		TVectorData<double> *velocities;;
	};

	/// action by which the world is capable of changing the background concentration
	class UpdateWorldBackgroundConcentrationAction : public Action {
	public:
		UpdateWorldBackgroundConcentrationAction();
		~UpdateWorldBackgroundConcentrationAction();

		void fire(Data *d);
		void prepare();
	private:
		OneDLinearMathFunctionData *odlmfd;
	};

	/// action by which the world learns how much nutrient a cell wants to consume
	class UpdateWorldDesiredConsumptionAction : public Action {
	public:
		UpdateWorldDesiredConsumptionAction();
		~UpdateWorldDesiredConsumptionAction();

		void fire(Data *d);
		void prepare();
	private:

		TVectorData<double> *desired_cell_consumption;
		MapIntIntData *local_index_map;
	};


	/// action which updates the world when a cell dies
	class UpdateWorldBlindAgentDeathAction : public Action {
	public:
		UpdateWorldBlindAgentDeathAction();
		~UpdateWorldBlindAgentDeathAction();

		void fire(Data *d);
		void prepare();
	private:

		void swap(int index1,int index2);
		void popLastElement();

		MapIntIntData *local_index_map ;
		MapIntIntData *index_to_agent_id_map;
		IntegerData *numberofcells;
		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cellupvecs;
		TVectorData<double> *desired_cell_consumption;
		TVectorData<double> *cellspeeds;

		bool existsCellVoxelPositions;
		TVectorData<int> *cell_voxel_positions;
		TVectorData<int> *number_cells_per_voxel;

		// if (celltype == ECOLI) {
		bool existsSwimmingStates;
		TVectorData<int> *swimmingstates;
		TVectorData<int> *lastswimmingstates;

		// if (celltype == BLIND) {
		bool existsCellNextTurnAngle;
		TVectorData<double> *cellnextturnangle;
		TVectorData<int> *cell_wants_to_move;

	};


	/// action which updates the world when a cell dies
	class UpdateWorldBlindAgentBirthAction : public Action {
	public:
		UpdateWorldBlindAgentBirthAction();
		~UpdateWorldBlindAgentBirthAction();

		void fire(Data *d);
		void prepare();
	private:
		MapIntIntData *local_index_map ;
		MapIntIntData *index_to_agent_id_map;
		IntegerData *numberofcells;
		TVectorData<TVectorData<double>* > *cell_positions;
		TVectorData<TVectorData<double>* > *cell_dir_vecs;
		TVectorData<TVectorData<double>* > *cellupvecs;
		TVectorData<double> *desired_cell_consumption;
		TVectorData<double> *cellspeeds;

		bool existsCellVoxelPositions;
		TVectorData<int> *cell_voxel_positions;
		TVectorData<int> *number_cells_per_voxel;

		// if (celltype == ECOLI) {
		bool existsSwimmingStates;
		TVectorData<int> *swimmingstates;
		TVectorData<int> *lastswimmingstates;

		// if (celltype == BLIND) {
		bool existsCellNextTurnAngle;
		TVectorData<double> *cellnextturnangle;
		TVectorData<int> *cell_wants_to_move;
	};


	/// action which updates the ligand concentration that a cell sees
	class UpdateCellSeenLigandAction : public Action {
	public:
		UpdateCellSeenLigandAction();
		~UpdateCellSeenLigandAction();
		void prepare();

		void fire(Data *d);
	private:
		DoubleData *essence;
		DoubleData *kcat;
		DoubleData *Km;
		TVectorData<double> *appetite;
		TVectorData<double> *ligand_conc;
		DoubleData *yield;
		DoubleData *effeciency;
	};

	/// action which updates the cell internal accumulated ligand counter
	class UpdateCellAccumulatedLigandAction : public Action {
	public:
		UpdateCellAccumulatedLigandAction();
		~UpdateCellAccumulatedLigandAction();
		void prepare();

		void fire(Data *d);
	private:
		DoubleData *essence;
		DoubleData *yield;
		TVectorData<double> *accumulated_ligands;
		DoubleData *effeciency;
	};

	/// action by which the outputagent receives the positions of the cells
	class SpecialAgentReceiveCellPositionsAction : public Action {
	public:
		SpecialAgentReceiveCellPositionsAction();
		~SpecialAgentReceiveCellPositionsAction();

		void fire(Data*);
		void prepare();

		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		bool isbinary;
	};

	/// action with which the special agent accumulates data for generating output
	class SpecialAgentAccumulateCellDataAction : public Action {
	public:
		SpecialAgentAccumulateCellDataAction();
		~SpecialAgentAccumulateCellDataAction();

		void fire(Data *);
		void prepare();

		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		int cellcounter;
		bool isbinary;


		TVectorData<TVectorData<double>* > *cell_matrix;
		IntegerData *numberofcellsData;
	};

	/// action with which the special agent accumulates data about the blind agent for generating output
	class SpecialAgentAccumulateBlindAgentDataAction : public Action {
	public:
		SpecialAgentAccumulateBlindAgentDataAction();
		~SpecialAgentAccumulateBlindAgentDataAction();

		void fire(Data *);
		void prepare();

		void prepareOutputfile(string, bool);

	private:
		ofstream *out;
		int blindagentcounter;
		bool isbinary;
	};



	/**
	 * When cells are born or die, they tell the registrar with a
	 * message to be sent to the special agent.  Here, the special
	 * agent responds to the notification.
	 */
	class SpecialAgentUpdateBlindAgentCountAction : public Action {
	public:
		SpecialAgentUpdateBlindAgentCountAction();
		~SpecialAgentUpdateBlindAgentCountAction();

		void fire(Data *);
		void prepare();
		// why do we need that?!
		void prepareOutputfile(string, bool);
	};


	class SpecialAgentOutputGridAction : public Action {
	public:
		SpecialAgentOutputGridAction();
		~SpecialAgentOutputGridAction();

		void fire(Data *);
		void prepare();
		void prepareOutputfile(string, bool);
	private:
		ofstream *out;
		bool isbinary;
	};

}

#endif /* CHEMOPOPACTION_HH_ */
