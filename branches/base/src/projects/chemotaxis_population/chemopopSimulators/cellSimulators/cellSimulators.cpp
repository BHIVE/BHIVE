/**
 * SC_simulators.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#include "cellSimulators.hh"

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
//		cerr << CheYp->at(0) << endl;
		double deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(0)/(Kd->getDouble()+CheYp->at(0))));
		double kPlus = w->getDouble()*exp(deltaG);
		double kMinus = w->getDouble()*exp(-deltaG);
		double cwBias = kMinus/(kPlus+kMinus);
//		cerr << kPlus << " " << kMinus << endl;
		for (int m=0; m<n_motors->getInteger(); m++) {
			if(Util::RANDOM_CLOSED()<=cwBias) this->motorStates->at(m)=CellParameterData::CW;
			else this->motorStates->at(m)=CellParameterData::CCW;
		}
//		cerr << kPlus << " " << kMinus << endl;
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

	// FOR DEBUGGING THE MOTOR CURVE
//	cerr<<"\n\ndata=[";
//	for(CheYp->at(0)=0; CheYp->at(0)<5; CheYp->at(0)+=0.1) {
//		deltaG = (g1->getDouble()/2.0)*((1.0/2.0)-(CheYp->at(0)/(Kd->getDouble()+CheYp->at(0))));
//		kPlus = w->getDouble()*exp(deltaG);
//		kMinus = w->getDouble()*exp(-deltaG);
//		double CWbias		=  kMinus/(kPlus+kMinus);
//		double SwitchFreq   =  kMinus*(1-CWbias) + kPlus*(CWbias);
//
//		cerr<<CheYp->at(0)<<","<<CWbias<<","<<SwitchFreq<<"; ...\n";
//	} cerr<<"]; figure; hold on; plot(data(:,1),data(:,2));\n\n";  exit(44);

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
	this->collectHistory = false;
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


	cellTime = (DoubleData *) agent->getDatabase()->getDataItem("celltime");


	if(agent->getDatabase()->existsDataItem("ligAggregator")) {
		this->collectHistory = true;
		tumbleTriggeredAverageOFFSET = (DoubleData *) agent->getDatabase()->getDataItem("tumbleTriggeredAverageOFFSET");
		haLig_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("ligAggregator");
		haCheYP_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("cheYpTumbleAggregator");
		haAct_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("actTumbleAggregator");
		haMeth_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("methTumbleAggregator");
		haMotor_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("motorTumbleAggregator");
		haRunHistory_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("runHistoryTumbleAggregator");
		hadlogLig_TUMBLE = (HistoryAggregator *) agent->getDatabase()->getDataItem("dlogligAggregator");

		hcMotor      = (HistoryCollector *) agent->getDatabase()->getDataItem("motorCWHistory");
		hcRunHistory = (HistoryCollector *) agent->getDatabase()->getDataItem("runHistory");
	} else {
		this->collectHistory = false;
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



	// check that we aggregate now if we have to
	// remember what happened right before the tumble
	if(this->collectHistory) {
		haCheYP_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		haLig_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		haAct_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		haMeth_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		haMotor_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		haRunHistory_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
		hadlogLig_TUMBLE->checkScheduledAggregations(this->cellTime->getDouble());
	}



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
//			cerr << this->waitingTimeInSemi->getDouble() << endl;
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


	// Detect if we are starting a run or tumble, and record accordingly
	if(last_swimming_state->getInteger() == CellParameterData::RUN  &&
			swimming_state->getInteger() == CellParameterData::TUMBLE  ) {

		// remember what happened right before the tumble
		if(this->collectHistory) {
			haCheYP_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			haLig_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			haAct_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			haMeth_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			haMotor_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			haRunHistory_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
			hadlogLig_TUMBLE->scheduleAggregation(this->cellTime->getDouble()+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0));
		}
	}
	//else if (last_swimming_state->getInteger() == CellParameterData::TUMBLE  &&
	//		swimming_state->getInteger() == CellParameterData::RUN  ) {
	//
	//}


	// record the current state
	int n_cw = 0;
	for(int m=0;m<n_motors->getInteger(); m++) {
		if(currentMotorStates->at(m)==CellParameterData::CW) { n_cw++; }
	}

	if(this->collectHistory) {
		hcMotor->makeHistory((double)n_cw);
		hcRunHistory->makeHistory(swimming_state->getInteger());
	}

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


FullSignalingSimulator::FullSignalingSimulator()
{
	// internal parameters for simulation, initialized to default values
	this->dt = 0.01;
	this->sqrtOfDt = pow(this->dt,0.5);
	this->internalTime=0;
	this->TarTerm=1;
	this->TsrTerm=1;
	this->ligIndex=0;
	collectHistory=false;
}

FullSignalingSimulator::~FullSignalingSimulator() { }
void FullSignalingSimulator::initialise() { }
void FullSignalingSimulator::prepare() { }
void FullSignalingSimulator::synchroniseWithDatabase() { }


/*  helper function used just in FullSignalingSimulator to validate the agent database */
DoubleData *validateDoubleData(string dataname, Database *d) {
	if(!d->existsDataItem(dataname))
		throw HiveException("Database is not set properly for simulator, missing data named: '"+dataname+"'",
				"FullSignalingSimulator::setAgent(Agent *ag)");

	Data *datum = d->getDataItem(dataname);
	if(datum->getType().compare("DoubleType")!=0)
		throw HiveException("Database is not set properly for simulator, missing data named: '"+dataname+"'",
				"validateDoubleData(string dataname, Database *d)");
	return (DoubleData *)datum;
}
/*  helper function used just in FullSignalingSimulator to validate the agent database */
IntegerData *validateIntegerData(string dataname, Database *d) {
	if(!d->existsDataItem(dataname))
		throw HiveException("Database is not set properly for simulator, missing data named: '"+dataname+"'",
				"FullSignalingSimulator::setAgent(Agent *ag)");

	Data *datum = d->getDataItem(dataname);
	if(datum->getType().compare("IntegerType")!=0)
		throw HiveException("Database is not set properly for simulator, missing data named: '"+dataname+"'",
				"validateIntegerData(string dataname, Database *d)");
	return (IntegerData *)datum;
}



