/*
 * chemotaxisMovement.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: msneddon
 */


#include "chemotaxisMovement.hh"
#include "../../../exception/exception.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"

#include <cmath>
#include "../../../projects/simpleCell/simpleCell.hh"

using namespace Hive;
using namespace Chemotaxis;

const int ChemotaxisMovementSimulator::X = 0;
const int ChemotaxisMovementSimulator::Y = 1;
const int ChemotaxisMovementSimulator::Z = 2;

const double ChemotaxisMovementSimulator::DefaultGammaAlpha = 4.;
const double ChemotaxisMovementSimulator::DefaultGammaBeta = 1./18.32045567939674;
const double ChemotaxisMovementSimulator::DefaultGammaOffset = -4.606176605609249;


ChemotaxisMovementSimulator::ChemotaxisMovementSimulator(double eqTime)
{
	this->agent=NULL;
	this->eqTime = eqTime;
	this->internalTime = 0;
}
ChemotaxisMovementSimulator::~ChemotaxisMovementSimulator() {}

void ChemotaxisMovementSimulator::setAgent(Agent *ag)
{
	this->agent=ag;
	try {
		initialise();
	} catch (HiveException& e) {
		e.addTraceException("ChemotaxisMovementSimulator::addAgent(Agent *a)");
		throw e;
	}
}

void ChemotaxisMovementSimulator::initialise()
{

	if(agent==NULL)
		throw HiveException("Cannot initialize ChemotaxisMovementSimulator if no agent was added.",
				"ChemotaxisMovementSimulator::initialise()");

	// Check the database for the presence of the basic objects, and extract that info
	Database *db = agent->getDatabase();
	if(db->existsDataItem("CellSpeed")) {
		DoubleData *dd=(DoubleData *)db->getDataItem("CellSpeed");
		speed = dd->getDouble();
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs CellSpeed data.",
						"ChemotaxisMovementSimulator::initialise()");
	}
	if(db->existsDataItem("RotDiffConst")) {
		DoubleData *dd=(DoubleData *)db->getDataItem("RotDiffConst");
		rotDiffConst = dd->getDouble();
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs RotationalDiffusion data.",
							"ChemotaxisMovementSimulator::initialise()");
	}
	if(db->existsDataItem("Position")) {
		DoubleVectorData *dvd=(DoubleVectorData *)db->getDataItem("Position");
		pos[X]=dvd->getDouble(X+1);
		pos[Y]=dvd->getDouble(Y+1);
		pos[Z]=dvd->getDouble(Z+1);
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs RotationalDiffusion data.",
							"ChemotaxisMovementSimulator::initialise()");
	}
	if(db->existsDataItem("MovementDt")) {
		DoubleData *dd=(DoubleData *)db->getDataItem("MovementDt");
		movementDt = dd->getDouble();
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs MovementDt data.",
						"ChemotaxisMovementSimulator::initialise()");
	}
	if(db->existsDataItem("CurrentCellState")) {
		IntegerData *id=(IntegerData *)db->getDataItem("CurrentCellState");
		currentMovementState = id->getInteger();
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs CurrentCellState data.",
						"ChemotaxisMovementSimulator::initialise()");
	}
	if(db->existsDataItem("LastCellState")) {
		IntegerData *id=(IntegerData *)db->getDataItem("LastCellState");
		lastMovementState = id->getInteger();
	} else {
		throw HiveException("ChemotaxisMovementSimulator needs LastCellState data.",
						"ChemotaxisMovementSimulator::initialise()");
	}



	//Now that we have the basic data, we can set up the simulator's internal
	//information here
	gs = new Util::GammaSampler(DefaultGammaAlpha,DefaultGammaBeta,DefaultGammaOffset);

	//starting direction
	dir[X]=1; dir[Y]=0; dir[Z]=0;
	up[X]=0; up[Y]=1; up[Z]=0;

	//Init the rotation matrix
	rotMat[0][0]=1;   rotMat[0][1]=0;   rotMat[0][2]=0;
	rotMat[1][0]=0;   rotMat[1][1]=1;   rotMat[1][2]=0;
	rotMat[2][0]=0;   rotMat[2][1]=0;   rotMat[2][2]=1;


	//orient ourselves randomly
	changeDirRandom();





