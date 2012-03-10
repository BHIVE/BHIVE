/**
 * SC_simulators.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */



#include "cellSimulators.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"

#include <math.h>


using namespace std;
using namespace Hive;
using namespace ChemoPop;




///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////



MotorSimulator::MotorSimulator()
{
//	this->nMotors = nMotors;
//	this->motorStates = new int[nMotors];
//	for (int i=0; i<this->nMotors; i++) motorStates[i]=CellParameterData::CW;
//	this->Kd = Kd;
//	this->g1 = g1;
//	this->w = w;
//	this->cheYpLevel = 0;
	this->dt=0.01;
//
//	this->areMotorsCorrelated = true;
//	this->internalTime=0;
}

MotorSimulator::~MotorSimulator()
{
	delete [] motorStates;
}

void MotorSimulator::setAgent (Agent *ag) {
	this->agent = ag;



	//First, figure out how many motors we have and if the motor traces are coordinated
	//and the data to store the motor output
	Data *d = agent->getDatabase()->getDataItem("n_motors");
	this->n_motors = ((IntegerData *)d);
	d = agent->getDatabase()->getDataItem("MotorStates");
	this->motorStates = ((TVectorData <int> *)d);

	d = agent->getDatabase()->getDataItem("isFlagellaCoordinated");
	this->areMotorsCorrelated = ((BoolData *)d)->getBool();


	// get CheYp Input value for the cell (or multiple if we need multiple traces per cell)
	d = agent->getDatabase()->getDataItem("CheYp");
	this->CheYp = (TVectorData <double> *) d;



	// Grab the CellTime clock and the EQ time
	d = agent->getDatabase()->getDataItem("celltime");
	this->celltime = (DoubleData *) d;
	d = agent->getDatabase()->getDataItem("eqtime");
	this->eqtime = (DoubleData *)d;


	// Get Motor Parameters
	d = agent->getDatabase()->getDataItem("motor_Kd");
	this->Kd = (DoubleData *) d;
	d = agent->getDatabase()->getDataItem("motor_g1");
	this->g1 = (DoubleData *) d;
	d = agent->getDatabase()->getDataItem("motor_w");
	this->w = (DoubleData *) d;






	if(areMotorsCorrelated) {

		double deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(0)/(Kd->getDouble()+CheYp->at(0))));
		double kPlus = w->getDouble()*exp(deltaG);
		double kMinus = w->getDouble()*exp(-deltaG);
		double cwBias = kMinus/(kPlus+kMinus);
		for (int m=0; m<n_motors->getInteger(); m++) {
			if(Util::RANDOM_CLOSED()<=cwBias) this->motorStates->at(m)=CellParameterData::CW;
			else this->motorStates->at(m)=CellParameterData::CCW;
		}

	} else {
		for(int i=0; i<CheYp->size(); i++) {
			double deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(i)/(Kd->getDouble()+CheYp->at(i))));
			double kPlus = w->getDouble()*exp(deltaG);
			double kMinus = w->getDouble()*exp(-deltaG);
			double cwBias = kMinus/(kPlus+kMinus);
			if(Util::RANDOM_CLOSED()<=cwBias) this->motorStates->at(i)=CellParameterData::CW;
			else this->motorStates->at(i)=CellParameterData::CCW;
		}

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
		deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(0)/(Kd->getDouble()+CheYp->at(0))));
		kPlus = w->getDouble()*exp(deltaG);
		kMinus = w->getDouble()*exp(-deltaG);


		//cout<<internalTime<<" corr motors:";
		for(int m=0; m<this->n_motors->getInteger(); m++) {

			// let's do one step per iteration from now on
			//for (double currentTime=0; currentTime<t; currentTime+=dt)
			//{
				double randNum = Util::RANDOM_CLOSED();
				if(motorStates->at(m)==CellParameterData::CW) {
					if(randNum<(kPlus*dt)) {
						motorStates->at(m) = CellParameterData::CCW;
					} else {
						motorStates->at(m) = CellParameterData::CW;
					}

				} else /*if(motorStates[m]==CCW) */ {
					if(randNum>=(1-(kMinus*dt))) {
						motorStates->at(m) = CellParameterData::CW;
					} else {
						motorStates->at(m) = CellParameterData::CCW;
					}
				}
			//}
			//cout<<" "<<motorStates[m];
		}
		//cout<<"\n";
	} else {


		//cout<<internalTime<<" uncorr motors:";
		for(int m=0; m<this->n_motors->getInteger(); m++) {
			deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(m)/(Kd->getDouble()+CheYp->at(m))));
			kPlus = w->getDouble()*exp(deltaG);
			kMinus = w->getDouble()*exp(-deltaG);

			//for (double currentTime=0; currentTime<t; currentTime+=dt)
			//{
				double randNum = Util::RANDOM_CLOSED();
				if(motorStates->at(m)==CellParameterData::CW) {
					if(randNum<(kPlus*dt)) {
						motorStates->at(m) = CellParameterData::CCW;
					} else {
						motorStates->at(m) = CellParameterData::CW;
					}
				} else /*if(motorStates[m]==CCW) */ {
					if(randNum>=(1-(kMinus*dt))) {
						motorStates->at(m) = CellParameterData::CW;
					} else {
						motorStates->at(m) = CellParameterData::CCW;
					}
				}
			//}
			//cout<<" "<<motorStates[m];
		}
		//cout<<"\n";
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








