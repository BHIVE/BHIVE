/**
 * SC_agentFactories.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#include "SC_agentFactories.hh"
#include "../simpleCell.hh"
#include "../simulators/SC_simulators.hh"
#include "../../../exception/exception.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../simulators/cellMovement/chemotaxisMovement/chemotaxisMovement.hh"

using namespace std;
using namespace Hive;
using namespace SimpleCell;


SimpleCellFactory::SimpleCellFactory(
		const int signalType,
		const int motorType,
		const int motionType,
		double eqTime):AgentFactory()
{
	this->signalingSimulatorType = signalType;
	this->motorSimulatorType = motorType;
	this->motionSimulatorType = motionType;
	this->eqTime=eqTime;
}

SimpleCellFactory::SimpleCellFactory(InputSystemReader *isr):AgentFactory(isr) {}
SimpleCellFactory::~SimpleCellFactory() {}

Agent *SimpleCellFactory::createAgent(double meanCheYp, double tau, double noise, int nMotors, bool areMotorsCorrelated)
{
	double speed = 20;
	double rotDiffConst = 0.062;
	double movementDtvalue = 0.01;

	double startX = 0; double startY = 0; double startZ = 0;


	Agent *a = new Agent();
	Database *db = a->getDatabase();

	// Set the swimming characteristics of the cell
	Data *speedData = new Hive::DoubleData("CellSpeed",speed);
	Data *rotDiffConstData = new Hive::DoubleData("RotDiffConst",rotDiffConst);
	Data *movementDt = new Hive::DoubleData("MovementDt",movementDtvalue);

	// Set the position of this cell
	DoubleVectorData *position = new Hive::DoubleVectorData("Position",4);
	position->setDouble(a->getAgentId(),0);
	position->setDouble(startX,1);
	position->setDouble(startY,2);
	position->setDouble(startZ,3);
	DoubleVectorData *dir = new Hive::DoubleVectorData("Direction",4);
	dir->setDouble(a->getAgentId(),0);
	dir->setDouble(startX,1);
	dir->setDouble(startY,2);
	dir->setDouble(startZ,3);
	DoubleVectorData *lastPos = new Hive::DoubleVectorData("LastPosition",4);
	lastPos->setDouble(a->getAgentId(),0);
	lastPos->setDouble(startX,1);
	lastPos->setDouble(startY,2);
	lastPos->setDouble(startZ,3);
	DoubleVectorData *lastDir = new Hive::DoubleVectorData("LastDirection",4);
	lastDir->setDouble(a->getAgentId(),0);
	lastDir->setDouble(startX,1);
	lastDir->setDouble(startY,2);
	lastDir->setDouble(startZ,3);


	// Set the signaling information for the cell
	DoubleVectorData *chemical = new Hive::DoubleVectorData("Chemical",1);
	chemical->setDouble(0.0,0);

	// Set CheYp values for the cell (or multiple if we need multiple traces per cell)
	DoubleVectorData *ypConc;
	if(!areMotorsCorrelated) {
		ypConc = new Hive::DoubleVectorData("CheYp",nMotors);
		for (int i=0; i<nMotors; i++) {
			ypConc->setDouble(meanCheYp,i);
		}
	} else {
		ypConc = new Hive::DoubleVectorData("CheYp",1);
		ypConc->setDouble(meanCheYp,0);
	}



	IntVectorData *motorState = new Hive::IntVectorData("MotorStates",nMotors);
	for (int m=0; m<nMotors; m++) {
		motorState->setInt(SimpleCellConstants::CW,m);
	}


	Data *currentCellState = new Hive::IntegerData("CurrentCellState",SimpleCellConstants::TUMBLE);
	Data *lastCellState = new Hive::IntegerData("LastCellState",SimpleCellConstants::TUMBLE);



	// Add all the information to the database
	db->addData(speedData->getName(),speedData);
	db->addData(rotDiffConstData->getName(),rotDiffConstData);
	db->addData(movementDt->getName(),movementDt);

	db->addData(position->getName(),position);
	db->addData(dir->getName(),dir);
	db->addData(lastPos->getName(),lastPos);
	db->addData(lastDir->getName(),lastDir);
	db->addData(chemical->getName(),chemical);
	db->addData(ypConc->getName(),ypConc);
	db->addData(motorState->getName(),motorState);
	db->addData(currentCellState->getName(),currentCellState);
	db->addData(lastCellState->getName(),lastCellState);



	addSimulatorToAgent(a,meanCheYp, tau, noise, nMotors,areMotorsCorrelated, eqTime);



	//Add action to update ligand concentration from the outside
	SimpleCell::UpdateLigandConcentration * ulc = new UpdateLigandConcentration(0);
	a->addAction(ulc);
	ulc->setAgent(a);





	return a;
}

Agent *SimpleCellFactory::createAgent(Action **ac)
{
	return 0;
}

Agent *SimpleCellFactory::duplicateAgent(Agent *ag)
{
	return 0;
}

void SimpleCellFactory::finalise()
{

}

void SimpleCellFactory::addSimulatorToAgent(Agent *ag,
		double meanCheYp, double tau, double noise, int nMotors, bool areMotorsCorrelated, double eqTime)
{
	// //////////////////////////////////////////////
	// Set the Signaling Network Simulator
    // //////////////////////////////////////////////
	if(signalingSimulatorType==SimpleCellConstants::NOISE_SIGNALING) {

		int nTracesNeeded = 1;
		if(!areMotorsCorrelated) nTracesNeeded = nMotors;

		OU_NoiseSimulator *sim = new OU_NoiseSimulator(tau,noise,meanCheYp,nTracesNeeded);
		sim->setAgent(ag);
		ag->addSimulator(sim);

	} else if (signalingSimulatorType==SimpleCellConstants::SIMPLE_WINGREEN_SIGNALING) {
		int nTracesNeeded = 1;
		if(!areMotorsCorrelated) nTracesNeeded = nMotors;

		SimpleSignalingSimulator *sim = new SimpleSignalingSimulator(tau,noise,meanCheYp,nTracesNeeded);
		sim->setAgent(ag);
		ag->addSimulator(sim);

	} else {
		throw(HiveException("Unknown signaling network type set for this factory.",
				"SimpleCell::SimpleCellFactory::addSimulatorToAgent(Agent *ag)"));
	}


	if(motorSimulatorType==SimpleCellConstants::SINGLE_MOTOR) {

		double Kd = 3.06;
		double g1 = 40;
		double w = 1.3;

		Simulator *sim = new MotorSimulator(nMotors,Kd,g1,w);
		sim->setAgent(ag);
		ag->addSimulator(sim);

	} else if (motorSimulatorType==SimpleCellConstants::VOTING_MOTORS) {

	} else {
		throw(HiveException("Unknown motor simulator type set for this factory.",
				"SimpleCell::SimpleCellFactory::addSimulatorToAgent(Agent *ag)"));
	}




	if(motionSimulatorType==SimpleCellConstants::STANDARD_SWIMMING)
	{
		double waitingTimeInSemi = 0.2;
		int minMotorsInCCWtoSwim = 2;
		Simulator *sim = new SimpleCell::ConformationModelSimulator(waitingTimeInSemi, minMotorsInCCWtoSwim);
		sim->setAgent(ag);
		ag->addSimulator(sim);

		Simulator *simMovement = new Chemotaxis::ChemotaxisMovementSimulator(eqTime);
		simMovement->setAgent(ag);
		ag->addSimulator(simMovement);

//	} else if (motorSimulatorType==SimpleCellConstants::VOTING_MOTORS) {
//
	} else {
		throw(HiveException("Unknown motor simulator type set for this factory.",
				"SimpleCell::SimpleCellFactory::addSimulatorToAgent(Agent *ag)"));
	}



	// mM per um, in AgentCell, used 10^-8 M / um => 10^-5 mM / um
	// lets try something twice as steep
//	double slope = 2.5*10-5;
//	double intercept = 0.01;
//
//	Simulator *linearEnv = new SimpleCell::SimpleLinearEnv(slope,intercept);
//	linearEnv->setAgent(ag);
//	ag->addSimulator(linearEnv);


}



EnvironmentAgentFactory::EnvironmentAgentFactory(const int type):AgentFactory()
{
	simulatorType = type;
//	if(type==SimpleCellConstants::CONSTANT_ENVIRONMENT) {
//
//	} else if (type==SimpleCellConstants::CAPILLARY_ENVIRONMENT) {
//
//	} else {
//		throw(HiveException("Unknown environment type given.",
//				"SimpleCell::EnvironmentAgentFactory::EnvironmentAgentFactory(const int type)"));
//	}
}

EnvironmentAgentFactory::EnvironmentAgentFactory(InputSystemReader *isr):AgentFactory(isr) {}
EnvironmentAgentFactory::~EnvironmentAgentFactory() {}

Agent *EnvironmentAgentFactory::createAgent()
{
	Agent *a = new Agent();
	this->addSimulatorToAgent(a);
	return a;
}

Agent *EnvironmentAgentFactory::createAgent(Action **ac)
{
	return 0;
}

Agent *EnvironmentAgentFactory::duplicateAgent(Agent *ag)
{
	return 0;
}

void EnvironmentAgentFactory::finalise()
{

}
