/**
 * SC_simulators.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */



#include "SC_simulators.hh"
#include "../simpleCell.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"

#include <math.h>


using namespace std;
using namespace Hive;
using namespace SimpleCell;

OU_NoiseSimulator::OU_NoiseSimulator(double tau, double stdDev, double meanCheYp, int tracesNeeded)
{
	this->tau = tau;
	this->stdDev = stdDev;
	this->meanCheYp = meanCheYp;

	this->variance = (stdDev*meanCheYp)*(stdDev*meanCheYp);
	double D = (2.0*variance)/this->tau;
	this->sqrtOfD = pow(D,0.5);

	//initialize all the traces that we need
	this->tracesNeeded = tracesNeeded;
	this->cheYpLevel = new double [this->tracesNeeded];
	for (int trace=0; trace<tracesNeeded; trace++) {
		this->cheYpLevel[trace] = (stdDev*meanCheYp)*Util::RANDOM_GAUSSIAN()+meanCheYp;
	}

	this->dt = 0.01; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);
	this->dCheYp = 0;
	this->internalTime=0;
	//cout<<"here"<<endl; exit(1);
}

OU_NoiseSimulator::~OU_NoiseSimulator()
{
	delete [] cheYpLevel;
}

void OU_NoiseSimulator::setAgent(Agent *ag) {
	this->agent = ag;

	// Make sure agent has a CheYp data object in its database, and if so, set it.
	Data *yp = agent->getDatabase()->getDataItem("CheYp");
	if(yp->getType().compare("DoubleVectorType")==0) {
		for (int trace=0; trace<tracesNeeded; trace++) {
			((DoubleVectorData *)yp)->setDouble(this->cheYpLevel[trace],trace);
		}
	} else {
		throw HiveException("Database is not set properly for simulator, requires DoubleVectorData named 'CheYp'",
				"OU_NoiseSimulator::setAgent(Agent *ag)");
	}
}

void OU_NoiseSimulator::initialise() { }

void OU_NoiseSimulator::step(double t)
{
	this->dt = t; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);

	//CheYp should not be changed from last time, otherwise we have problems.  But we
	//won't catch this here because I assume I did everything perfectly the first time.

	//Simulate each trace as needed
	//cout<<internalTime<<" cheYp: ";
	for(int trace=0; trace<tracesNeeded; trace++) {
		//Only perform one step at each iteration
		//for (double currentTime = 0; currentTime<t; currentTime+=dt) {
			dCheYp = -(1.0/tau)*(cheYpLevel[trace]-meanCheYp)*dt+sqrtOfD*Util::RANDOM_GAUSSIAN()*sqrtOfDt;
			cheYpLevel[trace] = cheYpLevel[trace] + dCheYp;
			if(cheYpLevel[trace]<0) cheYpLevel[trace] = 0;
		//}
		//cout<<" "<<cheYpLevel[trace];
	}
	//cout<<"\n";

	//save the new CheYp concentration in the database
	DoubleVectorData *dvd = ((DoubleVectorData *)agent->getDatabase()->getDataItem("CheYp"));
	for (int trace=0; trace<tracesNeeded; trace++) {
		dvd->setDouble(this->cheYpLevel[trace],trace);
	}

	internalTime += t;
}

void OU_NoiseSimulator::prepare() { }

void OU_NoiseSimulator::synchroniseWithDatabase() { }







///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////



MotorSimulator::MotorSimulator(int nMotors, double Kd, double g1, double w)
{
	this->nMotors = nMotors;
	this->motorStates = new int[nMotors];
	for (int i=0; i<this->nMotors; i++) motorStates[i]=SimpleCellConstants::CW;
	this->Kd = Kd;
	this->g1 = g1;
	this->w = w;
	this->cheYpLevel = 0;
	this->dt=0.01;

	this->areMotorsCorrelated = true;
	this->internalTime=0;
}

MotorSimulator::~MotorSimulator()
{
	delete [] motorStates;
}