ConformationModelSimulator::ConformationModelSimulator()
{
//	this->waitingTimeInSemi = waitingTimeInSemi;
//	this->minMotorsInCCWtoRun = minMotorsInCCWtoRun;
//
//	this->nMotors = 0;
//	this->lastMotorState = NULL;
//	this->intervalTime = NULL;
//	this->timeInSemi=NULL;
//	this->internalTime = 0;
}

ConformationModelSimulator::~ConformationModelSimulator()
{
//	delete [] lastMotorState;
//	delete [] intervalTime;
//	delete [] timeInSemi;
}

void ConformationModelSimulator::setAgent (Agent *ag) {
	this->agent = ag;




	// Grab the CellTime clock and the EQ time
	Data *d = agent->getDatabase()->getDataItem("celltime");
	this->celltime = (DoubleData *) d;
	d = agent->getDatabase()->getDataItem("eqtime");
	this->eqtime = (DoubleData *)d;



	// Grab the Motor input data
	d = agent->getDatabase()->getDataItem("n_motors");
	this->n_motors = ((IntegerData *)d);
	d = agent->getDatabase()->getDataItem("MotorStates");
	this->currentMotorStates = ((TVectorData <int> *)d);
	d = agent->getDatabase()->getDataItem("LastMotorStates");
	this->lastMotorState = ((TVectorData <int> *)d);


	// Grab the Flagella state data data
	d = agent->getDatabase()->getDataItem("minNumberOfMotorsInCCWneededToSwim");
	this->minNumberOfMotorsInCCWneededToSwim = ((IntegerData *)d);
	d = agent->getDatabase()->getDataItem("minTimeInSemi");
	this->waitingTimeInSemi = (DoubleData *)d;


	//flagella state information
	d = agent->getDatabase()->getDataItem("FlagellaStates");
	this->flagellaStates = ((TVectorData <int> *)d);
	d = agent->getDatabase()->getDataItem("LastFlagellaStates");
	this->lastFlagellaStates = ((TVectorData <int> *)d);

	d = agent->getDatabase()->getDataItem("intervalTime");
	this->intervalTime = ((TVectorData <double> *)d);
	d = agent->getDatabase()->getDataItem("timeInSemi");
	this->timeInSemi = ((TVectorData <double> *)d);



	// initialize this data as we see fit
	for (int m=0; m<n_motors->getInteger(); m++) {
		this->lastMotorState->at(m)=currentMotorStates->at(m);
		this->intervalTime->at(m) = 0;
		this->timeInSemi->at(m) = 0;

		if(lastMotorState->at(m) == CellParameterData::CCW) {

			flagellaStates->at(m)=CellParameterData::NORMAL;
			lastFlagellaStates->at(m)=CellParameterData::NORMAL;
		} else {
			flagellaStates->at(m)=CellParameterData::CURLY;
			lastFlagellaStates->at(m)=CellParameterData::CURLY;
		}
	}




	//look up the place to dump our results
	d = agent->getDatabase()->getDataItem("swimming_state");
	this->swimming_state = ((IntegerData *)d);
	swimming_state->setInteger(CellParameterData::TUMBLE);
	d = agent->getDatabase()->getDataItem("last_swimming_state");
	this->last_swimming_state = ((IntegerData *)d);
	last_swimming_state->setInteger(CellParameterData::TUMBLE);

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
	for(int m=0; m<n_motors->getInteger(); m++) {
		intervalTime->at(m)=intervalTime->at(m)+dt;
	//	cout<<intervalTime[m]<<" ";
	}
	//cout<<endl;

	//Remember our previous flagella state.
	for(int m=0;m<n_motors->getInteger(); m++)
		this->lastFlagellaStates->at(m) = this->flagellaStates->at(m);


    // update the interval time if we changed states
	for(int m=0; m<n_motors->getInteger(); m++) {

		//if we switched to CW from CCW....
		if(currentMotorStates->at(m)==CellParameterData::CW &&
				lastMotorState->at(m)==CellParameterData::CCW ) {

			//reset interval time, and generate how long we will stay in the semi state
			intervalTime->at(m) = dt;
			timeInSemi->at(m)=-log(Util::RANDOM_CLOSED())*this->waitingTimeInSemi->getDouble();

		} else if (currentMotorStates->at(m)==CellParameterData::CCW &&
				lastMotorState->at(m)==CellParameterData::CW) {

			//reset interval time, set time in semi to zero
			intervalTime->at(m)=dt;
			timeInSemi->at(m)=0;
		}
	}



	//Update the flagella state based on the amount of time we are in any particular state
	for(int m=0;m<n_motors->getInteger(); m++)
	{
		//First see if we've been in the CW state long enough to be in the CURLY conformation
		if(currentMotorStates->at(m)==CellParameterData::CW)
		{
			if(intervalTime->at(m) > (norm2semiTime+minSemiTime+timeInSemi->at(m))) {
				flagellaStates->at(m)=CellParameterData::CURLY;
			} else if (intervalTime->at(m) > norm2semiTime) {
				flagellaStates->at(m)=CellParameterData::SEMI;
			}
		}

		//Otherwise, we are in the normal conformation...
		if(currentMotorStates->at(m) == CellParameterData::CCW &&
				intervalTime->at(m) > curly2normal)
		{
			flagellaStates->at(m)=CellParameterData::NORMAL;
		}
	}


	//Now we have to count how many flagella are in each state...
	int numInNormal=0;
	int numInSemi=0;
	for(int m=0;m<n_motors->getInteger(); m++) {
		if(flagellaStates->at(m)==CellParameterData::SEMI) {
			numInSemi++;
		}
		if(flagellaStates->at(m)==CellParameterData::NORMAL) {
			numInNormal++;
		}
	}


	// Based on the values, tell the swim to Run or Tumble
	this->last_swimming_state->setInteger(this->swimming_state->getInteger());
	//cout<<numInNormal<<" "<<numInSemi<<endl;
	if(numInNormal>=this->minNumberOfMotorsInCCWneededToSwim->getInteger() && numInSemi==0) {
		swimming_state->setInteger(CellParameterData::RUN);
		//cout<<"Run!!"<<endl;
	} else {
		swimming_state->setInteger(CellParameterData::TUMBLE);
		//cout<<"Tumble!!"<<endl;
	}


	//Remember the last motor state
	for(int m=0;m<n_motors->getInteger(); m++)
		this->lastMotorState->at(m) = this->currentMotorStates->at(m);



	// And we updated the database, so we are done!
	//cout<<internalTime<<" flagella state: ";
	//for(int m=0;m<nMotors; m++) cout<<" "<<flagellaState[m];
	//cout<<"\n";


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












SimpleSignalingSimulator::SimpleSignalingSimulator()
{
	// internal parameters for simulation, initialized to default values
	this->dt = 0.01; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);
	this->internalTime=0;
	this->Mdot=0;
	this->TarTerm=1;
	this->TsrTerm=1;
	this->ligIndex=0;
}

