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

#include "agentparser.hh"
#include "../../agent/agent.hh"
#include "../../agent/data/mathfunction/mathfunctiondata.hh"
#include "../../agent/data/template/tdata.hh"
#include "../../agent/data/grid/quadtreeformicrofluidic.hh"
#include "../../projects/chemotaxis_population/cubegeometryconstructor.hh"
#include "../../projects/chemotaxis_population/cellpositioninitialiser.hh"
#include "../meshinputreader/gridreaderformicrofluidic.hh"

namespace Hive {

	class GWAParser : public AgentParser {
	public:
		GWAParser();
		~GWAParser();

	protected:
		// does the actual parsing
		void doIt();

		// parse environment
		void parseEnvironment();

		// parse math environments
		void parseMathEnvironment();

		// parse grid environments
		void parseGridEnvironment();

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
		Database *db;

		// filenames for initialising the system for yann.
		string name_geometry_input_file_fmf;
		string name_nutrient_input_file_fmf;
		string name_cellposi_input_file_fmf;
	};
}


#endif /* GWAPARSER_HH_ */