void MotorSimulator::setAgent (Agent *ag) {
	this->agent = ag;

	// Make sure agent has a CheYp data object in its database, and if so, grab it and
	// Initialize motors to a default state based on CheYp levels
	Data *yp = agent->getDatabase()->getDataItem("CheYp");
	if(yp->getType().compare("DoubleVectorType")==0) {

		int cheYtraces = ((DoubleVectorData *)yp)->getNumElements();
		if(cheYtraces<1) {
			throw HiveException(string("Database is not set properly for motor simulator, requires")+
					string(" DoubleVectorData named 'CheYp' to have more than one element"),
					"SingleMotorSimulator::setAgent(Agent *ag)");
		}
		if(cheYtraces>1) {
			if(cheYtraces!=nMotors) {
				throw HiveException(string("Database is not set properly for motor simulator, requires")+
									string(" DoubleVectorData named 'CheYp' to have same number of elements as") +
									string(" there are motors if motors are to be not correlated"),
									"SingleMotorSimulator::setAgent(Agent *ag)");
			}
			this->areMotorsCorrelated=false;
		}

		if(areMotorsCorrelated) {
			this->cheYpLevel = new double [1];
			this->cheYpLevel[0] = ((DoubleVectorData *)yp)->getDouble(0);

			double deltaG = (g1/2.0)*((1.0/2.0)-(cheYpLevel[0]/(Kd+cheYpLevel[0])));
			double kPlus = w*exp(deltaG);
			double kMinus = w*exp(-deltaG);
			double cwBias = kMinus/(kPlus+kMinus);
			for (int m=0; m<nMotors; m++) {
				if(Util::RANDOM_CLOSED()<=cwBias) this->motorStates[m]=SimpleCellConstants::CW;
				else this->motorStates[m]=SimpleCellConstants::CCW;
			}

		} else {
			this->cheYpLevel = new double [cheYtraces];
			for(int i=0; i<cheYtraces; i++) {
				this->cheYpLevel[i] = ((DoubleVectorData *)yp)->getDouble(i);
				double deltaG = (g1/2.0)*((1.0/2.0)-(cheYpLevel[i]/(Kd+cheYpLevel[i])));
				double kPlus = w*exp(deltaG);
				double kMinus = w*exp(-deltaG);
				double cwBias = kMinus/(kPlus+kMinus);
				if(Util::RANDOM_CLOSED()<=cwBias) this->motorStates[i]=SimpleCellConstants::CW;
				else this->motorStates[i]=SimpleCellConstants::CCW;
			}
		}

	} else {
		throw HiveException(string("Database is not set properly for motor simulator, requires")+
							string(" DoubleVectorData named 'CheYp'"),
							"SingleMotorSimulator::setAgent(Agent *ag)");
	}


	// Make sure agent has a MotorStates data object in its database for output
	Data *motorStates = agent->getDatabase()->getDataItem("MotorStates");
	if(motorStates->getType().compare("IntVectorType")==0) {

			this->currentMotorStates = (IntVectorData *)motorStates;
			for (int m=0; m<nMotors; m++) {
				currentMotorStates->setInt(this->motorStates[m],m);
			}

			if(currentMotorStates->getNumElements()!=nMotors) {
				throw HiveException(string("Database is not set properly for motor simulator, requires")+
						string(" IntVectorData named 'MotorStates' to have same number of elements as motors"),
						"SingleMotorSimulator::setAgent(Agent *ag)");
			}

	} else {
		throw HiveException(string("Database is not set properly for motor conformation simulator, requires")+
							string(" IntVectorData named 'MotorStates'"),
							"SingleMotorSimulator::setAgent(Agent *ag)");
	}


}