void FullSignalingSimulator::setAgent(Agent *ag)
{
	// Save the agent and get its database
	this->agent = ag;
	Database *database = agent->getDatabase();


	// Try to load parameter pointers from the database
	try {

		// FIRST THE REACTION RATES AND CONSTANTS
		kr=validateDoubleData("FM_kr",database);// catalytic CheR meth rate
		kb=validateDoubleData("FM_kb",database);// catalytic CheB-P demeth rate
		Kr=validateDoubleData("FM_BIGKr",database);// michaelis constant for CheR meth
		Kb=validateDoubleData("FM_BIGKb",database); // michaelis constant for CheB-P meth

		ab=validateDoubleData("FM_ab",database); // phosphorylation rate of CheB
		db=validateDoubleData("FM_db",database); // dephosphorylation rate of CheB-P

		ay=validateDoubleData("FM_ay",database); // phosphorylation rate of CheY
		dy=validateDoubleData("FM_dy",database); // dephosphorylation rate of CheY-P

		ap=validateDoubleData("FM_ap",database); // autphosphorylation rate of CheA


		// SECOND THE PROTEIN CONCENTRATIONS (IN MICROMOLAR)
		Rtot=validateDoubleData("FM_Rtot",database); // total CheR
		Btot=validateDoubleData("FM_Btot",database); // total CheB
		Ttot=validateDoubleData("FM_Ttot",database); // total Receptors
		Atot=validateDoubleData("FM_Atot",database); // total CheA
		Ytot=validateDoubleData("FM_Ytot",database); // total CheY
		Ztot=validateDoubleData("FM_Ztot",database); // total CheZ

		// THIRD THE TIME DEPENDENT VARIABLES
		activity=validateDoubleData("FM_activity",database);  // activity = Tstar/Ttot
		M=validateDoubleData("FM_M",database);    // average methylation level per receptor
		Bp=validateDoubleData("FM_Bp",database);   // [CheB-P]
		Ap=validateDoubleData("FM_Ap",database);   // [CheA-P]
		Yp=validateDoubleData("FM_Yp",database);   // [CheY-P]
		Tstar=validateDoubleData("FM_Tstar",database); // active receptors

		// FOURTH THE RECEPTOR CLUSTER PARAMETERS
		e1=validateDoubleData("e1",database);
		e0=validateDoubleData("e0",database);
		KoffTar=validateDoubleData("KoffTar",database);;
		KoffTsr=validateDoubleData("KoffTsr",database);;
		KonTar=validateDoubleData("KonTar",database);;
		KonTsr=validateDoubleData("KonTsr",database);
		TarCount=validateIntegerData("TarCount",database);
		TsrCount=validateIntegerData("TsrCount",database);

		// FIFTH: NOISE PARAMETERS
		tauNoise=validateDoubleData("tau_noise",database);
		CVofCheYP=validateDoubleData("noise_parameter",database);

		// SIXTH: OUTPUT VARIABLES
		ActivityForOutput=validateDoubleData("aktivitaet",database);
		CheYpForOutput=validateDoubleData("cheyphos",database);
		MethylationForOutput=validateDoubleData("methyl",database);
		dLig=validateDoubleData("dLigand",database);
		dCheYP=validateDoubleData("dCheYP",database);
		dActivity=validateDoubleData("dActivity",database);
		dMeth=validateDoubleData("dMeth",database);

		if(agent->getDatabase()->existsDataItem("ligHistory")) {
				this->collectHistory = true;
				hcLig   = (HistoryCollector *) agent->getDatabase()->getDataItem("ligHistory");
				hcCheYP   = (HistoryCollector *) agent->getDatabase()->getDataItem("cheYpHistory");
				hcAct   = (HistoryCollector *) agent->getDatabase()->getDataItem("actHistory");
				hcMeth   = (HistoryCollector *) agent->getDatabase()->getDataItem("methHistory");
				hcdlogLig   = (HistoryCollector *) agent->getDatabase()->getDataItem("dlogligHistory");
		} else {
			this->collectHistory = false;
		}

		// SEVENTH: GET THE OTHER STUFF
		// Grab the Ligand Concentration Data
		ligands = (TVectorData <double> *) database->getDataItem("ligands");
		accumulated_ligands = (TVectorData <double> *)database->getDataItem("accumulated_ligands");
		// Grab the CellTime clock and the EQ time
		celltime = (DoubleData *) database->getDataItem("celltime");
		eqtime = (DoubleData *)database->getDataItem("eqtime");
		// grab the integration options
		integrationMethod = (IntegerData *) database->getDataItem("IntegrationMethod");
		integrationSubSteps = (IntegerData *)database->getDataItem("IntegrationSubSteps");
		useSteadyStateApprox = (BoolData *)database->getDataItem("UseSteadyStateApprox");

	} catch (HiveException& e) {
		e.addTraceException("FullSignalingSimulator::setAgent(Agent *ag)");
		throw e;
	}


	// Make sure that we are not trying to uncoordinate the motors (IF WE ARE THROW AN ERROR)
	if(database->existsDataItem("isFlagellaCoordinated")) {
		if(database->getDataItem("isFlagellaCoordinated")->getType().compare("BoolType")!=0) {
			if(((BoolData *)database->getDataItem("isFlagellaCoordinated"))->getBool()==false)
				throw HiveException("Cannot have uncoordinated motors in FullSignalingSimulator!",
								"FullSignalingSimulator::setAgent(Agent *ag)");
		}
	}

	// Make sure that we have the coupling variable between Yp and CheYp
	YpCouplingVector = (TVectorData <double> *)database->getDataItem("CheYp");


	// compute the noise parameters for the simulation from the steady-states
	variance = 0;
	double D = 0;
	sqrtOfD = sqrt(D);
	meth_noise_parameter = 0;
	//
	//double D = (2.0*variance)/this->tauNoise->getDouble();
	//this->sqrtOfD = pow(D,0.5);
//
//		// calculate the adaptation time tau tau
//		double local_tau = -1./((a-a*a)*((a*a*a*k2)/((a+K2)*(a+K2))-(3.0*a*a*k2)/(a+K2)+((1-a)*k1)/((1-a+K1)*(1-a+K1))-(k1)/(1-a+K1)));
//		this->tauNoise = new DoubleData("tau_noise", local_tau);
//
//		// get the coefficient of variation of the cheyp level
//		d = agent->getDatabase()->getDataItem("noise_parameter");
//		double cv = ((DoubleData *) d)->getDouble();
//
//		/// calculate variance (sigma^2) of the cheyp level
//		double v = cv*cheyp_mean*cv*cheyp_mean;
//
//		// calculate the noise parameters
//		double dYpda   = y0->getDouble()*a0->getDouble()/(pow(a0->getDouble()+a,2));
//		double dadM    = -e1->getDouble()*(a-a*a);
//		double sigma_M = sqrt(v)/(dYpda*dadM);
//		double D = 2.0*sigma_M*sigma_M/local_tau;
//		this->noise_parameter = new DoubleData("noise_parameter", sqrt(D));

	double meanMethylationLevel = M->getDouble();

	//INITALIZE OUR VARIABLES
	M->setDouble(  meanMethylationLevel  );
	TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
	TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
	activity->setDouble( 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*M->getDouble())*TarTerm*TsrTerm)) );
	Tstar->setDouble( activity->getDouble()*Ttot->getDouble());


	YpCouplingVector->at(0)=Yp->getDouble();

	lastLig=0;
	lastCheYp=0;
	lastActivity=0;
	lastMeth=0;
}




