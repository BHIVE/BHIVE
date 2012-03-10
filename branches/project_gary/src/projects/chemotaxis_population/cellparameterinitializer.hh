/*
 * cellparameterinitialiser.hh
 *
 *  Created on: Apr 19, 2010
 *      Author: msneddon
 */

#ifndef CELLPARAMETERINITIALIZER_HH_
#define CELLPARAMETERINITIALIZER_HH_


#include <fstream>
#include <iostream>
#include <vector>

#include "agentinitializer.hh"
#include "../../agent/agent.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"

using namespace Hive;
using namespace std;


namespace ChemoPop {
	/// class for holding the parameter data of the cells
	class CellParameterData : public ParameterData {
		public:
			CellParameterData();
			~CellParameterData();

			//is the chemotaxis sensing system on or off.
			int isChemotactic;

			// Motion parameters
			double speed;
			double rotDiffConst;

			// Flagella Parameters
			int flagellaModelType;
			bool isFlagellaCoordinated;
			int n_motors;
			double motor_Kd;
			double motor_g1;
			double motor_w;

			int minNumberOfMotorsInCCWneededToSwim;
			double minTimeInSemi;

			// Signaling and Noise Parameters
			double tau;
			double sigma;
			double meanCheYp;

			// Receptor Cluster Parameters
			double alpha;

			double e0;
			double e1;

			double KoffTar; // This is 0.02 mM or 0.00002 M
			double KonTar; // mM
			double KoffTsr; // mM
			double KonTsr; //Yes, 1000 M, or 10^6 mM
			int TarCount;
			int TsrCount;

			// parameters for the entire population
			static double eqTime;
			static double outputInterval;

			// constant flags
			static const int FLAGELLA_VOTING_MODEL = 0;
			static const int FLAGELLA_CONFORMATION_MODEL = 1;
			static const int FLAGELLA_SOURJICK_MODEL = 2;

			// constants for the motor and multiple flagella models
			const static int CCW;
			const static int CW;

			const static int RUN;
			const static int TUMBLE;

			const static int NORMAL;
			const static int SEMI;
			const static int CURLY;
			const static int NONE;
	};

	/**
		 * @brief  a class to initialize cell parameters
		 *
		 * @author msneddon
		 *
		 * @date 4-18-10
		 */
		class CellParameterInitializer : public AgentInitializer {
			public:
				// initialize e.coli cells from the given input file
				CellParameterInitializer(string inputFileName);

				// initialize blind agent cells with just a count
				CellParameterInitializer(int n_blind_agents);


				~CellParameterInitializer();

				int get_n_cells() const { return this->getNumberCells(); }

				void parseFile(string filename) { cerr << "# CellParameterInitializer::parseFile not implemented" << endl; };

				// Given the agent, sets up the next cell's database
				void setNextCellParameters(Agent *cell);



				bool areCellsBlindAgents() const { return isBlindAgent; };
				double getEqTime() const;
				double getOutputInterval() const;


			private:
				bool isBlindAgent;

				vector <CellParameterData *> cells;


		};


}


#endif /* CELLPARAMETERINITIALISER_HH_ */