void MotorSimulator::step(double t)
{
	//variables we need
	this->dt = t;
	double deltaG = 0;
	double kPlus = 0;
	double kMinus = 0;

	// Depending on whether or not the motor traces are correlated, we have to simulate
	// slightly differently (mostly for performance gains, we have the top level if statement
	if(this->areMotorsCorrelated) {

		//get the CheYp value
		this->cheYpLevel[0] = ((DoubleVectorData *)agent->getDatabase()->getDataItem("CheYp"))->getDouble(0);
		deltaG = (g1/2.0)*((1.0/2.0)-(cheYpLevel[0]/(Kd+cheYpLevel[0])));
		kPlus = w*exp(deltaG);
		kMinus = w*exp(-deltaG);


		//cout<<internalTime<<" corr motors:";
		for(int m=0; m<this->nMotors; m++) {

			// let's do one step per iteration from now on
			//for (double currentTime=0; currentTime<t; currentTime+=dt)
			//{
				double randNum = Util::RANDOM_CLOSED();
				if(motorStates[m]==SimpleCellConstants::CW) {
					if(randNum<(kPlus*dt)) {
						motorStates[m] = SimpleCellConstants::CCW;
					} else {
						motorStates[m] = SimpleCellConstants::CW;
					}

				} else /*if(motorStates[m]==CCW) */ {
					if(randNum>=(1-(kMinus*dt))) {
						motorStates[m] = SimpleCellConstants::CW;
					} else {
						motorStates[m] = SimpleCellConstants::CCW;
					}
				}
			//}
			//cout<<" "<<motorStates[m];
		}
		//cout<<"\n";
	} else {

		DoubleVectorData *dvd = ((DoubleVectorData *)agent->getDatabase()->getDataItem("CheYp"));
		for (int m=0; m<this->nMotors; m++) {
			this->cheYpLevel[m] = dvd->getDouble(m);
		}

		//cout<<internalTime<<" uncorr motors:";
		for(int m=0; m<this->nMotors; m++) {
			deltaG = (g1/2.0)*((1.0/2.0)-(cheYpLevel[m]/(Kd+cheYpLevel[m])));
			kPlus = w*exp(deltaG);
			kMinus = w*exp(-deltaG);

			//for (double currentTime=0; currentTime<t; currentTime+=dt)
			//{
				double randNum = Util::RANDOM_CLOSED();
				if(motorStates[m]==SimpleCellConstants::CW) {
					if(randNum<(kPlus*dt)) {
						motorStates[m] = SimpleCellConstants::CCW;
					} else {
						motorStates[m] = SimpleCellConstants::CW;
					}
				} else /*if(motorStates[m]==CCW) */ {
					if(randNum>=(1-(kMinus*dt))) {
						motorStates[m] = SimpleCellConstants::CW;
					} else {
						motorStates[m] = SimpleCellConstants::CCW;
					}
				}
			//}
			//cout<<" "<<motorStates[m];
		}
		//cout<<"\n";
	}

	//Update the motor states in the database
	for (int m=0; m<nMotors; m++) {
		currentMotorStates->setInt(this->motorStates[m],m);
	}

	internalTime += t;
}

void MotorSimulator::prepare() {};
void MotorSimulator::synchroniseWithDatabase() {};
void MotorSimulator::initialise() {};














///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////








ConformationModelSimulator::ConformationModelSimulator(double waitingTimeInSemi, int minMotorsInCCWtoRun)
{
	this->waitingTimeInSemi = waitingTimeInSemi;
	this->minMotorsInCCWtoRun = minMotorsInCCWtoRun;

	this->nMotors = 0;
	this->lastMotorState = NULL;
	this->intervalTime = NULL;
	this->timeInSemi=NULL;
	this->internalTime = 0;
}

ConformationModelSimulator::~ConformationModelSimulator()
{
	delete [] lastMotorState;
	delete [] intervalTime;
	delete [] timeInSemi;
}

