/**
 * SC_simulators.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef CELLSIMULATORS_HH_
#define CELLSIMULATORS_HH_




#include "../../../simulators/simulator.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../util/util.hh"
#include "../cellparameterinitializer.hh"

using namespace Hive;

namespace ChemoPop
{
	class SimpleSignalingSimulator : public Simulator
	{
		public:
			SimpleSignalingSimulator();
			~SimpleSignalingSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:


			IntegerData *isChemotactic;

			// parameters of the noise and mean cheYp
			DoubleData *tau;
			DoubleData *stdDev;
			DoubleData *meanCheYp;

			// calculated parameters (from input parameters)
			double variance;
			double sqrtOfD;
			double sigM;// = 1/e1 * (alpha/((1-(alpha/meanYp))*meanYp*meanYp ))*sigmaY;


			//Input parameter that is set from the environment
			int ligIndex;
			TVectorData<double> *ligands;
			TVectorData<double> *accumulated_ligands;


			DoubleData *eqtime;
			DoubleData *celltime;



			// internal parameters for simulation
			double dt;
			double sqrtOfDt;
			double Mdot;
			double TarTerm;
			double TsrTerm;

			double internalTime;




			//activity, activation, and cluster parameters
			double m0;
			DoubleData *alpha;

			DoubleData *e1;
			DoubleData *e0;

			DoubleData *KoffTar;
			DoubleData *KoffTsr;
			DoubleData *KonTar;
			DoubleData *KonTsr;

			IntegerData *TarCount;
			IntegerData *TsrCount;




			// terms to keep track of the traces
			int tracesNeeded;
			TVectorData<double> *CheYp;
			TVectorData<double>  *methLevel;
			double *act;// = new double [tracesNeeded];

	};




	class MotorSimulator : public Simulator
	{
		public:
			MotorSimulator();
			~MotorSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			IntegerData *n_motors;

			TVectorData<int> *motorStates;

			TVectorData<double> *CheYp;

			DoubleData *Kd;
			DoubleData *g1;
			DoubleData *w;

			double dt;

			bool areMotorsCorrelated;


			double internalTime;
			DoubleData *eqtime;
			DoubleData *celltime;
	};



	class ConformationModelSimulator : public Simulator
	{
		public:
			ConformationModelSimulator();
			~ConformationModelSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			double internalTime;
			DoubleData *eqtime;
			DoubleData *celltime;

			//model parameters
			DoubleData *waitingTimeInSemi;
			IntegerData * minNumberOfMotorsInCCWneededToSwim;


			//motor state information
			IntegerData *n_motors;
			TVectorData<int> *currentMotorStates;
			TVectorData<int> *lastMotorState;

			//flagella state information
			TVectorData<int> *flagellaStates;
			TVectorData<int> *lastFlagellaStates;
			TVectorData<double> *intervalTime;
			TVectorData<double> *timeInSemi;


			//cell state information
			IntegerData *swimming_state;
			IntegerData *last_swimming_state;



	};


	class VotingModelSimulator : public Simulator
	{
		public:
			VotingModelSimulator();
			~VotingModelSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			double internalTime;

			//model parameters
			IntegerData * minNumberOfMotorsInCCWneededToSwim;


			//motor state information
			IntegerData *n_motors;
			TVectorData<int> *currentMotorStates;

			//cell state information
			IntegerData *swimming_state;
			IntegerData *last_swimming_state;
	};













}





#endif /* SC_SIMULATORS_HH_ */
