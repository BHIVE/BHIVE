/**
 * SC_simulators.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef CELLSIMULATORS_HH_
#define CELLSIMULATORS_HH_


#include "../../../../core/simulators/simulator.hh"
#include "../../../../core/agent/data/primitive/primitiveData.hh"
#include "../../../../core/util/util.hh"
#include "../../../../core/agent/data/data.hh"
#include "../../../../core/exception/exception.hh"
#include "../../chemopopData/historyCollector.hh"

#include "../../chemopopInitialiser/cellparameterinitializer.hh"

#include <cmath>

using namespace std;
using namespace Hive;

namespace ChemoPop
{

    class FullSignalingSimulator : public Simulator
	{
		public:
    		FullSignalingSimulator();
			~FullSignalingSimulator();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();

			void setAgent (Agent *ag);



			////////////////////
			// PARAMETERS THAT DO NOT CHANGE IN TIME AND SHOULD BE FIXED
			DoubleData *kr; // catalytic CheR meth rate
			DoubleData *kb; // catalytic CheB-P demeth rate
			DoubleData *Kr; // michaelis constant for CheR meth
			DoubleData *Kb; // michaelis constant for CheB-P meth

			DoubleData *ab; // phosphorylation rate of CheB
			DoubleData *db; // dephosphorylation rate of CheB-P

			DoubleData *ap; // autophosphorylation rate of CheA

			DoubleData *ay; // phosphorylation rate of CheY
			DoubleData *dy; // dephosphorylation rate of CheY


			///////////////////
			// CELL PARAMETERS THAT MAY CHANGE FROM CELL TO CELL
			DoubleData *Rtot; // total CheR
			DoubleData *Btot; // total CheB
			DoubleData *Ttot; // total Receptors
			DoubleData *Atot; // total CheA
			DoubleData *Ytot; // total CheY
			DoubleData *Ztot; // total CheZ


			///////////////////
			// VARIABLES THAT DO CHANGE IN TIME
			DoubleData *activity; // activity = Tstar/Ttot
			DoubleData *M;        // average methylation level per receptor
			DoubleData *Bp;       // [CheB-P]
			DoubleData *Ap;       // [CheA-P]
			DoubleData *Yp;       // [CheY-P]
			DoubleData *Tstar;    // active receptors

			// calculated parameters (from input parameters)
			double variance;
			double sqrtOfD;
			double meth_noise_parameter;
			DoubleData *tauNoise;
			DoubleData *CVofCheYP;

			//Input parameter that is set from the environment and output to couple the motors
			int ligIndex;
			TVectorData<double> *ligands;
			TVectorData<double> *accumulated_ligands;
			TVectorData<double> *YpCouplingVector;


			DoubleData *eqtime;
			DoubleData *celltime;
			double internalTime;

			IntegerData *integrationMethod;
			IntegerData *integrationSubSteps;
			BoolData *useSteadyStateApprox;


			// internal parameters for simulation
			double dt;
			double sqrtOfDt;
			double Mdot;
			double TarTerm;
			double TsrTerm;

			//activity, activation, and cluster parameters
			DoubleData *e1;
			DoubleData *e0;

			DoubleData *KoffTar;
			DoubleData *KoffTsr;
			DoubleData *KonTar;
			DoubleData *KonTsr;

			IntegerData *TarCount;
			IntegerData *TsrCount;

			// data that we track for outputting, this is the
			// change over the last time step
			DoubleData *ActivityForOutput;
			DoubleData *CheYpForOutput;
			DoubleData *MethylationForOutput;

			DoubleData *dLig;
			DoubleData *dCheYP;
			DoubleData *dActivity;
			DoubleData *dMeth;

			double lastLig;
			double lastCheYp;
			double lastActivity;
			double lastMeth;


			HistoryCollector *hcLig;
			HistoryCollector *hcdlogLig;
			HistoryCollector *hcCheYP;
			HistoryCollector *hcAct;
			HistoryCollector *hcMeth;


		protected:

				void initialise();


		private:

	};




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
			DoubleData *tauAdapt;
			DoubleData *tauNoise;
			DoubleData *stdDev;
			DoubleData *meanCheYp;

			// calculated parameters (from input parameters)
			double variance;
			double sqrtOfD;
			double sigM;// = 1/e1 * (alpha/((1-(alpha/meanYp))*meanYp*meanYp ))*sigmaY;

			vector<double> lastCheYpForNoise;

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

			// data that we track for outputting, this is the
			// change over the last time step
			DoubleData *dLig;
			DoubleData *dCheYP;
			DoubleData *dActivity;
			DoubleData *dMeth;


			double lastLig;
			double lastCheYp;
			double lastActivity;
			double lastMeth;


			HistoryCollector *hcLig;
			HistoryCollector *hcdlogLig;
			HistoryCollector *hcCheYP;
			HistoryCollector *hcAct;
			HistoryCollector *hcMeth;

			/// parameters for Williams updated model for the methylation dynamics
			BoolData   *is_wills_methylation_model;
			DoubleData *kr;
			DoubleData *Kr;
			DoubleData *kb;
			DoubleData *Kb;
			DoubleData *noise_parameter; // = D

			DoubleData *a0;
			DoubleData *y0;
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


			DoubleData * cellTime;

			HistoryCollector *hcMotor;
			HistoryCollector *hcRunHistory;

			HistoryAggregator *haLig_TUMBLE;
			HistoryAggregator *haCheYP_TUMBLE;
			HistoryAggregator *haAct_TUMBLE;
			HistoryAggregator *haMeth_TUMBLE;
			HistoryAggregator *haMotor_TUMBLE;
			HistoryAggregator *haRunHistory_TUMBLE;
			HistoryAggregator *hadlogLig_TUMBLE;


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