void ConformationModelSimulator::setAgent (Agent *ag) {
	this->agent = ag;

	// Make sure agent has a MotorStates data object in its database, and if so, grab it
	Data *motorStates = agent->getDatabase()->getDataItem("MotorStates");
	if(motorStates->getType().compare("IntVectorType")==0) {

		this->currentMotorStates = (IntVectorData *)motorStates;
		this->nMotors = currentMotorStates->getNumElements();

		if(nMotors<1) {
			throw HiveException(string("Database is not set properly for motor conformation simulator, requires")+
					string(" IntVectorData named 'MotorStates' to have more than one element"),
					"SingleMotorSimulator::setAgent(Agent *ag)");
		}

		this->lastMotorState = new int[nMotors];
		this->intervalTime = new double [nMotors];
		this->timeInSemi = new double [nMotors];
		this->flagellaState = new int[nMotors];
		this->lastFlagellaState = new int[nMotors];
		for (int m=0; m<nMotors; m++) {
			lastMotorState[m] = currentMotorStates->getInt(m);
			intervalTime[m] = 0;
			timeInSemi[m]=0;
			if(lastMotorState[m]==SimpleCellConstants::CCW) {
				flagellaState[m]=SimpleCellConstants::NORMAL;
				lastFlagellaState[m]=SimpleCellConstants::NORMAL;
			} else {
				flagellaState[m]=SimpleCellConstants::CURLY;
				lastFlagellaState[m]=SimpleCellConstants::CURLY;
			}
		}
	} else {
		throw HiveException(string("Database is not set properly for motor conformation simulator, requires")+
							string(" IntVectorData named 'MotorStates'"),
							"ConformationModelSimulator::setAgent(Agent *ag)");
	}

	// Also check that we have a place to dump our results, namely, the CurrentCellState and LastCellState
	// variables must be on the DB
	Data *current = agent->getDatabase()->getDataItem("CurrentCellState");
	Data *last = agent->getDatabase()->getDataItem("LastCellState");
	if(current->getType().compare("IntegerType")==0 && last->getType().compare("IntegerType")==0) {
		this->currentCellState=(IntegerData *)current;
		this->lastCellState=(IntegerData *)last;
	} else {
		throw HiveException(string("Database is not set properly for motor conformation simulator, requires")+
							string(" IntData named 'CurrentCellState' and 'LastCellState'"),
							"ConformationModelSimulator::setAgent(Agent *ag)");
	}


}

void ConformationModelSimulator::step(double t)
{

    //Model Variables that do not change, ever. (add the extra .00001 to avoid rounding errors)
    double norm2semiTime = 0.0200001;
	double curly2normal = 0.0100001;
	double minSemiTime = 0.0200001;
	double dt = t;


	// For double checking that we have the correct motor values
	//	cout<<internalTime<<" in conf:";
	//	for(int m=0; m<nMotors; m++) {
	//		cout<<" "<<currentMotorStates->getInt(m);
	//	}
	//	cout<<"\n";



	//Update the interval time for each motor, assuming no change in state
	for(int m=0; m<nMotors; m++) {
		intervalTime[m]=intervalTime[m]+dt;
	//	cout<<intervalTime[m]<<" ";
	}
	//cout<<endl;

	//Remember our previous flagella state.
	for(int m=0;m<nMotors; m++)
		this->lastFlagellaState[m] = this->flagellaState[m];


    // update the interval time if we changed states
	for(int m=0; m<nMotors; m++) {

		//if we switched to CW from CCW....
		if(currentMotorStates->getInt(m)==SimpleCellConstants::CW &&
				lastMotorState[m]==SimpleCellConstants::CCW ) {

			//reset interval time, and generate how long we will stay in the semi state
			intervalTime[m] = dt;
			timeInSemi[m]=-log(Util::RANDOM_CLOSED())*this->waitingTimeInSemi;

		} else if (currentMotorStates->getInt(m)==SimpleCellConstants::CCW &&
				lastMotorState[m]==SimpleCellConstants::CW) {

			//reset interval time, set time in semi to zero
			intervalTime[m]=dt;
			timeInSemi[m]=0;
		}
	}



	//Update the flagella state based on the amount of time we are in any particular state
	for(int m=0;m<nMotors; m++)
	{
		//First see if we've been in the CW state long enough to be in the CURLY conformation
		if(currentMotorStates->getInt(m)==SimpleCellConstants::CW)
		{
			if(intervalTime[m]>(norm2semiTime+minSemiTime+timeInSemi[m])) {
				flagellaState[m]=SimpleCellConstants::CURLY;
			} else if (intervalTime[m]>norm2semiTime) {
				flagellaState[m]=SimpleCellConstants::SEMI;
			}
		}

		//Otherwise, we are in the normal conformation...
		if(currentMotorStates->getInt(m)==SimpleCellConstants::CCW &&
				intervalTime[m]>curly2normal)
		{
			flagellaState[m]=SimpleCellConstants::NORMAL;
		}
	}


	//Now we have to count how many flagella are in each state...
	int numInNormal=0;
	int numInSemi=0;
	for(int m=0;m<nMotors; m++) {
		if(flagellaState[m]==SimpleCellConstants::SEMI) {
			numInSemi++;
		}
		if(flagellaState[m]==SimpleCellConstants::NORMAL) {
			numInNormal++;
		}
	}


	// Based on the values, tell the swim to Run or Tumble
	this->lastCellState->setInteger(this->currentCellState->getInteger());
	//cout<<numInNormal<<" "<<numInSemi<<endl;
	if(numInNormal>=minMotorsInCCWtoRun && numInSemi==0) {
		currentCellState->setInteger(SimpleCellConstants::RUN);
		//cout<<"Run!!"<<endl;
	} else {
		currentCellState->setInteger(SimpleCellConstants::TUMBLE);
		//cout<<"Tumble!!"<<endl;
	}


	//Remember the last motor state
	for(int m=0;m<nMotors; m++)
		this->lastMotorState[m] = this->currentMotorStates->getInt(m);



	// And we updated the database, so we are done!
	//cout<<internalTime<<" flagella state: ";
	//for(int m=0;m<nMotors; m++) cout<<" "<<flagellaState[m];
	//cout<<"\n";

	//cout<<internalTime<<" cell state: "<<this->currentCellState->getInteger()<<"\n";
	internalTime += t;
}