// OLD VERSION FOR ORIGINAL JAM FEST CODE
//	if(agent==NULL)
//		throw HiveException("Cannot initialize ChemotaxisMovementSimulator if no agent was added.",
//				"ChemotaxisMovementSimulator::initialise()");
//
//	// Check the database for the presence of the basic objects, and extract that info
//	Database *db = agent->getDatabase();
//	if(db->existsDataItem("CellSpeed")) {
//		DoubleData *dd=(DoubleData *)db->getDataItem("CellSpeed");
//		speed = dd->getDouble();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs CellSpeed data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("RotDiffConst")) {
//		DoubleData *dd=(DoubleData *)db->getDataItem("RotDiffConst");
//		rotDiffConst = dd->getDouble();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs RotationalDiffusion data.",
//							"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("Position")) {
//		DoubleVectorData *dvd=(DoubleVectorData *)db->getDataItem("Position");
//		pos[X]=dvd->getDouble(X+1);
//		pos[Y]=dvd->getDouble(Y+1);
//		pos[Z]=dvd->getDouble(Z+1);
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs RotationalDiffusion data.",
//							"ChemotaxisMovementSimulator::initialise()");
//	}
////	if(db->existsDataItem("xPos")) {
////		DoubleData *dd=(DoubleData *)db->getDataItem("xPos");
////		xPos = dd->getDouble();
////	} else {
////		throw HiveException("ChemotaxisMovementSimulator needs xPos data.",
////							"ChemotaxisMovementSimulator::initialise()");
////	}
////	if(db->existsDataItem("yPos")) {
////		DoubleData *dd=(DoubleData *)db->getDataItem("yPos");
////		yPos = dd->getDouble();
////	} else {
////		throw HiveException("ChemotaxisMovementSimulator needs yPos data.",
////						"ChemotaxisMovementSimulator::initialise()");
////	}
////	if(db->existsDataItem("zPos")) {
////		DoubleData *dd=(DoubleData *)db->getDataItem("zPos");
////		zPos = dd->getDouble();
////	} else {
////		throw HiveException("ChemotaxisMovementSimulator needs zPos data.",
////						"ChemotaxisMovementSimulator::initialise()");
////	}
//	if(db->existsDataItem("MovementDt")) {
//		DoubleData *dd=(DoubleData *)db->getDataItem("MovementDt");
//		movementDt = dd->getDouble();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs MovementDt data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("CurrentMovementState")) {
//		IntegerData *id=(IntegerData *)db->getDataItem("CurrentMovementState");
//		currentMovementState = id->getInteger();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs CurrentMovementState data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("LastMovementState")) {
//		IntegerData *id=(IntegerData *)db->getDataItem("LastMovementState");
//		lastMovementState = id->getInteger();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs LastMovementState data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//
//
//
//	if(db->existsDataItem("NumberCW_Motors")) {
//		IntegerData *id=(IntegerData *)db->getDataItem("NumberCW_Motors");
//		numMotorsCW = id->getInteger();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs NumberCW_Motors data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("NumberCCW_Motors")) {
//		IntegerData *id=(IntegerData *)db->getDataItem("NumberCCW_Motors");
//		numMotorsCCW = id->getInteger();
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs NumberCCW_Motors data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//	if(db->existsDataItem("Chemical")) {
//		DoubleVectorData *id=(DoubleVectorData *)db->getDataItem("Chemical");
//		double chemicalOne = id->getDouble(0);
//		double chemicalTwo = id->getDouble(1);
//	} else {
//		throw HiveException("ChemotaxisMovementSimulator needs chemical data.",
//						"ChemotaxisMovementSimulator::initialise()");
//	}
//
//	//Now that we have the basic data, we can set up the simulator's internal
//	//information here
//	gs = new Util::GammaSampler(DefaultGammaAlpha,DefaultGammaBeta,DefaultGammaOffset);
//
//	//starting direction
//	dir[X]=1; dir[Y]=0; dir[Z]=0;
//	up[X]=0; up[Y]=1; up[Z]=0;
//
//	//Init the rotation matrix
//	rotMat[0][0]=1;   rotMat[0][1]=0;   rotMat[0][2]=0;
//	rotMat[1][0]=0;   rotMat[1][1]=1;   rotMat[1][2]=0;
//	rotMat[2][0]=0;   rotMat[2][1]=0;   rotMat[2][2]=1;
//
//
//	//orient ourselves randomly
//	changeDirRandom();
}


