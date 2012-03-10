/*
 * ecoliSimulator.cpp
 *
 *  Created on: Jun 6, 2010
 *      Author: msneddon
 */


#include "bactCell.hh"



using namespace std;
using namespace Hive;
using namespace SimpleCell;

const int Ecoli_Simulator::X = 0;
const int Ecoli_Simulator::Y = 1;
const int Ecoli_Simulator::Z = 2;


Ecoli_Simulator::Ecoli_Simulator()
{
	double DefaultGammaAlpha = 4.;
	double DefaultGammaBeta = 1./18.32045567939674;
	double DefaultGammaOffset = -4.606176605609249;
	gs = new Util::GammaSampler(DefaultGammaAlpha,DefaultGammaBeta,DefaultGammaOffset);
}

Ecoli_Simulator::~Ecoli_Simulator()
{
	delete gs;

}

void Ecoli_Simulator::step(Agent* inAgent,double t)
{

	BacteriaDatabase* db = (BacteriaDatabase*)inAgent->getDatabase()->getDataItem(BACTERIADATABASE);

	int ligIndex = 0;

	//get the timestep
	this->dt = t; //seconds
	this->sqrtOfDt = pow(this->dt,0.5);

	//This simulator is always running, so update the time here
	db->celltime += dt;

	//////////////////  SIGNALING AND SENSING

	//std::numeric_limits<double>::has_quiet_NaN

	// make sure ligands is greater than zero, set up such that nan
	if(db->ligands.at(ligIndex)>=0){} else { db->ligands.at(ligIndex)=0; }

	// calculate the activity from the ligand and resting methylation state
	TarTerm=pow( (1.0+(db->ligands.at(ligIndex)/db->KoffTar)) / (1.0+(db->ligands.at(ligIndex)/db->KonTar)) ,db->TarCount);
	TsrTerm=pow( (1.0+(db->ligands.at(ligIndex)/db->KoffTsr)) / (1.0+(db->ligands.at(ligIndex)/db->KonTsr)) ,db->TsrCount);
	m0 = (log((db->alpha/db->meanCheYp)-1) -db->e0 -log(TarTerm) -log(TsrTerm))/db->e1;



	//based on sigma, set sigM
	db->sigM = 1/db->e1 *(db->alpha/((1-(db->alpha/db->meanCheYp))
					*db->meanCheYp*db->meanCheYp ))*(db->sigma*db->meanCheYp);



	// calculate the next cheY-p value for each trace needed
	for(int trace=0; trace<db->tracesNeeded; trace++) {

		// first the change in methylation state
		Mdot = -(db->methLevel.at(trace)-m0) / db->tau;
		db->methLevel.at(trace) = db->methLevel.at(trace) + Mdot*dt + db->sigM*sqrt(2/db->tau) * Util::RANDOM_GAUSSIAN()*sqrtOfDt;

		if(db->methLevel.at(trace)>8) db->methLevel.at(trace)=8;
		if(db->methLevel.at(trace)<0) db->methLevel.at(trace)=0;

		// then the new activity
		db->act.at(trace) = 1.0/(1.0+(exp(db->e0+db->e1*db->methLevel.at(trace))*TarTerm*TsrTerm));

		// finally, the updated CheYp value
		db->CheYp.at(trace)=db->alpha * db->act.at(trace);
	}





	/////////  MOTOR SIMULATION
	//variables we need
	double deltaG = 0;
	double kPlus = 0;
	double kMinus = 0;

	// Depending on whether or not the motor traces are correlated, we have to simulate
	// slightly differently (mostly for performance gains, we have the top level if statement
	if(db->areMotorsCorrelated) {

		//get the CheYp value
		deltaG = (db->motor_g1/2.0)*((1.0/2.0)-(db->CheYp.at(0)/(db->motor_Kd+db->CheYp.at(0))));
		kPlus = db->motor_w*exp(deltaG);
		kMinus = db->motor_w*exp(-deltaG);


		//cout<<internalTime<<" corr motors:";
		for(int m=0; m<db->n_motors; m++) {

			// let's do one step per iteration from now on
			//for (double currentTime=0; currentTime<t; currentTime+=dt)
			//{
				double randNum = Util::RANDOM_CLOSED();
				if(db->MotorStates.at(m)==db->CW) {
					if(randNum<(kPlus*dt)) {
						db->MotorStates.at(m) = db->CCW;
					} else {
						db->MotorStates.at(m) = db->CW;
					}

					} else /*if(motorStates[m]==CCW) */ {
						if(randNum>=(1-(kMinus*dt))) {
							db->MotorStates.at(m) = db->CW;
						} else {
							db->MotorStates.at(m) = db->CCW;
						}
					}
				//}
				//cout<<" "<<motorStates[m];
			}
			//cout<<"\n";
		} else {


			//cout<<internalTime<<" uncorr motors:";
			for(int m=0; m<db->n_motors; m++) {
				deltaG = (db->motor_g1/2.0)*((1.0/2.0)-(db->CheYp.at(m)/(db->motor_Kd+db->CheYp.at(m))));
				kPlus = db->motor_w*exp(deltaG);
				kMinus = db->motor_w*exp(-deltaG);

				//for (double currentTime=0; currentTime<t; currentTime+=dt)
				//{
					double randNum = Util::RANDOM_CLOSED();
					if(db->MotorStates.at(m)==db->CW) {
						if(randNum<(kPlus*dt)) {
							db->MotorStates.at(m) = db->CCW;
						} else {
							db->MotorStates.at(m) = db->CW;
						}
					} else /*if(motorStates[m]==CCW) */ {
						if(randNum>=(1-(kMinus*dt))) {
							db->MotorStates.at(m) = db->CW;
						} else {
							db->MotorStates.at(m) = db->CCW;
						}
					}
				//}
				//cout<<" "<<motorStates[m];
			}
			//cout<<"\n";
		}



	// FLAGELLA SIMULATOR
	//  note: this is where the multiple flagella model would be inserted.  For now
	//  we use just use the simple voting model

	//count motors in CCW (these are swimming motors)
	int ccwCount = 0;
	for (int k=0; k < int(db->MotorStates.size()); k++)
		if(db->MotorStates.at(k)==db->CCW) ccwCount++;

	//set swimming state accordingly, with a threshold of just one motor for now
	if(ccwCount>0)
		db->swimming_state = db->RUN;
	else
		db->swimming_state = db->TUMBLE;




	// Movement Simulator: accounts for rotational diffusion and for
	// running vs tumbling behavior by setting the direction value.
	if(get_propagator()->getCurrentTime() >= db->eqtime) {

		if(db->swimming_state==db->RUN)
		{
			swimToNewPosition(db,dt);
			db->last_swimming_state=db->RUN;
		}
		else if(db->swimming_state==db->TUMBLE
					&& db->last_swimming_state==db->RUN)
		{
			//This lets us change directions based on the gamma distribution
			this->changeDirDistribution(db);

			//This lets us change directions completely randomly
			//this->changeDirRandom(db);

			//Update our last position, because we did not move
			db->last_swimming_state=db->TUMBLE;
		}
	}



	// Finally, we have to set the original direction vector so that it
	// can be read by the world.  We assume "direction" is actually the "displacement" vector
	vector<double> direction(3); // = db->getDirection();
	direction.at(Ecoli_Simulator::X) = db->pos[Ecoli_Simulator::X]-db->lastPos[Ecoli_Simulator::X];
	direction.at(Ecoli_Simulator::Y) = db->pos[Ecoli_Simulator::Y]-db->lastPos[Ecoli_Simulator::Y];
	direction.at(Ecoli_Simulator::Z) = db->pos[Ecoli_Simulator::Z]-db->lastPos[Ecoli_Simulator::Z];
	db->setDirection(direction);


	// get back results by using this line to output to the console with grep on xxxx.
    // agentID.agentNode  ligand  activity(0)  cheYp(0)  motor(0)  x  y  z
	// cout<<"xxxx "<<inAgent->getAgentId()<<"."<<inAgent->getAgentNode()<<" "<<db->celltime<<" "<<db->ligands.at(0)<<" "<<db->act.at(0)<<" "<<db->CheYp.at(0)<<" "<<db->MotorStates.at(0)<<" "<<db->pos[0]<<" "<<db->pos[1]<<" "<<db->pos[2]<<endl;



	internalTime += t;

}