double giNoise (double x) { return 1; }
double giNoNoise (double x) { return 0; }

// q = "Dm" diffusion constant of m
double rk4_ti_step ( double x,double dt, double q,
  double fi ( double x, FullSignalingSimulator *fsSIM), double gi(double x), FullSignalingSimulator *fsSIM)
//****************************************************************************
//  Purpose:
//    RK4_TI_STEP takes one step of a stochastic Runge Kutta scheme.
//
//  Discussion:
//    The Runge-Kutta scheme is fourth-order, and suitable for time-invariant
//    systems in which F and G do not depend explicitly on time.
//
//    d/dx X(t,xsi) = F ( X(t,xsi) ) + G ( X(t,xsi) ) * w(t,xsi)
//
//    this method has been adapted for use in the chemotaxis population simulator
//
//  Licensing:
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//    06 July 2010
//
//  Author:
//    John Burkardt
//
//  Reference:
//
//    Jeremy Kasdin,
//    Runge-Kutta algorithm for the numerical integration of
//    stochastic differential equations,
//    Journal of Guidance, Control, and Dynamics,
//    Volume 18, Number 1, January-February 1995, pages 114-120.
//
//    Jeremy Kasdin,
//    Discrete Simulation of Colored Noise and Stochastic Processes
//    and 1/f^a Power Law Noise Generation,
//    Proceedings of the IEEE,
//    Volume 83, Number 5, 1995, pages 802-827.
//
//  Parameters:
//
//    Input, double X,  the value  of the variable at the current time.
//    Input, double dt, the time step.
//    Input, double q, the spectral density of the input white noise.
//
//    Input, double FI ( double X ), the name of the deterministic right hand side function.
//    Input, double GI ( double X ), the name of the stochastic right hand side function.
//
//    Output, double RK4_TI_STEP, the value at time T+dt.
{
  double a21;  double a31;  double a32;  double a41;  double a42;  double a43;  double a51;  double a52;  double a53;  double a54;
  double k1;  double k2;  double k3;  double k4;
  double q1;  double q2;  double q3;  double q4;
  double w1;  double w2;  double w3;  double w4;
  double x1;  double x2;  double x3;  double x4;
  double xstar;

  a21 =   2.71644396264860;
  a31 = - 6.95653259006152;
  a32 =   0.78313689457981;
  a41 =   0.0;
  a42 =   0.48257353309214;
  a43 =   0.26171080165848;
  a51 =   0.47012396888046;
  a52 =   0.36597075368373;
  a53 =   0.08906615686702;
  a54 =   0.07483912056879;

  q1 =   2.12709852335625;
  q2 =   2.73245878238737;
  q3 =  11.22760917474960;
  q4 =  13.36199560336697;

  x1 = x;
  w1 = Util::RANDOM_GAUSSIAN() * sqrt ( q1 * q / dt );
  k1 = dt * fi(x1,fsSIM) + dt * gi(x1) * w1;

  x2 = x1 + a21 * k1;
  w2 = Util::RANDOM_GAUSSIAN() * sqrt ( q2 * q / dt );
  k2 = dt * fi (x2,fsSIM) + dt * gi (x2) * w2;

  x3 = x1 + a31 * k1 + a32 * k2;
  w3 = Util::RANDOM_GAUSSIAN() * sqrt ( q3 * q / dt );
  k3 = dt * fi (x3,fsSIM) + dt * gi (x3) * w3;

  x4 = x1 + a41 * k1 + a42 * k2;
  w4 = Util::RANDOM_GAUSSIAN() * sqrt ( q4 * q / dt );
  k4 = dt * fi (x4,fsSIM) + dt * gi(x4) * w4;

  xstar = x1 + a51 * k1 + a52 * k2 + a53 * k3 + a54 * k4;

  return xstar;
}