void ChemotaxisMovementSimulator::step(double t)
{
	//cout<<"before:"<<endl;
	//cout<<pos[X]<<" "<<pos[Y]<<" "<<pos[Z]<<endl;

	this->currentMovementState=((IntegerData *)agent->getDatabase()->getDataItem("CurrentCellState"))->getInteger();



	///////// Move the cell for the elapsed time, only if we are simulating and not equilibrating
	///////// We should start out in a random direction anyway, so we don't have to do anything here.


	if(this->internalTime>=this->eqTime) {

		for(double currentTime=0; currentTime<t; currentTime+=movementDt)
		{
			if(currentMovementState==SimpleCell::SimpleCellConstants::RUN)
			{
				swimToNewPosition(movementDt);
				lastMovementState=SimpleCell::SimpleCellConstants::RUN;
			}
			else if(currentMovementState==SimpleCell::SimpleCellConstants::TUMBLE
					&& lastMovementState==SimpleCell::SimpleCellConstants::RUN)
			{
				//This lets us change directions based on the gamma distribution
				this->changeDirDistribution();

				//This lets us change directions completely randomly
				//this->changeDirRandom();

				//Update our last position, because we did not move
				lastMovementState=SimpleCell::SimpleCellConstants::TUMBLE;
			}
		}
	}

	//update the database with our new position
	//Database *db = agent->getDatabase();
	//((DoubleData *)db->getDataItem("xPos"))->setDouble(xPos);
	//((DoubleData *)db->getDataItem("yPos"))->setDouble(yPos);
	//((DoubleData *)db->getDataItem("zPos"))->setDouble(zPos);
	DoubleVectorData *dvd=(DoubleVectorData *)(agent->getDatabase()->getDataItem("Position"));
	dvd->setDouble(pos[X],X+1); dvd->setDouble(pos[Y],Y+1); dvd->setDouble(pos[Z],Z+1);


	// update internal time;
	this->internalTime += t;









// OLD VERSION FOR ORIGINAL JAM FEST CODE
//	cout<<"before:"<<endl;
//		cout<<pos[X]<<" "<<pos[Y]<<" "<<pos[Z]<<endl;
//
//	//cout<<"   -agent: "<<agent->getAgentId()<<" started at position: "<<xPos<<" "<<yPos<<" "<<zPos<<endl;
//	// decide if we run or tumble
//
//	int limit = 1; //limit of the voting model
//
//	//voting model...
//	if(numMotorsCW>=limit) {
//		this->currentMovementState = TUMBLE;
//		cout<<this->agent->getAgentId()<<" tumble"<<endl;
//	} else {
//		this->currentMovementState = RUN;
//		cout<<this->agent->getAgentId()<<" run"<<endl;
//	}
//
//	///////// Move the cell for the elapsed time
//	for(double currentTime=0; currentTime<t; currentTime+=movementDt)
//	{
//		if(currentMovementState==RUN)
//		{
//			swimToNewPosition(movementDt);
//			lastMovementState=RUN;
//		}
//		else if(currentMovementState==TUMBLE && lastMovementState==RUN)
//		{
//			//This lets us change directions based on the gamma distribution
//			this->changeDirDistribution();
//
//			//This lets us change directions completely randomly
//			//this->changeDirRandom();
//
//			//Update our last position, because we did not move
//			lastMovementState=TUMBLE;
//		}
//	}
//
//	//update the database
//	Database *db = agent->getDatabase();
//	//((DoubleData *)db->getDataItem("xPos"))->setDouble(xPos);
//	//((DoubleData *)db->getDataItem("yPos"))->setDouble(yPos);
//	//((DoubleData *)db->getDataItem("zPos"))->setDouble(zPos);
//	DoubleVectorData *dvd=(DoubleVectorData *)db->getDataItem("Position");
//	dvd->setDouble(pos[X],X+1); dvd->setDouble(pos[Y],Y+1); dvd->setDouble(pos[Z],Z+1);
//
//	((IntegerData *)db->getDataItem("CurrentMovementState"))->setInteger(currentMovementState);
//	((IntegerData *)db->getDataItem("LastMovementState"))->setInteger(lastMovementState);
//	cout<<"after:"<<endl;
//		cout<<pos[X]<<" "<<pos[Y]<<" "<<pos[Z]<<endl;
}

