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
#include <sstream>
#include <string>
#include <vector>

#include "agentinitializer.hh"
#include "../../agent/agent.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../agent/data/template/tdata.hh"

using namespace Hive;
using namespace std;

/*
 * 	The input has to obey the following format.
 * 	In front of the data there has to be a flag which either has a value of
 * 	OLD_MODEL or WILLS_MODEL indicating whether the cells use the meancheyp, tau, sigma
 *  model or will's methylation model.
 *
 *  for the old model a parameter line has to look as follows:
 *  CheYp, tau, noise, numOfMotors, isCoordinated, flagellaModel, minFlagellaToRun, semiWaitingTime, numOfCells
 *
 *  for the new model a parameters have to look as follows:
 *  kr, Kr, kb, Kb, noise_parameter, meth_init, numOfMotors, isCoordinated, flagellaModel, minFlagellaToRun, semiWaitingTime, numOfCells
 *
 */



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
			double tauAdapt;
			double tauNoise;
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

			//// NEW PARAMETERS FOR WILL'S NEW SIGNALLING MODEL
			double kr;
			double Kr;
			double kb;
			double Kb;
			double noise_parameter;
			double meth_init;
			double cheyp_mean;

			/// NEW PARAMETERS: for relating cheyp to activity
			double a0;
			double y0;

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

				/// destructor
				~CellParameterInitializer();

				// return the number of cells for which values for initialisation exist
				int get_n_cells() const { return this->getNumberCells(); }

				void parseFile(string filename) { cerr << "# CellParameterInitializer::parseFile not implemented" << endl; };

				// Given the agent, sets up the next cell's database
				void setNextCellParameters(Agent *cell);

				bool areCellsBlindAgents() const { return isBlindAgent; };
				double getEqTime() const;
				double getOutputInterval() const;

				/// NEW Methods needed due to update of signalling model. should acutally be private.
				void parseFileOldModel(ifstream& input);
				void parseFileNewModel(ifstream& input);
				void setNextCellParametersOldModel(Agent *cell);
				void setNextCellParametersNewModel(Agent *cell);

			private:
				BoolData is_wills_methylation_model;

				bool isBlindAgent;

				vector <CellParameterData *> cells;
				vector <CellParameterData *> uniqueCPDobjects;


		};


}


#endif /* CELLPARAMETERINITIALISER_HH_ */