double dMequation(double m, FullSignalingSimulator *fsSim) {
	double TtotMinusTstar = fsSim->Ttot->getDouble() - fsSim->Tstar->getDouble();
	double dM= (fsSim->kr->getDouble()*fsSim->Rtot->getDouble()*(1.0-fsSim->activity->getDouble()))  /  (fsSim->Kr->getDouble()+TtotMinusTstar)
		       -(fsSim->kb->getDouble()*fsSim->Bp->getDouble()*fsSim->activity->getDouble())  /  (fsSim->Kb->getDouble()+fsSim->Tstar->getDouble());
	return dM;
}

double dBp_equation(double m, FullSignalingSimulator *fsSim) {
	double plusBp = fsSim->ab->getDouble()*fsSim->Ap->getDouble()*(fsSim->Btot->getDouble()-fsSim->Bp->getDouble());
	double dBp    =   plusBp  -fsSim->db->getDouble()*fsSim->Bp->getDouble();
	return dBp;
}

double dYp_equation(double m, FullSignalingSimulator *fsSim) {
	double plusYp = fsSim->ay->getDouble()*fsSim->Ap->getDouble()*(fsSim->Ytot->getDouble()-fsSim->Yp->getDouble());
	double dYp    = plusYp    -fsSim->dy->getDouble()*fsSim->Ztot->getDouble()*fsSim->Yp->getDouble();
	return dYp;
}

double dAp_equation(double m, FullSignalingSimulator *fsSim) {
	double plusBp = fsSim->ab->getDouble()*fsSim->Ap->getDouble()*(fsSim->Btot->getDouble()-fsSim->Bp->getDouble());
	double plusYp = fsSim->ay->getDouble()*fsSim->Ap->getDouble()*(fsSim->Ytot->getDouble()-fsSim->Yp->getDouble());
	double dAp =   fsSim->ap->getDouble()*(fsSim->Atot->getDouble()-fsSim->Ap->getDouble())*fsSim->activity->getDouble()  -plusBp   -plusYp;
	return dAp;
}

