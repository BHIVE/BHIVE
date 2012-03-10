#include "chemotaxisMovementInWorld.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;

ChemotaxisMovementSimulatorInWorld::ChemotaxisMovementSimulatorInWorld() {
	this->eqTime = 0;

	// initialise the rotation Matrix
	rotMat[0][0] = 1;	rotMat[0][1] = 0;	rotMat[0][2] = 0;
	rotMat[1][0] = 0;	rotMat[1][1] = 1; 	rotMat[1][2] = 0;
	rotMat[2][0] = 0;	rotMat[2][1] = 0; 	rotMat[2][2] = 1;

	double DefaultGammaAlpha = 4.;
	double DefaultGammaBeta = 1./18.32045567939674;
	double DefaultGammaOffset = -4.606176605609249;
	gs = new Util::GammaSampler(DefaultGammaAlpha, DefaultGammaBeta, DefaultGammaOffset);


	this->eqTime        = 0;
	this->numberofcells = 0;
	this->rotdiffconst  = 0;
	this->movement_dt   = 0;
	this->cellspeeds           = 0;
	this->last_swimming_states = 0;
	this->swimming_states      = 0;
	this->cell_positions = 0;
	this->cell_dir_vecs  = 0;
	this->cell_up_vecs   = 0;
}

ChemotaxisMovementSimulatorInWorld::~ChemotaxisMovementSimulatorInWorld() {
	delete gs;
}

void ChemotaxisMovementSimulatorInWorld::setWorldAgent(Agent *ag) {
	this->agent        = ag;
	this->internalTime = 0;

	this->initialise();
}

void ChemotaxisMovementSimulatorInWorld::initialise() {


	// neglecting error checking ...
	Database *db = agent->getDatabase();

	// setting the equilibration time.
	this->eqTime        = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();


	// i hope i didn't screw up on the names of the data items ...
	this->numberofcells = (IntegerData*) db->getDataItem("numberofcells");
	this->rotdiffconst  = (DoubleData*)  db->getDataItem("rotational_diffusion_constant");
	this->movement_dt   = (DoubleData*)  db->getDataItem("dt");


	this->cellspeeds           = (TVectorData<double>*) db->getDataItem("cellspeeds");


	this->last_swimming_states = (TVectorData<int>* )   db->getDataItem("lastswimmingstates");
	this->swimming_states      = (TVectorData<int>* )   db->getDataItem("swimmingstates");

	this->cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	this->cell_dir_vecs  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("celldirvecs");
	this->cell_up_vecs   = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellupvecs");


	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		this->changeDirRandom(i);
	}

}

void ChemotaxisMovementSimulatorInWorld::prepare() {
	// i think there is nothing to do here.
}

void ChemotaxisMovementSimulatorInWorld::synchroniseWithDatabase() {
	// i don't think there is anything to do here.
}

void ChemotaxisMovementSimulatorInWorld::step(double t) {
	if (this->internalTime >= this->eqTime) {
		// iterate over all the cells.
		for (int i=0; i<this->numberofcells->getInteger(); i++) {
			if (this->swimming_states->at(i) == CellParameterData::RUN) {
				this->swimToNewPosition(i);
				this->last_swimming_states->at(i) = CellParameterData::RUN;
			} else if (this->swimming_states->at(i) == CellParameterData::TUMBLE
					&& this->last_swimming_states->at(i) == CellParameterData::RUN) {
				this->changeDirDistribution(i);
				this->last_swimming_states->at(i) = CellParameterData::TUMBLE;
			}
		}
	}
	this->internalTime = this->internalTime + t;

	// done by timekeeper simulator now
	//((DoubleData*)(agent->getDatabase()->getDataItem("localworldtime")))->setDouble(this->internalTime);
}


void ChemotaxisMovementSimulatorInWorld::swimToNewPosition(int i) {
	this->moveToNewPosition(i);
	this->changeDirRotDiffusion(i);
}

void ChemotaxisMovementSimulatorInWorld::moveToNewPosition(int i) {

	//cerr<<"query cell speed @ "<<i<<endl;
	//cerr<<"cellspeeds.size()  "<<cellspeeds->size()<<endl;
	//cerr<<"cellspeeds->at(i)  "<<cellspeeds->at(i)<<endl;

	double distanceTraveled = this->cellspeeds->at(i) * this->movement_dt->getDouble();
	// the direction vector is always a unitvector!
	this->cell_positions->at(i)->at(0) += distanceTraveled*this->cell_dir_vecs->at(i)->at(0);
	this->cell_positions->at(i)->at(1) += distanceTraveled*this->cell_dir_vecs->at(i)->at(1);
	this->cell_positions->at(i)->at(2) += distanceTraveled*this->cell_dir_vecs->at(i)->at(2);
}


void ChemotaxisMovementSimulatorInWorld::changeDirRandom(int i)
{
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);
	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	Util::genUniformRandRotation3d(rotMat);
	Util::applyRotation(rotMat,local_dir);
	Util::applyRotation(rotMat,local_up);


	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];
}


void ChemotaxisMovementSimulatorInWorld::changeDirDistribution(int i) {
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);
	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	double angleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
	Util::genRot3dAboutAxis(rotMat,local_dir,angleInRad);
	Util::applyRotation(rotMat,local_up);

	angleInRad = gs->gen(0.,180.)*Util::PI/180.;
	Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
	Util::applyRotation(rotMat,local_dir);

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];
}

void ChemotaxisMovementSimulatorInWorld::changeDirRotDiffusion(int i) {
	double local_up[3];
	local_up[0] = this->cell_up_vecs->at(i)->at(0);
	local_up[1] = this->cell_up_vecs->at(i)->at(1);
	local_up[2] = this->cell_up_vecs->at(i)->at(2);

	double local_dir[3];
	local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
	local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
	local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

	double rotDiffTerm = sqrt(this->movement_dt->getDouble() * 2. * this->rotdiffconst->getDouble());
	double angleX = rotDiffTerm * Util::RANDOM_GAUSSIAN();
	double angleY = rotDiffTerm * Util::RANDOM_GAUSSIAN();
	double angleZ = rotDiffTerm * Util::RANDOM_GAUSSIAN();

	Util::genRotFromAngles(rotMat, angleX, angleY, angleZ);
	Util::applyRotation(rotMat,local_dir);
	Util::applyRotation(rotMat,local_up);

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];
}