void Ecoli_Simulator::initialise() {}







void Ecoli_Simulator::swimToNewPosition(BacteriaDatabase* db, double elapsedTime)
{
	moveToNewPosition(db,elapsedTime);
	//env->tryToMove(this->lastPos,this->pos,this->dir,this->up);
	changeDirRotDiffusion(db,elapsedTime);
}

void Ecoli_Simulator::moveToNewPosition(BacteriaDatabase* db, double elapsedTime)
{
	double distanceTraveled = db->CellSpeed*elapsedTime;
	db->lastPos[X]=db->pos[X]; db->lastPos[Y]=db->pos[Y]; db->lastPos[Z]=db->pos[Z];
	Util::applyDisplacement(db->pos,db->dir,distanceTraveled);
}




void Ecoli_Simulator::changeDirRandom(BacteriaDatabase* db)
{
	db->lastDir[X]=db->dir[X]; db->lastDir[Y]=db->dir[Y]; db->lastDir[Z]=db->dir[Z];
	db->lastUp[X]=db->up[X]; db->lastUp[Y]=db->up[Y]; db->lastUp[Z]=db->up[Z];

	//for now, each cell gets its own rotation matrix, but this is not strictly required
	//it will always be overwritten by the simulator to
	Util::genUniformRandRotation3d(db->rotMat);
	Util::applyRotation(db->rotMat,db->dir);
	Util::applyRotation(db->rotMat,db->up);
}
void Ecoli_Simulator::changeDirDistribution(BacteriaDatabase* db)
{
	db->lastDir[X]=db->dir[X]; db->lastDir[Y]=db->dir[Y]; db->lastDir[Z]=db->dir[Z];
	db->lastUp[X]=db->up[X]; db->lastUp[Y]=db->up[Y]; db->lastUp[Z]=db->up[Z];

	//First, rotate the up vector randomly between 0 and 2pi along the direction axis
	//This will not change the direction
	double angleInRad=Util::RANDOM_CLOSED()*2.*Util::PI;
	Util::genRot3dAboutAxis(db->rotMat,db->dir,angleInRad);
	Util::applyRotation(db->rotMat,db->up);

	//Second, rotate the direction by the given degree by the set gamma distribution
	angleInRad = gs->gen(0.,180.)*Util::PI/180.;
	Util::genRot3dAboutAxis(db->rotMat,db->up,angleInRad);
	Util::applyRotation(db->rotMat,db->dir);
}

void Ecoli_Simulator::changeDirRotDiffusion(BacteriaDatabase* db,double elapsedTime)
{
	db->lastDir[X]=db->dir[X]; db->lastDir[Y]=db->dir[Y]; db->lastDir[Z]=db->dir[Z];
	db->lastUp[X]=db->up[X]; db->lastUp[Y]=db->up[Y]; db->lastUp[Z]=db->up[Z];

	double rotDifTerm = sqrt(elapsedTime * 2. * db->RotDiffConst);
	double angleX = rotDifTerm * Util::RANDOM_GAUSSIAN();
	double angleY = rotDifTerm * Util::RANDOM_GAUSSIAN();
	double angleZ = rotDifTerm * Util::RANDOM_GAUSSIAN();

	Util::genRotFromAngles(db->rotMat,angleX,angleY,angleZ);
	Util::applyRotation(db->rotMat,db->dir);
	Util::applyRotation(db->rotMat,db->up);
}