void FullSignalingSimulator::step(double t)
{
	// STEP 0: some housekeeping of the timestep variable
	this->dt = t;
	this->sqrtOfDt = pow(this->dt,0.5);
	this->internalTime += dt;

	int n_substeps = this->integrationSubSteps->getInteger();
	double sub_dt = this->dt / (double)n_substeps;
	for(int step_k=0; step_k<n_substeps; step_k++) {

		// STEP 1: based on ligand concentration, compute the Tar and Tsr contributions to the activity function, then compute the current activity
		// based on the current methylation level.  We will use this activity value in our other calculations
		TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
		TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
		activity->setDouble( 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*M->getDouble())*TarTerm*TsrTerm)) );
		if(activity->getDouble()<0)  { activity->setDouble(0); }
		Tstar->setDouble( activity->getDouble()*Ttot->getDouble());
		if(Tstar->getDouble()<0)  { Tstar->setDouble(0); }

		double TtotMinusTstar = Ttot->getDouble() - Tstar->getDouble();


		// USE EULER INTEGRATION
		if(integrationMethod->getInteger()==0) {
			// STEP 2: compute dM/dt, which allows us to update the methylation level.  We calculate the value by
			// first adding the methylation (CheR) term, then the demethylation term (CheB), then the noise term.  We
			// then simply modify the current methylation level by Mdot and the noise.
			double dM  =    (kr->getDouble()*Rtot->getDouble()*(1.0-activity->getDouble()))  /  (Kr->getDouble()+TtotMinusTstar)
							-(kb->getDouble()*Bp->getDouble()*activity->getDouble())  /  (Kb->getDouble()+Tstar->getDouble());
			double dMn =    0; //meth_noise_parameter*Util::RANDOM_GAUSSIAN()*sqrt(sub_dt);

			// STEP 3: compute dAp, which allows us to update the CheA-P level
			double plusBp = ab->getDouble()*Ap->getDouble()*(Btot->getDouble()-Bp->getDouble());
			double plusYp = ay->getDouble()*Ap->getDouble()*(Ytot->getDouble()-Yp->getDouble());
			double dAp = ap->getDouble()*(Atot->getDouble()-Ap->getDouble())*activity->getDouble()  -plusBp   -plusYp;

			if(!useSteadyStateApprox->getBool()) {
				// STEP 4: compute dBp, which allows us to update the CheB-P level
				double dBp    =   plusBp  -db->getDouble()*Bp->getDouble();
				// STEP 5: compute dYp, which allows us to update the CheY-P level
				double dYp    = plusYp    -dy->getDouble()*Ztot->getDouble()*Yp->getDouble();
				Bp->setDouble( Bp->getDouble()+dBp*sub_dt );
				Yp->setDouble( Yp->getDouble()+dYp*sub_dt );
				//cerr<<"a"<<endl;
			} else {
				//cerr<<"b"<<endl;
				Bp->setDouble( Btot->getDouble() / (1 + (db->getDouble()) / (ab->getDouble()*Ap->getDouble()))   );
				Yp->setDouble( Ytot->getDouble() / (1 + (dy->getDouble()*Ztot->getDouble()) / (ay->getDouble()*Ap->getDouble())) );
			}
			M->setDouble(  M->getDouble() + dM*sub_dt + dMn  );
			Ap->setDouble( Ap->getDouble()+dAp*sub_dt );
		}

		// USE 4TH ORDER RUNGE-KUTTA
		else if(integrationMethod->getInteger()==1) {

			double methNoiseParameter = 0;
			double nextMeth = rk4_ti_step(M->getDouble(),sub_dt,methNoiseParameter,dMequation,giNoise,this);
			double nextAp = rk4_ti_step(Ap->getDouble(),sub_dt,0,dAp_equation,giNoNoise,this);

			if(!useSteadyStateApprox->getBool()) {
				double bNoiseParameter = 0;
				double nextBp = rk4_ti_step(Bp->getDouble(),sub_dt,bNoiseParameter,dBp_equation,giNoNoise,this);
				double nextYp = rk4_ti_step(Yp->getDouble(),sub_dt,0,dYp_equation,giNoNoise,this);
				Bp->setDouble( nextBp );
				Yp->setDouble( nextYp );
				//cerr<<"c"<<endl;
			} else {
				//cerr<<"d"<<endl;
				Bp->setDouble( Btot->getDouble() / (1 + (db->getDouble()) / (ab->getDouble()*Ap->getDouble()))   );
				Yp->setDouble( Ytot->getDouble() / (1 + (dy->getDouble()*Ztot->getDouble()) / (ay->getDouble()*Ap->getDouble())) );
			}
			M->setDouble(nextMeth);
			Ap->setDouble( nextAp );
		}
		else {
			throw HiveException("Unknown integration method selected (check DoubleData 'IntegrationMethod')!","FullSignalingSimulator::step()");
		}

		if(M->getDouble()>48)  { M->setDouble(48); }
		if(M->getDouble()<0)  { M->setDouble(0); }
		if(Yp->getDouble()<0) { Yp->setDouble(0); }
		if(Bp->getDouble()<0) { Bp->setDouble(0); }

	}

	//cerr<<	activity->getDouble()<<endl;
	//cerr<<	Ap->getDouble()<<endl;
	//cerr<<	Yp->getDouble()<<endl;
	//cerr<<	"---"<<endl;
	//exit(1);


	// STEP 7: update variables and pointer needed to assemble output later
	ActivityForOutput->setDouble(activity->getDouble());
	CheYpForOutput->setDouble(Yp->getDouble());
	MethylationForOutput->setDouble(M->getDouble());
	YpCouplingVector->at(0)=Yp->getDouble();

	if(collectHistory) {
		if(internalTime>eqtime->getDouble()) {
			hcCheYP->makeHistory(Yp->getDouble());
			hcLig->makeHistory(ligands->at(0));
			hcAct->makeHistory(activity->getDouble());
			hcMeth->makeHistory(M->getDouble());
			hcdlogLig->makeHistory((ligands->at(0)-lastLig) / ligands->at(0));
		}
	}

	dLig->setDouble(ligands->at(0)-lastLig);
	dCheYP->setDouble(Yp->getDouble()-lastCheYp);
	dActivity->setDouble(activity->getDouble()-lastActivity);
	dMeth->setDouble(M->getDouble()-lastMeth);
	lastLig=ligands->at(0);
	lastCheYp=Yp->getDouble();
	lastActivity=activity->getDouble();
	lastMeth=M->getDouble();
}










