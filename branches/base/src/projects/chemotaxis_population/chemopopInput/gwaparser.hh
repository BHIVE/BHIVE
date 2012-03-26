/*
 * gwaparser.hh
 *
 * garrit's world agent parser: as with the rest of the simulation environment, this is by
 * no means a finished product and is merely meant to facilitate the life of nick and yann
 * to initialise the environment of their simulations.
 *
 * we need to provide a couple of examples of the input files.
 *
 *
 *  Created on: Sep 28, 2010
 *      Author: jentsch
 */

#ifndef GWAPARSER_HH_
#define GWAPARSER_HH_

#include "../../../core/agent/agent.hh"
#include "../../../core/agent/data/mathfunction/mathfunctiondata.hh"
#include "../../../core/agent/data/template/tdata.hh"
#include "../../../core/agent/data/boundary/boundarydata.hh"
#include "../../../core/input/systemParser/agentparser.hh"

#include "../chemopopData/grid/quadtreeformicrofluidic.hh"
#include "../cubegeometryconstructor.hh"
#include "../chemopopInitialiser/cellpositioninitialiser.hh"
#include "meshinputreader/gridreaderformicrofluidic.hh"
#include "../chemopopData/historyCollector.hh"

namespace Hive {

	class GWAParser : public AgentParser {
	public:
		GWAParser();
		~GWAParser();

		void setTumbleTriggeredAverageParameters( bool generate_traj_before_tumble_output,
					double generateTrajBeforeTumbleTimeOFFSET,
					double generateTrajBeforeTumbleTimeSTART,
					double generateTrajBeforeTumbleTimeEND);

		// return the equilibration time
		double getEquilibrationTime() { return equilibration_time; };

	protected:
		// does the actual parsing
		void doIt();

		// parse environment
		void parseEnvironment();

		// parse math environments
		void parseMathEnvironment();

		// parse grid environments
		void parseGridEnvironment();

		// parse boundary behaviors
		void parseBoundaryBehavior();

		// parse simulated environments
		void parseEnvironmentSimulator();

		// parse information about the initial cell positions
		// this also adds general data structures to the world agent that
		// are needed to simulate movement of cells.
		void parseCellPositions();

		// method for initialising cell positions from a file when
		// doing a microfluidic simulation
//		void readCellPositionsForMicroFluidic(string);

		// for parsing the time steps
		void parseTimeSteps();

		// for parsing the flags of the system
		void parseFlags();

		// ------------ methods called by parseFlags ------------------------
		// for adding datastructures to world agent such that cell agents can model metabolism.
		void addSimpleMetabolismStructures();

		// setting the cell type (either ecoli or blindagent) and adding the relevant datastructures
		// to the world agent.
		void setCellType(string);

		// set whether the dimensions of the simulation are in 2D or 3D
		void setDimension(string);

		// set the rotational diffusion constant of the population
		void setRotDif(string);

		// set the speed of the population
		void setCellSpeed(string value);

		// set whether tumbles are randomly distributed or gamma distributed
		void setTumbleStyle(string value);


		Database *db;

		// filenames for initialising the system for yann.
		string name_geometry_input_file_fmf;
		string name_nutrient_input_file_fmf;
		string name_cellposi_input_file_fmf;


		// parameters for tumble triggered averages
		bool generate_traj_before_tumble_output;
		double generateTrajBeforeTumbleTimeOFFSET;
		double generateTrajBeforeTumbleTimeSTART;
		double generateTrajBeforeTumbleTimeEND;


		double equilibration_time;
	};
}


#endif /* GWAPARSER_HH_ */