void ConformationModelSimulator::prepare() {};
void ConformationModelSimulator::synchroniseWithDatabase() {};
void ConformationModelSimulator::initialise() {};












///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////












SimpleSignalingSimulator::SimpleSignalingSimulator(double tau, double stdDev, double meanCheYp, int tracesNeeded)
{
	cout<<"creating simple signaling simulator.\n";
	// parameters of the noise and mean cheYp
	this->tau = tau;
	this->stdDev = stdDev;
	this->meanCheYp = meanCheYp;
	this->tracesNeeded = tracesNeeded;

	//Fixed parameters of the cluster that you must change by hand
	this->alpha = 6.0;

	this->e0 = 1.0*(19);
	this->e1 = -0.45*(19);

	this->KoffTar   = 0.02e-3; // This is 0.02 mM or 0.00002 M
	this->KonTar    = 0.5e-3; // mM
	this->KoffTsr   = 100e-3; // mM
	this->KonTsr    = 1000; //Yes, 1000 M, or 10^6 mM
	this->TarCount  = 6;
	this->TsrCount  = 13;


	//Input parameter that is set from the environment, initialized to zero
	this->currentLigConc = 0;

	// internal parameters for simulation, initialized to default values
	this->dt = 0.01; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);
	this->internalTime=0;
	this->Mdot=0;
	this->TarTerm=1;
	this->TsrTerm=1;


	// calculated parameters (from input parameters)
	this->variance = (stdDev*meanCheYp)*(stdDev*meanCheYp);
	double D = (2.0*variance)/this->tau;
	this->sqrtOfD = pow(D,0.5);

	this->m0 = (log((alpha/meanCheYp)-1) -e0 -log(TarTerm) -log(TsrTerm))/e1;
	this->sigM = 1/e1 * (alpha/((1-(alpha/meanCheYp))*this->meanCheYp*this->meanCheYp ))*(this->stdDev*this->meanCheYp);


	// terms to keep track of the traces
	methLevel = new double [this->tracesNeeded];
	act = new double [this->tracesNeeded];
	cheYp = new double [this->tracesNeeded];

	//initialize all the traces that we need
	for (int trace=0; trace<this->tracesNeeded; trace++) {
		this->methLevel[trace] = 0;
		this->act[trace] = 0;
		this->cheYp[trace] = 0;

	}
}

SimpleSignalingSimulator::~SimpleSignalingSimulator()
{
	//delete [] cheYpLevel;
}