///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////


SimpleSignalingSimulator::SimpleSignalingSimulator()
{
	// internal parameters for simulation, initialized to default values
	this->dt = 0.01; //seconds  MAN THIS IS DANGEROUS!
	this->sqrtOfDt = pow(this->dt,0.5);
	this->internalTime=0;
	this->Mdot=0;
	this->TarTerm=1;
	this->TsrTerm=1;
	this->ligIndex=0;
	this->lastCheYpForNoise.reserve(0);
	this->collectHistory=false;
}

SimpleSignalingSimulator::~SimpleSignalingSimulator()
{
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

	/////--------------------------------------------------------------------------------------------------
	///// NEW 10-20-2010: set flag of whether methylation model to be used is the old one or Will's updated
	///// version
	d = agent->getDatabase()->getDataItem("is_wills_methylation_model");
	this->is_wills_methylation_model = (BoolData *) d;

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

	///// if this is the case we also need to get kr, kb, Kr, and Kb from the agent's database
	if (this->is_wills_methylation_model->getBool()) {
		// kr
		d = agent->getDatabase()->getDataItem("kr");
		this->kr = (DoubleData*) d;

		// Kr
		d = agent->getDatabase()->getDataItem("Kr");
		this->Kr = (DoubleData*) d;

		// kb
		d = agent->getDatabase()->getDataItem("kb");
		this->kb = (DoubleData*) d;

		// Kb
		d = agent->getDatabase()->getDataItem("Kb");
		this->Kb = (DoubleData*) d;

		// a0
		d = agent->getDatabase()->getDataItem("a0");
		this->a0 = (DoubleData *) d;

		// y0
		d = agent->getDatabase()->getDataItem("y0");
		this->y0 = (DoubleData *) d;

		// cheyp: mean cheyp_level
		d = agent->getDatabase()->getDataItem("cheyp_mean");
		double cheyp_mean = ((DoubleData *) d)->getDouble();

		// convert cheyp_mean to activity;
		double a = cheyp_mean*a0->getDouble()/(y0->getDouble()-cheyp_mean);

		// make calculating the adaptation time a little easier
		double k1 = kr->getDouble()*(-1.)*(e1->getDouble());
		double K1 = Kr->getDouble();
		double k2 = kb->getDouble()*(-1.)*(e1->getDouble());
		double K2 = Kb->getDouble();

		// calculate the adaptation time tau tau
		double local_tau = -1./((a-a*a)*((a*a*a*k2)/((a+K2)*(a+K2))-(3.0*a*a*k2)/(a+K2)+((1-a)*k1)/((1-a+K1)*(1-a+K1))-(k1)/(1-a+K1)));
		this->tauNoise = new DoubleData("tau_noise", local_tau);

		// get the coefficient of variation of the cheyp level
		d = agent->getDatabase()->getDataItem("noise_parameter");
		double cv = ((DoubleData *) d)->getDouble();

		/// calculate variance (sigma^2) of the cheyp level
		double v = cv*cheyp_mean*cv*cheyp_mean;

		// calculate the noise parameter
		double dYpda   = y0->getDouble()*a0->getDouble()/(pow(a0->getDouble()+a,2));
		double dadM    = -e1->getDouble()*(a-a*a);
		double sigma_M = sqrt(v)/(dYpda*dadM);
		double D = 2.0*sigma_M*sigma_M/local_tau;
		this->noise_parameter = new DoubleData("noise_parameter", sqrt(D));


	}
	///// END NEW 10-20-2010
	/////----------------------------------------------------------------------------------------------------

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

	// the following initialisation only has to be done, if
	// the cell is using the old methylation model
	if (!this->is_wills_methylation_model->getBool()) {
		//Get tau, and noise level
		d = agent->getDatabase()->getDataItem("tauAdapt");
		this->tauAdapt = (DoubleData *) d;
		//cerr<<"tauAdapt: "<<tauAdapt->getDouble()<<endl;

		d = agent->getDatabase()->getDataItem("tauNoise");
		this->tauNoise = (DoubleData *) d;
		//cerr<<"tauNoise: "<<tauNoise->getDouble()<<endl;

		d = agent->getDatabase()->getDataItem("sigma");
		this->stdDev = (DoubleData *) d;
		//cerr<<"sigma: "<<stdDev->getDouble()<<endl;

		d = agent->getDatabase()->getDataItem("cheyp_mean");
		this->meanCheYp = (DoubleData *) d;

		// calculated parameters (from input parameters)
		this->variance = (stdDev->getDouble()*meanCheYp->getDouble())*(stdDev->getDouble()*meanCheYp->getDouble());
		double D = (2.0*variance)/this->tauNoise->getDouble();
		this->sqrtOfD = pow(D,0.5);


		for (int trace=0; trace<this->tracesNeeded; trace++) {
			lastCheYpForNoise.push_back( (stdDev->getDouble())*Util::RANDOM_GAUSSIAN() );
		}

		this->m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();

		//OLD:  not needed, but not used so leave it for now.
		this->sigM = 1/e1->getDouble() *(alpha->getDouble()/((1-(alpha->getDouble()/meanCheYp->getDouble()))
					*this->meanCheYp->getDouble()*this->meanCheYp->getDouble() ))*(this->stdDev->getDouble()*this->meanCheYp->getDouble());
	}

	//initialize all the traces that we need
	methLevel = (TVectorData<double> *)agent->getDatabase()->getDataItem("methLevel");
	act = new double [this->tracesNeeded];

//	cerr << tracesNeeded << endl;
// 	exit(1);

	for (int trace=0; trace<this->tracesNeeded; trace++) {
		this->methLevel->at(trace) = 0;
		this->act[trace] = 0;
	}

	//initialize all the traces that we need around the given value
	if(this->isChemotactic->getInteger()==1) {
		TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
		TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
	} else {
		cout<<"WARNING: CELLS ARE NOT CHEMOTACTIC!"<<endl;
		TarTerm = 1;
		TsrTerm = 1;
	}

	/// this is the initialisation if it is not the wills methylation model
	if (!this->is_wills_methylation_model->getBool()) {
		//		m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();
		//		for (int trace=0; trace<this->tracesNeeded; trace++) {
		//			this->methLevel->at(trace) = this->sigM*Util::RANDOM_GAUSSIAN()+this->m0;
		//			this->act[trace] = 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*methLevel->at(trace))*TarTerm*TsrTerm));
		//			this->CheYp->at(trace)=alpha->getDouble() * act[trace];
		//		}
		for (int trace=0; trace<this->tracesNeeded; trace++) {
			this->methLevel->at(trace) = m0;
		}
	} else { // wills new model
		m0 = ((DoubleData *) agent->getDatabase()->getDataItem("meth_init"))->getDouble();
		// NOTE THAT ALL TRACES START WITH THE SAME INITIAL METHYLATION LEVEL!
		for (int trace=0; trace<this->tracesNeeded; trace++) {
			this->methLevel->at(trace) = m0;
			this->act[trace] = 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*methLevel->at(trace))*TarTerm*TsrTerm));
		}
	}





	// output values for changes from step to step in ligand and cheYp
	dLig      = (DoubleData *)agent->getDatabase()->getDataItem("dLigand");
	dCheYP    = (DoubleData *)agent->getDatabase()->getDataItem("dCheYP");
	dActivity = (DoubleData *)agent->getDatabase()->getDataItem("dActivity");
	dMeth     = (DoubleData *)agent->getDatabase()->getDataItem("dMeth");

	if(agent->getDatabase()->existsDataItem("ligHistory")) {
		this->collectHistory = true;
		hcLig   = (HistoryCollector *) agent->getDatabase()->getDataItem("ligHistory");
		hcCheYP   = (HistoryCollector *) agent->getDatabase()->getDataItem("cheYpHistory");
		hcAct   = (HistoryCollector *) agent->getDatabase()->getDataItem("actHistory");
		hcMeth   = (HistoryCollector *) agent->getDatabase()->getDataItem("methHistory");
		hcdlogLig   = (HistoryCollector *) agent->getDatabase()->getDataItem("dlogligHistory");

	} else {
		this->collectHistory = false;
	}

	lastLig=0;
	lastCheYp=0;
	lastActivity=0;
	lastMeth=0;

}