void ChemotaxisMovementSimulator::swimToNewPosition(double elapsedTime)
{
	moveToNewPosition(elapsedTime);
	//env->tryToMove(this->lastPos,this->pos,this->dir,this->up);
	changeDirRotDiffusion(elapsedTime);
}

void ChemotaxisMovementSimulator::moveToNewPosition(double elapsedTime)
{
	double distanceTraveled = speed*elapsedTime;
	lastPos[X]=pos[X]; lastPos[Y]=pos[Y]; lastPos[Z]=pos[Z];
	Util::applyDisplacement(pos,dir,distanceTraveled);
}


void ChemotaxisMovementSimulator::prepare()
{


}


void ChemotaxisMovementSimulator::synchroniseWithDatabase()
{
//	try {
//		Database *db = agent->getDatabase();
//		speed =((DoubleData *)db->getDataItem("CellSpeed"))->getDouble();
//		rotDiffConst =((DoubleData *)db->getDataItem("RotDiffConst"))->getDouble();
//		DoubleVectorData *dvd=(DoubleVectorData *)db->getDataItem("Position");
//		pos[X]=dvd->getDouble(X+1);
//		pos[Y]=dvd->getDouble(Y+1);
//		pos[Z]=dvd->getDouble(Z+1);
//		//cout<<"sync"<<endl;
//		//cout<<" == "<<pos[X]<<" "<<pos[Y]<<" "<<pos[Z]<<endl;
//		//dvd->printContent(cout);
//		//xPos =((DoubleData *)db->getDataItem("xPos"))->getDouble();
//		//yPos =((DoubleData *)db->getDataItem("yPos"))->getDouble();
//		//zPos =((DoubleData *)db->getDataItem("zPos"))->getDouble();
//		movementDt =((DoubleData *)db->getDataItem("MovementDt"))->getDouble();
//		currentMovementState =((IntegerData *)db->getDataItem("CurrentCellState"))->getInteger();
//		lastMovementState =((IntegerData *)db->getDataItem("LastCellState"))->getInteger();
//
//
//	} catch (HiveException& e) {
//		e.addTraceException("ChemotaxisMovementSimulator::synchroniseWithDatabase()");
//		throw e;
//	}
}





void ChemotaxisMovementSimulator::changeDirRandom()
{
	lastDir[X]=dir[X]; lastDir[Y]=dir[Y]; lastDir[Z]=dir[Z];
	lastUp[X]=up[X]; lastUp[Y]=up[Y]; lastUp[Z]=up[Z];

	Util::genUniformRandRotation3d(rotMat);
	Util::applyRotation(rotMat,dir);
	Util::applyRotation(rotMat,up);
}
void ChemotaxisMovementSimulator::changeDirDistribution()
{
	lastDir[X]=dir[X]; lastDir[Y]=dir[Y]; lastDir[Z]=dir[Z];
	lastUp[X]=up[X]; lastUp[Y]=up[Y]; lastUp[Z]=up[Z];

	//First, rotate the up vector randomly between 0 and 2pi along the direction axis
	//This will not change the direction
	double angleInRad=Util::RANDOM_CLOSED()*2.*Util::PI;
	Util::genRot3dAboutAxis(rotMat,dir,angleInRad);
	Util::applyRotation(rotMat,up);

	//Second, rotate the direction by the given degree by the set gamma distribution
	angleInRad = gs->gen(0.,180.)*Util::PI/180.;
	Util::genRot3dAboutAxis(rotMat,up,angleInRad);
	Util::applyRotation(rotMat,dir);
}

void ChemotaxisMovementSimulator::changeDirRotDiffusion(double elapsedTime)
{
	lastDir[X]=dir[X]; lastDir[Y]=dir[Y]; lastDir[Z]=dir[Z];
	lastUp[X]=up[X]; lastUp[Y]=up[Y]; lastUp[Z]=up[Z];

	double rotDifTerm = sqrt(elapsedTime * 2. * this->rotDiffConst);
	double angleX = rotDifTerm * Util::RANDOM_GAUSSIAN();
	double angleY = rotDifTerm * Util::RANDOM_GAUSSIAN();
	double angleZ = rotDifTerm * Util::RANDOM_GAUSSIAN();

	Util::genRotFromAngles(rotMat,angleX,angleY,angleZ);
	Util::applyRotation(rotMat,dir);
	Util::applyRotation(rotMat,up);
}