void SimpleSignalingSimulator::setAgent(Agent *ag) {
	this->agent = ag;

	// Make sure agent has a CheYp data object in its database, and if so, set it.
	Data *yp = agent->getDatabase()->getDataItem("CheYp");
	if(yp->getType().compare("DoubleVectorType")==0) {
		for (int trace=0; trace<tracesNeeded; trace++) {
			((DoubleVectorData *)yp)->setDouble(this->cheYp[trace],trace);
		}
	} else {
		throw HiveException("Database is not set properly for simulator, requires DoubleVectorData named 'CheYp'",
				"SimpleSignalingSimulator::setAgent(Agent *ag)");
	}


	// Grab the Ligand information, and make sure it is there
	Data *ligand = agent->getDatabase()->getDataItem("Chemical");
	if(ligand->getType().compare("DoubleVectorType")==0) {
		this->currentLigConc = ((DoubleVectorData *)ligand)->getDouble(0);
	} else {
		throw HiveException("Database is not set properly for simulator, requires DoubleVectorData named 'CheYp'",
				"SimpleSignalingSimulator::setAgent(Agent *ag)");
	}


	//initialize all the traces that we need around the given value
	cout<<"CheYp :"<<endl;
	TarTerm=pow( (1.0+(currentLigConc/KoffTar)) / (1.0+(currentLigConc/KonTar)) ,TarCount);
	TsrTerm=pow( (1.0+(currentLigConc/KoffTsr)) / (1.0+(currentLigConc/KonTsr)) ,TsrCount);
	m0 = (log((alpha/meanCheYp)-1) -e0 -log(TarTerm) -log(TsrTerm))/e1;
	for (int trace=0; trace<this->tracesNeeded; trace++) {
		this->methLevel[trace] = this->sigM*Util::RANDOM_GAUSSIAN()+this->m0;
		this->act[trace] = 1.0/(1.0+(exp(e0+e1*methLevel[trace])*TarTerm*TsrTerm));
		this->cheYp[trace] = alpha * act[trace];
	}
}

void SimpleSignalingSimulator::initialise() { }

void SimpleSignalingSimulator::step(double t)
{
	this->dt = t; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);

	// look up ligand concentration
	this->currentLigConc = ((DoubleVectorData *)agent->getDatabase()->getDataItem("Chemical"))->getDouble(0);

	// calculate the activity from the ligand and resting methylation state
	TarTerm=pow( (1.0+(currentLigConc/KoffTar)) / (1.0+(currentLigConc/KonTar)) ,TarCount);
	TsrTerm=pow( (1.0+(currentLigConc/KoffTsr)) / (1.0+(currentLigConc/KonTsr)) ,TsrCount);
	m0 = (log((alpha/meanCheYp)-1) -e0 -log(TarTerm) -log(TsrTerm))/e1;

	// calculate the next cheY-p value for each trace needed
	for(int trace=0; trace<tracesNeeded; trace++) {

		// first the change in methylation state
		Mdot = -(methLevel[trace]-m0) / tau;
		methLevel[trace] = methLevel[trace] + Mdot*dt + sigM*sqrt(2/tau) * Util::RANDOM_GAUSSIAN()*sqrtOfDt;

		// then the new activity
		act[trace] = 1.0/(1.0+(exp(e0+e1*methLevel[trace])*TarTerm*TsrTerm));

		// finally, the updated CheYp value
		cheYp[trace] = alpha * act[trace];

	}


	// and before we leave, save the new CheYp concentration in the database
	DoubleVectorData *dvd = ((DoubleVectorData *)agent->getDatabase()->getDataItem("CheYp"));
	for (int trace=0; trace<tracesNeeded; trace++) {
		dvd->setDouble(this->cheYp[trace],trace);
	}

	internalTime += t;
}

void SimpleSignalingSimulator::prepare() { }

void SimpleSignalingSimulator::synchroniseWithDatabase() { }










SimpleLinearEnv::SimpleLinearEnv(double slope, double intercept)
{
	this->slope = slope;
	this->intercept = intercept;
	this->ligand=0; this->position=0;
}

SimpleLinearEnv::~SimpleLinearEnv() {}

void SimpleLinearEnv::step(double t)
{
	// use the X position (getDouble(1)) to update ligand concentration
	double newLig = 0.4*exp(0.005*position->getDouble(1));

			//slope*position->getDouble(1)+intercept;
	if(newLig<0) newLig = 0;

	// set the ligand concentration
	this->ligand->setDouble(newLig,0);
}

void SimpleLinearEnv::prepare() {}