SimpleSignalingSimulator::~SimpleSignalingSimulator()
{
	delete [] methLevel;
	delete [] act;
}


void SimpleSignalingSimulator::initialise() { }
void SimpleSignalingSimulator::prepare() { }
void SimpleSignalingSimulator::synchroniseWithDatabase() { }


void SimpleSignalingSimulator::setAgent(Agent *ag)
{
	this->agent = ag;

	// First, figure out if the cell is chemotactic or not.
	Data *d = agent->getDatabase()->getDataItem("isChemotactic");
	if(d->getType().compare("IntegerType")==0) {
		this->isChemotactic = (IntegerData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}



	// set receptor cluster parameters
	d = agent->getDatabase()->getDataItem("alpha");
	if(d->getType().compare("DoubleType")==0) {
		this->alpha = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
	d = agent->getDatabase()->getDataItem("e1");
	if(d->getType().compare("DoubleType")==0) {
		this->e1 = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
	d = agent->getDatabase()->getDataItem("e0");
	if(d->getType().compare("DoubleType")==0) {
		this->e0 = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}


	d = agent->getDatabase()->getDataItem("KoffTar");
	if(d->getType().compare("DoubleType")==0) {
		this->KoffTar = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
	d = agent->getDatabase()->getDataItem("KoffTsr");
	if(d->getType().compare("DoubleType")==0) {
		this->KoffTsr = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}


	d = agent->getDatabase()->getDataItem("KonTar");
	if(d->getType().compare("DoubleType")==0) {
		this->KonTar = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
	d = agent->getDatabase()->getDataItem("KonTsr");
	if(d->getType().compare("DoubleType")==0) {
		this->KonTsr = (DoubleData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}

	d = agent->getDatabase()->getDataItem("TarCount");
	if(d->getType().compare("IntegerType")==0) {
		this->TarCount = (IntegerData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}
	d = agent->getDatabase()->getDataItem("TsrCount");
	if(d->getType().compare("IntegerType")==0) {
		this->TsrCount = (IntegerData *)d;
	} else {
		throw HiveException("Database is not set properly for simulator","SimpleSignalingSimulator::setAgent(Agent *ag)");
	}


	// Determine how many traces are needed by extracting the number of flagella
	// and the flag that tells us whether or not
	d = agent->getDatabase()->getDataItem("n_motors");
	int n_motors = ((IntegerData *)d)->getInteger();
	d = agent->getDatabase()->getDataItem("isFlagellaCoordinated");
	bool isFlagellaCoordinated = ((BoolData *)d)->getBool();

	if (isFlagellaCoordinated)
		this->tracesNeeded = 1;
	else
		this->tracesNeeded = n_motors;


	// Grab the Ligand Concentration Data
	d = agent->getDatabase()->getDataItem("ligands");
	this->ligands = (TVectorData <double> *) d;

	d = agent->getDatabase()->getDataItem("accumulated_ligands");
	this->accumulated_ligands = (TVectorData <double> *)d;


	// Grab the CellTime clock and the EQ time
	d = agent->getDatabase()->getDataItem("celltime");
	this->celltime = (DoubleData *) d;


	d = agent->getDatabase()->getDataItem("eqtime");
	this->eqtime = (DoubleData *)d;




	// get CheYp value for the cell (or multiple if we need multiple traces per cell)
	d = agent->getDatabase()->getDataItem("CheYp");
	this->CheYp = (TVectorData <double> *) d;



	//Get tau, and noise level
	d = agent->getDatabase()->getDataItem("tau");
	this->tau = (DoubleData *) d;

	d = agent->getDatabase()->getDataItem("sigma");
	this->stdDev = (DoubleData *) d;

	d = agent->getDatabase()->getDataItem("meanCheYp");
	this->meanCheYp = (DoubleData *) d;




	// calculated parameters (from input parameters)
	this->variance = (stdDev->getDouble()*meanCheYp->getDouble())*(stdDev->getDouble()*meanCheYp->getDouble());
	double D = (2.0*variance)/this->tau->getDouble();
	this->sqrtOfD = pow(D,0.5);

	this->m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();
	this->sigM = 1/e1->getDouble() *(alpha->getDouble()/((1-(alpha->getDouble()/meanCheYp->getDouble()))
					*this->meanCheYp->getDouble()*this->meanCheYp->getDouble() ))*(this->stdDev->getDouble()*this->meanCheYp->getDouble());



	//initialize all the traces that we need
	methLevel = (TVectorData<double> *)agent->getDatabase()->getDataItem("methLevel");
	act = new double [this->tracesNeeded];

	for (int trace=0; trace<this->tracesNeeded; trace++) {
		this->methLevel->at(trace) = 0;
		this->act[trace] = 0;
	}



	//initialize all the traces that we need around the given value
	if(this->isChemotactic->getInteger()==1) {
		TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
		TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
	} else {
		TarTerm = 1;
		TsrTerm = 1;
	}

	m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();
	for (int trace=0; trace<this->tracesNeeded; trace++) {
		this->methLevel->at(trace) = this->sigM*Util::RANDOM_GAUSSIAN()+this->m0;
		this->act[trace] = 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*methLevel->at(trace))*TarTerm*TsrTerm));
		this->CheYp->at(trace)=alpha->getDouble() * act[trace];
	}
}


void SimpleSignalingSimulator::step(double t)
{

//	cerr<<"start"<<endl;
//	cerr<<"this->CheYp->at(0)  "<<this->CheYp->at(0)<<endl;

	this->dt = t; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);

	//This simulator is always running, so update the time here
	this->celltime->addToValue(dt);


	// calculate the activity from the ligand and resting methylation state
	// Otherwise, if we are not chemotactic, the Tar and Tsr term should be set to 1, which
	// was already taken care of earlier, so let's not waste time here.
	if(this->isChemotactic->getInteger()==1) {
		TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
		TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
	} //else { TarTerm = 1; TsrTerm = 1; }
	m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();


	// calculate the next cheY-p value for each trace needed
	for(int trace=0; trace<tracesNeeded; trace++) {

		// first the change in methylation state
		Mdot = -(methLevel->at(trace)-m0) / tau->getDouble();
		methLevel->at(trace) = methLevel->at(trace) + Mdot*dt + sigM*sqrt(2/tau->getDouble()) * Util::RANDOM_GAUSSIAN()*sqrtOfDt;

		if(methLevel->at(trace)>8) methLevel->at(trace)=8;
		if(methLevel->at(trace)<0) methLevel->at(trace)=0;

		// then the new activity
		act[trace] = 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*methLevel->at(trace))*TarTerm*TsrTerm));

		// finally, the updated CheYp value
		this->CheYp->at(trace)=alpha->getDouble() * act[trace];
	}


	//Before we leave, increment our accumulated ligand
	// - done in the updateCellLigandAction now.
	//for(int k=0; k<this->accumulated_ligands->size(); k++)
	//	accumulated_ligands->at(k)= accumulated_ligands->at(k)+ligands->at(k);

	internalTime += t;


}