void SimpleSignalingSimulator::step(double t)
{
	this->dt = t; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);


	// calculate the activity from the ligand and resting methylation state
	// Otherwise, if we are not chemotactic, the Tar and Tsr term should be set to 1, which
	// was already taken care of earlier, so let's not waste time here.
	if(this->isChemotactic->getInteger()==1) {
		TarTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTar->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTar->getDouble())) ,TarCount->getInteger());
		TsrTerm=pow( (1.0+(ligands->at(ligIndex)/KoffTsr->getDouble())) / (1.0+(ligands->at(ligIndex)/KonTsr->getDouble())) ,TsrCount->getInteger());
	} else { TarTerm = 1; TsrTerm = 1; }

	// If we are using the simple OU model, then we have to update m0 (mean methylation level)
	if (!this->is_wills_methylation_model->getBool()) {
		m0 = (log((alpha->getDouble()/meanCheYp->getDouble())-1) -e0->getDouble() -log(TarTerm) -log(TsrTerm))/e1->getDouble();
	}

	// calculate the next cheY-p value for each trace needed (multiple traces are needed to simulate uncoordinated motors)
	for(int trace=0; trace<tracesNeeded; trace++) {
		// first the change in methylation state
		if (this->is_wills_methylation_model->getBool()) {
			// first the deterministic change
			Mdot = this->kr->getDouble()*(1-act[trace])/(this->Kr->getDouble()+ 1.0 - act[trace]);
			Mdot -= this->kb->getDouble()*act[trace]*act[trace]*act[trace]/(this->Kb->getDouble()+act[trace]);
			// next the noise term
			double dMn = this->noise_parameter->getDouble()*Util::RANDOM_GAUSSIAN()*sqrtOfDt;
			methLevel->at(trace) = methLevel->at(trace) + Mdot*dt + dMn;
			//// make sure that methylation stays within bounds
			if(methLevel->at(trace)>4) { methLevel->at(trace)=4; }
			if(methLevel->at(trace)<0) { methLevel->at(trace)=0; }
		} else {
			Mdot = -(methLevel->at(trace)-m0) / tauAdapt->getDouble();
			methLevel->at(trace) = methLevel->at(trace) + Mdot*dt + sigM*sqrt(2/tauNoise->getDouble()) * Util::RANDOM_GAUSSIAN()*sqrtOfDt;
			if(methLevel->at(trace)>8) { methLevel->at(trace)=8; }
			if(methLevel->at(trace)<0) { methLevel->at(trace)=0; }
		}

		// regardless of the model, we can now update the activity
		act[trace] = 1.0/(1.0+(exp(e0->getDouble()+e1->getDouble()*methLevel->at(trace))*TarTerm*TsrTerm));


		// finally, the updated CheYp value
		if (this->is_wills_methylation_model->getBool()) {

			/// -----------------------------------------------------------------------------------------
			/// With Will's model, instead of a linear relationship between activity and cheyp concentration,
			/// we now assume a hyperbolic relationship.
			/// ------------------------------------------------------------------------------------------
			this->CheYp->at(trace) = y0->getDouble()*act[trace]/(a0->getDouble()+act[trace]);
			if(this->CheYp->at(trace)<0) { this->CheYp->at(trace)=0; }
			// get some data items for output (note that this is somewhat ineffecient because of the database lookup) someone please fix it!
			((DoubleData*)this->agent->getDatabase()->getDataItem("aktivitaet"))->setDouble(act[trace]);
			((DoubleData*)this->agent->getDatabase()->getDataItem("cheyphos"))->setDouble(CheYp->at(trace));
			((DoubleData*)this->agent->getDatabase()->getDataItem("methyl"))->setDouble(methLevel->at(trace));
		} else {
			// with the simple model, we update the cheyp activity in a simple linear way with scaling parameter alpha
			this->CheYp->at(trace)=alpha->getDouble() * act[trace];

			//THIS OFFSET BLOCK OF CODE ALLOWS ADDITION OF NOISE DIRECTLY TO CHEYP
			//			//first, determine what the Yp value should be without noise
			//			double currentMeanYp = alpha->getDouble() * act[trace];
			//			//now, determine the dYp (based on the noise
			//			double dYp = -(1.0/this->tauNoise->getDouble())*(lastCheYpForNoise.at(trace))*dt+(this->sqrtOfD*Util::RANDOM_GAUSSIAN()*sqrtOfDt);
			//			lastCheYpForNoise.at(trace) = lastCheYpForNoise.at(trace)+dYp;
			//			//and we update CheYp with the noise amount
			//			this->CheYp->at(trace) = currentMeanYp+lastCheYpForNoise.at(trace);
		}

		// make sure we don't go out of bounds
		if(this->CheYp->at(trace)<0) { this->CheYp->at(trace)=0; }
	}

	internalTime += t;

	// here we finally make some updates to track the snapshot output (what happens immediately before/after tumbles
	if(collectHistory) {
		if(internalTime>eqtime->getDouble()) {
			hcCheYP->makeHistory(this->CheYp->at(0));
			hcLig->makeHistory(this->ligands->at(0));
			hcAct->makeHistory(this->act[0]);
			hcMeth->makeHistory(this->methLevel->at(0));
			hcdlogLig->makeHistory((this->ligands->at(0)-lastLig) / this->ligands->at(0));
		}
	}

	dLig->setDouble(this->ligands->at(0)-lastLig);
	dCheYP->setDouble(this->CheYp->at(0)-lastCheYp);
	dActivity->setDouble(this->act[0]-lastActivity);
	dMeth->setDouble(this->methLevel->at(0)-lastMeth);
	lastLig=this->ligands->at(0);
	lastCheYp=this->CheYp->at(0);
	lastActivity=this->act[0];
	lastMeth=this->methLevel->at(0);

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




