/*
 * blindagentinitializer.hh
 *
 *  Created on: Jun 25, 2010
 *      Author: jentsch
 */

#ifndef BLINDAGENTINITIALIZER_HH_
#define BLINDAGENTINITIALIZER_HH_

#include <string>
#include <sstream>

#include "agentinitializer.hh"

using namespace std;
using namespace Hive;

namespace ChemoPop {
	///	class for holding parameter data for blind agents
	class BlindAgentParameterData : public ParameterData {
	public:
		// the blind agent parameter constructor sets up a default parameter
		BlindAgentParameterData();
		~BlindAgentParameterData();

		// rescaled velocity
		double          velocity;
		// typename
		string          type;
		// rescaled parameter for movement cost
		double			cost_for_movement;
		// death threshold
		double          death_thres;
		// birth threshold
		double          birth_thres;
		// yield
		double			yield;
		// rescaled kcat
		double          kcat;
		// rescaled Km
		double          Km;
		// initial essence
		double          initial_essence;
		// spontaneous wacking
		double			spontaneous_death_rate;
		// double rho for angle distribution
		double 			rho;
		// flag for which simulator should be used
		bool 			is_levy;
		// double parameter for distribution
		double          parameter_for_distribution;

		double          effeciency;

		double          r1;
	};

	/// class for initialising the parameters of the blindagent
	/// reads data from a file. each line needs to have the following information
	/// note that there are NO commas separating the different parameters only space
	/// typename velocity cost_for_movement kcat Km yield death_thres initial_essence spontaneous_death_rate
	///  rho is_levy parameter_for_movement_distribution number_of_cells
	class BlindAgentParameterInitializer : public AgentInitializer {
	public:
		BlindAgentParameterInitializer();
		BlindAgentParameterInitializer(string filename);
		~BlindAgentParameterInitializer();
		virtual void parseFile(string filename);
		virtual void setNextCellParameters(Agent *cell);
	protected:
		vector <BlindAgentParameterData *> uniqueBAPDs;  // needed for proper destruction
	};

}
#endif /* BLINDAGENTINITIALIZER_HH_ */
