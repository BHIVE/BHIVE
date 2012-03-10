/**
 * SC_simulators.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef SC_SIMULATORS_HH_
#define SC_SIMULATORS_HH_




#include "../../../simulators/simulator.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../util/util.hh"


using namespace Hive;

namespace SimpleCell
{

	class OU_NoiseSimulator : public Simulator
	{
		public:
			OU_NoiseSimulator(double tau, double stdDev, double meanCheYp, int tracesNeeded);
			~OU_NoiseSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			double tau;
			double stdDev;
			double meanCheYp;

			double variance;
			double sqrtOfD;

			int tracesNeeded;
			double *cheYpLevel;

			double dt;
			double sqrtOfDt;
			double dCheYp;

			double internalTime;
	};



	class MotorSimulator : public Simulator
	{
		public:
			MotorSimulator(int nMotors, double Kd, double g1, double w);
			~MotorSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			int nMotors;
			int *motorStates;
			double *cheYpLevel;

			double Kd;
			double g1;
			double w;

			double dt;

			bool areMotorsCorrelated;

			double internalTime;

			Hive::IntVectorData *currentMotorStates;
	};



	class ConformationModelSimulator : public Simulator
	{
		public:
			ConformationModelSimulator(double waitingTimeInSemi, int minMotorsInCCWtoRun);
			~ConformationModelSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:
			double internalTime;

			//model parameters
			double waitingTimeInSemi;
			int minMotorsInCCWtoRun;

			//motor state information
			Hive::IntVectorData *currentMotorStates;
			int nMotors;
			int *lastMotorState;

			int *flagellaState;
			int *lastFlagellaState;
			double *intervalTime;
			double *timeInSemi;


			//cell state information
			Hive::IntegerData * currentCellState;
			Hive::IntegerData * lastCellState;



	};













	class SimpleSignalingSimulator : public Simulator
	{
		public:
			SimpleSignalingSimulator(double tau, double stdDev, double meanCheYp, int tracesNeeded);
			~SimpleSignalingSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:

			// parameters of the noise and mean cheYp
			double tau;
			double stdDev;
			double meanCheYp;

			// calculated parameters (from input parameters)
			double variance;
			double sqrtOfD;
			double sigM;// = 1/e1 * (alpha/((1-(alpha/meanYp))*meanYp*meanYp ))*sigmaY;


			//Input parameter that is set from the environment
			double currentLigConc;// = 0;


			// internal parameters for simulation
			double dt;
			double sqrtOfDt;
			double Mdot;
			double TarTerm;
			double TsrTerm;

			double internalTime;




			//activity, activation, and cluster parameters
			double m0;
			double alpha;

			double e1;
			double e0;

			double KoffTar;
			double KoffTsr;
			double KonTar;
			double KonTsr;
			int TarCount;
			int TsrCount;




			// terms to keep track of the traces
			int tracesNeeded;
			double *methLevel;// = new double [tracesNeeded];
			double *act;// = new double [tracesNeeded];
			double *cheYp;// = new double [tracesNeeded];

	};




	class SimpleLinearEnv : public Simulator
	{
		public:
			SimpleLinearEnv(double slope, double intercept);
			~SimpleLinearEnv();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);

		protected:

			void initialise();


		private:

			// parameters of the noise and mean cheYp
			double slope;
			double intercept;

			DoubleVectorData * ligand;
			DoubleVectorData *position;
	};


}





#endif /* SC_SIMULATORS_HH_ */