///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////








VotingModelSimulator::VotingModelSimulator()
{
	//cout<<"blah"<<endl;
}

VotingModelSimulator::~VotingModelSimulator()
{
}

void VotingModelSimulator::setAgent (Agent *ag) {
	this->agent = ag;


	// Grab the Motor input data
	Data *d = agent->getDatabase()->getDataItem("n_motors");
	this->n_motors = ((IntegerData *)d);
	d = agent->getDatabase()->getDataItem("MotorStates");
	this->currentMotorStates = ((TVectorData <int> *)d);


	// Grab the Flagella parameters
	d = agent->getDatabase()->getDataItem("minNumberOfMotorsInCCWneededToSwim");
	this->minNumberOfMotorsInCCWneededToSwim = ((IntegerData *)d);



	//look up the place to dump our results
	d = agent->getDatabase()->getDataItem("swimming_state");
	this->swimming_state = ((IntegerData *)d);
	swimming_state->setInteger(CellParameterData::TUMBLE);

	d = agent->getDatabase()->getDataItem("last_swimming_state");
	this->last_swimming_state = ((IntegerData *)d);
	last_swimming_state->setInteger(CellParameterData::TUMBLE);
}

void VotingModelSimulator::step(double t)
{
	//query the number of motors in CCW
	int numOfMotorsInCCW = 0;
	for(int m=0; m<n_motors->getInteger(); m++)
		if(currentMotorStates->at(m)==CellParameterData::CCW  )
				numOfMotorsInCCW++;


	this->last_swimming_state->setInteger(this->swimming_state->getInteger());

	// Based on the number of motors in CCW, tell the swim to Run or Tumble
	if(numOfMotorsInCCW>=this->minNumberOfMotorsInCCWneededToSwim->getInteger()) {
		swimming_state->setInteger(CellParameterData::RUN);
	} else {
		swimming_state->setInteger(CellParameterData::TUMBLE);
	}

}


void VotingModelSimulator::prepare() {};
void VotingModelSimulator::synchroniseWithDatabase() {};
void VotingModelSimulator::initialise() {};