void SimpleLinearEnv::synchroniseWithDatabase() {}
void SimpleLinearEnv::setAgent (Agent *ag)
{
	this->agent = ag;


	// Grab the Ligand information, and make sure it is there
	Data *chemical = agent->getDatabase()->getDataItem("Chemical");
	if(chemical->getType().compare("DoubleVectorType")==0) {
		this->ligand = (DoubleVectorData *)chemical;
	} else {
		throw HiveException("Database is not set properly for simulator, requires DoubleVectorData named 'CheYp'",
				"SimpleSignalingSimulator::setAgent(Agent *ag)");
	}

	// Grab the position information, and make sure it is there
	Data *pos = agent->getDatabase()->getDataItem("Position");
	if(pos->getType().compare("DoubleVectorType")==0) {
		this->position = (DoubleVectorData *)pos;
	} else {
		throw HiveException("Database is not set properly for simulator, requires DoubleVectorData named 'CheYp'",
				"SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
}

void SimpleLinearEnv::initialise() {}








//cout<<"Cell "<<cellId<<" is equilibriating...  please wait."<<endl;
//
//this->currentLigConc = env->getLigConc(pos[X],pos[Y],pos[Z],currentTime);
//
////declare some variables we will need
//double deltaG=0,kPlus=0,kMinus=0,rand=0;
//int numOfMotorsTumbling = 0;
//double cheYp=0, Mdot=0;
//double sqrtDt = sqrt(dt);
//double TarTerm=0;
//double TsrTerm=0;
//
////Mean methylation level from input
//double m0 = log( alpha/meanYp-1 )/e1 - e0/e1;
//
////sigM from sigmaY
//double sigM = 1/e1 * (alpha/((1-(alpha/meanYp))*meanYp*meanYp ))*sigmaY;
//
//
//double eqTime = 0;
//
//while(eqTime<=duration)
//{
//
//	//Calculate activity from ligand
//	TarTerm=pow( (1.0+(currentLigConc/KoffTar)) / (1.0+(currentLigConc/KonTar)) ,TarCount);
//	TsrTerm=pow( (1.0+(currentLigConc/KoffTsr)) / (1.0+(currentLigConc/KonTsr)) ,TsrCount);
//
//	m0 = (log((alpha/meanYp)-1) -e0 -log(TarTerm) -log(TsrTerm))/e1;
//
//	//Calculate change in methylation level
//		Mdot = -( methLevel - m0 ) / tau;
//		//Mdot = Mdot*dt+(sigmaM*NFutil::RANDOM_GAUSSIAN())*(sqrtDt);
//		methLevel = methLevel + Mdot * dt + sigM * NFutil::RANDOM_GAUSSIAN()*(sqrtDt);
//
//	if(methLevel<0) methLevel=0;
//	if(methLevel>8) methLevel=8;
//
//	//Calculate new activity
//	act = 1.0/(1.0+(exp(e0+e1*methLevel) *TarTerm *TsrTerm));
//
//	//Calculate CheYp
//	cheYp = alpha * act;
//
//	/////////  Step the motors
//	deltaG = (g1/2.0)*((1.0/2.0)-(cheYp/(Kd+cheYp)));
//	kPlus=w*exp(deltaG);
//	kMinus=w*exp(-deltaG);
//
//	for(int m=0; m<motorCount; m++)
//	{
//	  rand=NFutil::RANDOM(1.0);
//	  if(mot[m]==CW) {
//		  if(rand<(kPlus*dt)) {
//			  mot[m]=CCW;
//		  } else {
//			  mot[m]=CW;
//		  }
//	  } else if(mot[m]==CCW) {
//		  if(rand>=(1-(kMinus*dt))) {
//			  mot[m]=CW;
//		  } else {
//			  mot[m]=CCW;
//		  }
//	  }
//	}
//
//	eqTime+=dt;
//}
//}
//
//
//
//double SimpleCell::stepTo(double endTime, double dt)
//{
////declare some variables we will need
//double deltaG=0,kPlus=0,kMinus=0,rand=0;
//int numOfMotorsTumbling = 0;
//double cheYp=0, Mdot=0;
//double sqrtDt = sqrt(dt);
//double TarTerm=0;
//double TsrTerm=0;
//
////Mean methylation level from input
//double m0 = log( alpha/meanYp-1 )/e1 - e0/e1;
//
//
////sigM from sigmaY
//double sigM = 1/e1 * (alpha/((1-(alpha/meanYp))*meanYp*meanYp ))*sigmaY;
//
////endTime = 1000;
////ofstream popFile;
////popFile.open("/home/msneddon/Desktop/Cap09/r1/cell.txt");
//
//while(currentTime<=endTime)
//{
//	//Calculate activity from ligand
//	TarTerm=pow( (1.0+(currentLigConc/KoffTar)) / (1.0+(currentLigConc/KonTar)) ,TarCount);
//	TsrTerm=pow( (1.0+(currentLigConc/KoffTsr)) / (1.0+(currentLigConc/KonTsr)) ,TsrCount);
//
//	m0 = (log((alpha/meanYp)-1) -e0 -log(TarTerm) -log(TsrTerm))/e1;
//
//	//Calculate change in methylation level
//		Mdot = -( methLevel - m0 ) / tau;
//		//cout<<Mdot<<endl;
//		//Mdot = Mdot*dt+(sigmaM*NFutil::RANDOM_GAUSSIAN())*(sqrtDt);
//		methLevel = methLevel + Mdot * dt + sigM*sqrt(2/tau) * NFutil::RANDOM_GAUSSIAN()*(sqrtDt);
//
//	if(methLevel<0) methLevel=0;
//	if(methLevel>8) methLevel=8;
//
//	//Calculate new activity
//	act = 1.0/(1.0+(exp(e0+e1*methLevel) *TarTerm *TsrTerm));
//
//	//Calculate CheYp
//	cheYp = alpha * act;
//
//	/////////  Step the motors
//	//cout<<"a "<<act<<endl;
//	//cout<<cheYp<<endl;
//	deltaG = (g1/2.0)*((1.0/2.0)-(cheYp/(Kd+cheYp)));
//	kPlus=w*exp(deltaG);
//	kMinus=w*exp(-deltaG);
//
//	numOfMotorsTumbling=0;
//	for(int m=0; m<motorCount; m++)
//	{
//	  rand=NFutil::RANDOM(1.0);
//	  if(mot[m]==CW) {
//		  if(rand<(kPlus*dt)) {
//			  mot[m]=CCW;
//			  //record the switching from CW to CCW
//			  motorFileStream<<currentTime<<"\t"<<m<<"\t"<<CCW<<"\n";
//
//		  } else {
//			  mot[m]=CW;
//			  numOfMotorsTumbling++;
//		  }
//	  } else if(mot[m]==CCW) {
//		  if(rand>=(1-(kMinus*dt))) {
//			  mot[m]=CW;
//			  numOfMotorsTumbling++;
//			  //record the switching from CCW to CW
//			  motorFileStream<<currentTime<<"\t"<<m<<"\t"<<CW<<"\n";
//
//		  } else {
//			  mot[m]=CCW;
//		  }
//	  }
//	}
//
//
//
//	///////// Move the cell
//	//check to see if the cell is now swimming or tumbling
//	if(numOfMotorsTumbling>0) {
//		this->CWtic++;
//		currentMovement=TUMBLE;      //////voting model for 5 motors
//	}
//	else {
//		this->CCWtic++;
//		currentMovement = SWIM;
//	}
//
//	//If we start tumbling, but were swimming, we need to choose a new direction
//	if(currentMovement==TUMBLE && lastMovement==SWIM)
//	{
//	  //This lets us change directions based on the gamma distribution
//	  this->changeDirDistribution();
//	  //This lets us change directions completely randomly
//	  //this->changeDirRandom();
//
//	  //Update our last position, because we did not move
//	  lastMovement=TUMBLE;
//	}
//	//If we are swimming, it doesn't matter what we did last time, just swim already!
//	else if(currentMovement==SWIM)
//	{
//	  swimToNewPosition(dt);
//	  lastMovement=SWIM;
//	}
//
//
//	//Now update the current ligand concentration at the new location
//	currentLigConc= env->getLigConc(pos[X],pos[Y],pos[Z],currentTime);
//
//	//if(currentTime<500) {currentLigConc=0;}
//	//else {currentLigConc=1e-3;};
//	//popFile<<" "<<currentTime<<" "<<act<<" "<<methLevel<<" "<<cheYp<<"\n";
//
//	if(env->isInCap(this)) {
//		if(firstTimeInCap <0.1) {
//			this->firstTimeInCap=currentTime;
//		}
//	}
//
//
//
//
//	//and update the time
//	currentTime+=dt;
//}
//
////popFile.close();
////exit(1);
//return currentTime;





























