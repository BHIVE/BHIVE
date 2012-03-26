#include "chemotaxisMovementInWorld.hh"
#include <iostream>
#include <math.h>
#include <float.h>

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
	this->cell_voxels = 0;
	this->cell_dir_vecs  = 0;
	this->cell_up_vecs   = 0;
	this->boundary_behavior = 0;
	this->wall_rot = 0;

	this->tumble_style_use_gamma = true;

	this->isGrid = false;

	this->generate_run_tumble_output = false;
	this->generate_Xdir_output = false;

	// use when you want to record the run-length distribution
	this->run_tumble_output_stream = 0;
}

ChemotaxisMovementSimulatorInWorld::~ChemotaxisMovementSimulatorInWorld() {
	if(generate_run_tumble_output) {
		// if you do not close this you risk of not clearing the buffer such that you may have an empty file.
		run_tumble_output_stream->close();
	}
	delete gs;
}

void ChemotaxisMovementSimulatorInWorld::setWorldAgent(Agent *ag) {
	this->agent        = ag;
	this->internalTime = 0;

	this->initialise();
}

void ChemotaxisMovementSimulatorInWorld::setGrid(string gridname)  {
	this->isGrid = true;
	this->g = (Grid *)agent->getDatabase()->getDataItem(gridname);
}

void ChemotaxisMovementSimulatorInWorld::initialise() {
	// neglecting error checking ...
	Database *db = agent->getDatabase();

	// setting the equilibration time.
	this->eqTime        = ((DoubleData*) db->getDataItem("eqTime"))->getDouble();

	// i hope i didn't screw up on the names of the data items ...
	this->numberofcells = (IntegerData*) db->getDataItem("numberofcells");
	if(db->existsDataItem("rotational_diffusion_constant")) {
		this->rotdiffconst  = (DoubleData*)  db->getDataItem("rotational_diffusion_constant");
		cerr<<"# in ChemotaxisMovementSimulatorInWorld::initialize(), detected ROTDIF = "<<this->rotdiffconst->getDouble()<<endl;
	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no ROTDIF flag found"<<endl;
		cerr<<"# WARNING! : for legacy reasons, the hive will assume cells have rotational diffusion"<<endl;
		cerr<<"# WARNING! : constants of 0.062 rad^2/s, but this should be made explicit!"<<endl;
		this->rotdiffconst = new DoubleData("rotational_diffusion_constant",0.062);
		db->addData(rotdiffconst);
	}
	this->movement_dt   = (DoubleData*)  db->getDataItem("dt");

	this->cellspeeds           = (TVectorData<double>*) db->getDataItem("cellspeeds");
	this->last_swimming_states = (TVectorData<int>* )   db->getDataItem("lastswimmingstates");
	this->swimming_states      = (TVectorData<int>* )   db->getDataItem("swimmingstates");


	if(db->existsDataItem("tumble_style_use_gamma")) {
			this->tumble_style_use_gamma = ((BoolData *) db->getDataItem("tumble_style_use_gamma"))->getBool();
	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no 'tumble_style_use_gamma' data found, assuming gamma distribution."<<endl;
	}

	this->cell_positions = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellpositions");
	if(db->existsDataItem("cell_voxel_positions")) {
		this->cell_voxels = (TVectorData<int>* ) db->getDataItem("cell_voxel_positions");
	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no 'cell_voxel_positions' data found"<<endl;
	}
	this->cell_dir_vecs  = (TVectorData<TVectorData<double>* >* ) db->getDataItem("celldirvecs");
	this->cell_up_vecs   = (TVectorData<TVectorData<double>* >* ) db->getDataItem("cellupvecs");


	// check if movement is in 2d.  If no flag was given, we assume 3D
	if(db->existsDataItem("isIn2D")) {
		this->isIn2D         = (BoolData *) db->getDataItem("isIn2D");

	} else {
		cerr<<"# WARNING! : in ChemotaxisMovementSimulatorInWorld::initialize(), no DIMENSION flag found"<<endl;
		cerr<<"# WARNING! : for legacy reasons, the hive will assume cells move in 3D, but this should be explicit!"<<endl;
		this->isIn2D = new BoolData("isIn2D",false);
		db->addData(isIn2D);
	}
	//db->printDataBase(cerr);
    // We have to make sure the up vector is in Z and the direction vector is in X
	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
		//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
		cell_dir_vecs->at(i)->at(0)=1;
		cell_dir_vecs->at(i)->at(1)=0;
		cell_dir_vecs->at(i)->at(2)=0;
		cell_up_vecs->at(i)->at(0)=0;
		cell_up_vecs->at(i)->at(1)=0;
		cell_up_vecs->at(i)->at(2)=1;

	}
	if(db->doesDataItemExist("boundaries")) {
		this->boundaries   = (TVectorData<SimpleBoundaryData* >* ) db->getDataItem("boundaries");
	} else {
		this->boundaries   = new TVectorData<SimpleBoundaryData* > ();
	}

	for (int i=0; i<this->numberofcells->getInteger(); i++) {
		this->changeDirRandom(i);
	}

	//for (int i=0; i<this->numberofcells->getInteger(); i++) {
	//	cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//	cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
	//	}


	// if there is boundary behavior stored, incorporate it
	if(db->doesDataItemExist("boundary_behavior")) {
		this->boundary_behavior = (StringData*) db->getDataItem("boundary_behavior");
	}
	// we should have some kind of error check mechanism in place here and in the rest of the simulator
	//else {
	//	throw HiveException("boundary_behavior variable not found", "ChemotaxisMovementSimulatorInWorld::initialise()");
	//}
	if(db->doesDataItemExist("wall_rot_angle")) {
		this->wall_rot = (DoubleData*) db->getDataItem("wall_rot_angle");
	}


	if(this->generate_Xdir_output) {
		if(db->existsDataItem("xdir_history") && db->existsDataItem("xdirTumble_history")) {

			tumbleTriggeredAverageOFFSET = (DoubleData *) agent->getDatabase()->getDataItem("tumbleTriggeredAverageOFFSET");
			this->hcXDirectionCollectors    = (TVectorData <HistoryCollector *> *) db->getDataItem("xdir_history");
			this->haXDirectionAggregators   = (TVectorData <HistoryAggregator *> *) db->getDataItem("xdirTumble_history");
		} else {
			throw HiveException("flag for generating Xdir output is true, but proper data has not been added to the DB!", "ChemotaxisMovementSimulatorInWorld::initialise()");
		}
	}

}

void ChemotaxisMovementSimulatorInWorld::prepare() {
	// i think there is nothing to do here.
}

void ChemotaxisMovementSimulatorInWorld::synchroniseWithDatabase() {
	// i don't think there is anything to do here.
}

/*
 * This function contains the logic for swimming chemotactic cells
 */
void ChemotaxisMovementSimulatorInWorld::step(double t) {

	// only move cells if we are past the equilibration time
	if (this->internalTime >= this->eqTime) {

		// iterate over all the cells in the world
		for (int i=0; i<this->numberofcells->getInteger(); i++) {

			// first, use our historyCollector to mark cell directions in the X direction if this information is requested output
			if(generate_Xdir_output) { this->haXDirectionAggregators->at(i)->checkScheduledAggregations(internalTime); }
			if(generate_Xdir_output) { this->hcXDirectionCollectors->at(i)->makeHistory(this->cell_dir_vecs->at(i)->at(0)); }

			// if the cell is running, then swim to the next position
			if (this->swimming_states->at(i) == CellParameterData::RUN) {
				// this is for recording the distribution of run-times to file if we start a run
				if (this->last_swimming_states->at(i) == CellParameterData::TUMBLE) {
					if (this->generate_run_tumble_output) {	recordRunTumble(i,1); }
				}
				this->swimToNewPosition(i);
				this->last_swimming_states->at(i) = CellParameterData::RUN;

			// else if this was the first time the cell is tumbling, then choose a new direction and remember output if needed
			} else if (this->swimming_states->at(i) == CellParameterData::TUMBLE && this->last_swimming_states->at(i) == CellParameterData::RUN) {

				// change directions in the style specified
				if(this->tumble_style_use_gamma) { this->changeDirDistribution(i); }
				else { this->changeDirRandom(i); }
				this->last_swimming_states->at(i) = CellParameterData::TUMBLE;
				// this is for recording the distribution of run-times to file if we end a run
				if (this->generate_run_tumble_output) {	recordRunTumble(i,0); }
				if(generate_Xdir_output) { this->haXDirectionAggregators->at(i)->scheduleAggregation(internalTime+(this->tumbleTriggeredAverageOFFSET->getDouble()/2.0)); }
			}

		}
	}
	this->internalTime = this->internalTime + t;
}




void ChemotaxisMovementSimulatorInWorld::swimToNewPosition(int i) {
	double initDist = this->cellspeeds->at(i) * this->movement_dt->getDouble();
	this->moveToNewPosition(i, initDist);
	// account for rotational diffusion!
	this->changeDirRotDiffusion(i);
}

void ChemotaxisMovementSimulatorInWorld::moveToNewPosition(int i, double distance) {
	//cerr<<"query cell speed @ "<<i<<endl;
	//cerr<<"cellspeeds.size()  "<<cellspeeds->size()<<endl;
	//cerr<<"cellspeeds->at(i)  "<<cellspeeds->at(i)<<endl;
	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;


	// huge IF for whether or not there is bdry interaction
	if(this->boundary_behavior != 0) {
		cerr<<"don't use this boundary yet!"<<endl;
		exit(59);

		/*  // get current voxel where cell is sitting
		int voxindex = this->cell_voxels->at(i), j, k;
		NVoxel *here = g->getVoxel(voxindex);
		// get upper-right and lower-left corners of current voxel
		Coordinate *ur = g->getVertexOfVoxel(6,voxindex),
				*ll = g->getVertexOfVoxel(0,voxindex);
		// define target location, posNow for current location, and dir to hold current direction
		TVectorData<double> *target = new TVectorData<double> ("target","tvector_double");
		target->reserveSize(3);
		TVectorData<double> **posNow = &(this->cell_positions->at(i));
		TVectorData<double> *dir = (TVectorData<double>*) this->cell_dir_vecs->at(i);
		// vector for holding distance to walls in x,y,z: ALONG DIR OF MOVEMENT
		double *wallpath = new double[3];

		// target is where cell would like to move
		for(j=0; j<3; j++)
			target->at(j) = (*posNow)->at(j) + distance * dir->at(j);

		// if cell wants to move within current voxel, then it can move
		if( g->insideThisVoxel(target->at(0), target->at(1), target->at(2), voxindex) ) {
			//cout << "i can move!" << endl;
			for(j=0; j<3; j++)
				(*posNow)->at(j) = target->at(j);
		}
		else {
			// find distance ALONG CELL'S PATH to walls
			for(j=0; j<3; j++) {
				if(target->at(j) - (*posNow)->at(j) > 0)
					wallpath[j] = (ur->at(j) - (*posNow)->at(j))/abs(dir->at(j));
				else if(abs(dir->at(j)) < 1e-6)
					// direction is essentially zero, then set distance arbitrarily large
					wallpath[j] = DBL_MAX;
				else
					wallpath[j] = ((*posNow)->at(j) - ll->at(j))/abs(dir->at(j));
			}

			// smallest is 0,1,2 for x,y,z walls respectively (WHICH wall it will hit first)
			// hit = the smallest distance along path before hitting the next wall
			int smallest = 0;
			for(j=1; j<3; j++) {
				if(wallpath[smallest] > wallpath[j])
					smallest = j;
			}
			double hit = wallpath[smallest];

			// move to nearest wall
			for(j=0; j<3; j++)
				(*posNow)->at(j) += hit * dir->at(j);
			// correct for machine error
			if( (*posNow)->at(smallest) - ll->at(smallest) < 1e-6 )
				(*posNow)->at(smallest) = ll->at(smallest);
			else
				(*posNow)->at(smallest) = ur->at(smallest);

			// incorporates (incomplete) model for rotation due to wall effect
			// currently based on 2006 Lauga paper (OmegaZ)
			// This IF was chosen to be after cell moves to wall, so its effect is on
			//   next iteration of movement
			if(this->boundary_behavior->getString() == "run_with_wall") {
				// feel wall effects within THRESH microns from wall
				double THRESH = 2.5;
				double *walldist = new double[6];
				// find distance to walls: lower,upper x, lower,upper y, lower,upper z
				for(k=0; k<6; k++) {
					if(here->isBoundary(k)) {
						if(k % 2 == 0)
							walldist[k] = (*posNow)->at(k/2) - ll->at(k/2);
						else
							walldist[k] = ur->at((k-1)/2) - (*posNow)->at((k-1)/2);
						// if distance is essentially zero (moved to wall), make very small
						if(abs(walldist[k]) < 1e-6)
							walldist[k] = 1e-6;
					}
					// if not a wall, doesn't matter so set to number larger than THRESH
					else walldist[k] = DBL_MAX;
				}
				for(k=0; k<6; k++){
					if(walldist[k] < THRESH) {
						// rotate direction and up vectors based on rotation rate and wall
						// i: cell index, walldist[k]: distance from wall,
						// hit: distance cell just moved, k: wall index
						changeDirFromWall(i,walldist[k],hit,k);
					}
				}
			}


			// if neighbor exists, change current voxel to that neighbor
			for(j=0; j<here->getNumberNeighbors(); j++) {
				if( g->insideThisVoxel( (*posNow)->at(0), (*posNow)->at(1), (*posNow)->at(2),
						here->getNeighborID(j) ) ) {
					this->cell_voxels->at(i) = here->getNeighborID(j);
					break;
				}
			}
			// if no neighbor, enact boundary behavior to determine change
			if(j == 4) {
				if(this->boundary_behavior->getString() == "reflection")
					ReflectOffWall(i,smallest);
				else if(this->boundary_behavior->getString() == "simple_parallel")
					SimpleParallelWall(i,smallest);
				else if(this->boundary_behavior->getString() == "run_with_wall")
					SimpleParallelWall(i,smallest);
			}

			if(hit > 1e-10) {
				// whatever happens, try to move the rest of the way recursively
				moveToNewPosition(i, distance-hit);
			} else {
				// stuck in a corner!!!  need to randomly rotate to get out!!!
				changeDirRandom(i);
				moveToNewPosition(i, distance);
			}
		}
		delete target; */
	}
	else {
		//ORIGINAL
		// the direction vector is always a unit-vector!
		//this->cell_positions->at(i)->at(0) += distance*this->cell_dir_vecs->at(i)->at(0);
		//this->cell_positions->at(i)->at(1) += distance*this->cell_dir_vecs->at(i)->at(1);
		//this->cell_positions->at(i)->at(2) += distance*this->cell_dir_vecs->at(i)->at(2);


		//WITH INFINITE WALL BOUNDARIES
		double distanceTraveled = this->cellspeeds->at(i) * this->movement_dt->getDouble();
		double start[3];
		start[0] = this->cell_positions->at(i)->at(0);
		start[1] = this->cell_positions->at(i)->at(1);
		start[2] = this->cell_positions->at(i)->at(2);

		// the direction vector is always a unitvector, so this works!
		double predictedEnd[3];
		predictedEnd[0] = this->cell_positions->at(i)->at(0) + distanceTraveled*this->cell_dir_vecs->at(i)->at(0);
		predictedEnd[1] = this->cell_positions->at(i)->at(1) + distanceTraveled*this->cell_dir_vecs->at(i)->at(1);
		predictedEnd[2] = this->cell_positions->at(i)->at(2) + distanceTraveled*this->cell_dir_vecs->at(i)->at(2);

		//cerr<<this->cell_positions->at(i)->at(2)<<endl;
		//cerr<<this->cell_dir_vecs->at(i)->at(2)<<endl;
		double direction[3];
		direction[0] = this->cell_dir_vecs->at(i)->at(0);
		direction[1] = this->cell_dir_vecs->at(i)->at(1);
		direction[2] = this->cell_dir_vecs->at(i)->at(2);

		// check boundary conditions
		for(int k=0; k<this->boundaries->size(); k++) {
			boundaries->at(k)->reflectIfNeeded(start,predictedEnd,direction,distanceTraveled);
		}

	    // update positions and directions
		this->cell_positions->at(i)->at(0) = predictedEnd[0];
		this->cell_positions->at(i)->at(1) = predictedEnd[1];
		this->cell_positions->at(i)->at(2) = predictedEnd[2];
		this->cell_dir_vecs->at(i)->at(0)  = direction[0];
		this->cell_dir_vecs->at(i)->at(1)  = direction[1];
		this->cell_dir_vecs->at(i)->at(2)  = direction[2];

		//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
		//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
		//if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {exit(1);}
	}

}

// reflects cell direction vector over the j axis
// i: cell index,  j: wall index (0,1,2 for x,y,z planes)
void ChemotaxisMovementSimulatorInWorld::ReflectOffWall(int i, int j)
{
	this->cell_dir_vecs->at(i)->at(j) *= -1;
}

// make cell direction vector parallel to wall j
// i: cell index,  j: wall index (0,1,2 for x,y,z planes)
void ChemotaxisMovementSimulatorInWorld::SimpleParallelWall(int i, int j)
{
	double normal = this->cell_dir_vecs->at(i)->at(j);
	// if heading straight into the wall, pick random angle
	if( (abs(normal) - 1) < 1e-6 || abs(normal) < 1e-6) {
		// generate random angle in plane between 0 and 2pi
		double angle = Util::RANDOM(2*Util::PI);
		this->cell_dir_vecs->at(i)->at( (j+1)%3 ) = cos(angle);
		this->cell_dir_vecs->at(i)->at( (j+2)%3 ) = sin(angle);
	} else {
		// scale is the constant to multiply by to retain unit length of direction
		double scale = 1 / sqrt( 1 - pow(normal,2) );
		for(int k=0; k<3; k++)
			if(k != j) this->cell_dir_vecs->at(i)->at(k) /= scale;
	}
	// make parallel to wall j
	this->cell_dir_vecs->at(i)->at(j) = 0;
}

// rotates cell direction vector based on OmegaZ equation in 2006 Lauga paper
// i: cell index, h: distance between cell surface and wall,
// dist: distance cell is trying to move (affects magnitude of rotation)
// wall: wall index (0,1,2 for x,y,z planes)
void ChemotaxisMovementSimulatorInWorld::changeDirFromWall(int i, double h, double dist, int wall)
{
	// CONSTANTS FOR CALCULATING ROTATION RATE
	// avel: angular velocity; b,lambda: radius and wavelength of helix;
	// n: number of wavelengths; phi: aspect ratio of cell; r: radius of bundle
	// w: width of cell;
	double avel=150, b=0.25, lambda=2.5, n=3, phi=2, r=0.05, w=1.2;
	// calculate equivalent sphere radius a
	double t0 = pow(pow(phi,2)-1,0.5),
			t1 = (2*pow(phi,2)-1)/pow(t0,3) * log((phi+t0)/(phi-t0)) - 2*phi/pow(t0,2),
			a = 4*w/(3*t1);
	// L: length of flag. helix; d: dist from wall to cell center
	// c: drag coeff parallel to cell length; e: no biol. meaning
	double L=n*lambda, d=h+0.5*a, c=2*Util::PI/log(2*d/r), e=2*Util::PI*b/lambda;
	// calculate rotation rate
	double et = 1/pow(1+pow(e,2),0.5), nt0 = avel*2*c*b*(1+0.5*pow(e,2))*et;
	// nt1 term of omega function has integral that depends on w,phi,b,h,n,r
	// here, it has been reduced to a polynomial regression dependent only on h
	double nt1 = -0.02377 + 0.04484*h - 0.06692*pow(h,2) + 0.06398*pow(h,3) - 0.02684*pow(h,4);
	double dt0 = c*L*(1+0.75*pow(e,2))*et, dt1 = 6*Util::PI*a*((8/15)*log(a/h)+0.96);
	// CCW angle
	double angle = abs((dist/this->cellspeeds->at(i))*( nt0*nt1/((2/3)*dt0-( pow(dt0,2)/(2*dt0+dt1) )) ));
	if(angle > 1e-6) {
		double axis[3], mat[3][3];
		axis[0] = axis[1] = axis[2] = 0;
		mat[0][0] = 1;	mat[0][1] = 0;	mat[0][2] = 0;
		mat[1][0] = 0;	mat[1][1] = 1; 	mat[1][2] = 0;
		mat[2][0] = 0;	mat[2][1] = 0; 	mat[2][2] = 1;
		if(wall%2 == 0) {
			axis[wall/2] += 1;
		}
		else {
			// CW if +directional wall (i.e. wall%2 == 1)
			angle *= -1;
			axis[(wall-1)/2] += 1;
		}

		double local_up[3];
		local_up[0] = this->cell_up_vecs->at(i)->at(0);
		local_up[1] = this->cell_up_vecs->at(i)->at(1);
		local_up[2] = this->cell_up_vecs->at(i)->at(2);
		double local_dir[3];
		local_dir[0] = this->cell_dir_vecs->at(i)->at(0);
		local_dir[1] = this->cell_dir_vecs->at(i)->at(1);
		local_dir[2] = this->cell_dir_vecs->at(i)->at(2);

		Util::genRot3dAboutAxis(mat,axis,angle);
		Util::applyRotation(mat,local_dir);
		Util::applyRotation(mat,local_up);

		this->cell_up_vecs->at(i)->at(0) = local_up[0];
		this->cell_up_vecs->at(i)->at(1) = local_up[1];
		this->cell_up_vecs->at(i)->at(2) = local_up[2];

		this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
		this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
		this->cell_dir_vecs->at(i)->at(2) = local_dir[2];
	}
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

	if(this->isIn2D->getBool()) {
		double angleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
		Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
	} else {
		Util::genUniformRandRotation3d(rotMat);
		Util::applyRotation(rotMat,local_up);
	}
	Util::applyRotation(rotMat,local_dir);

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

	//	double lastDir[3];
	//	lastDir[0] = local_dir[0];
	//	lastDir[1] = local_dir[1];
	//	lastDir[2] = local_dir[2];

	if(this->isIn2D->getBool()) {
		// generate the angle
		double angleInRad = gs->gen(0.,180.)*Util::PI/180.;
		// rotate around the up direction, which should always be up (0,0,1)
		Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
		Util::applyRotation(rotMat,local_dir);
	} else {
		// First we have to rotate the up vector around the direction (because we
		// can rotate randomly into any dimension from the cells perspective)
		double angleInRad = Util::RANDOM_CLOSED()*2.*Util::PI;
		Util::genRot3dAboutAxis(rotMat,local_dir,angleInRad);
		Util::applyRotation(rotMat,local_up);
		// Then we can rotate the cell direction about the up vector
		angleInRad = gs->gen(0.,180.)*Util::PI/180.;
		Util::genRot3dAboutAxis(rotMat,local_up,angleInRad);
		Util::applyRotation(rotMat,local_dir);
	}

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

	//	double scalar = lastDir[0]*local_dir[0]+lastDir[1]*local_dir[1]+lastDir[2]*local_dir[2];
	//	double a = sqrt(lastDir[0]*lastDir[0]+lastDir[1]*lastDir[1]+lastDir[2]*lastDir[2]);
	//	double b = sqrt(local_dir[0]*local_dir[0] + local_dir[1]*local_dir[1]+local_dir[2]*local_dir[2]);
	//		cout << scalar/(a*b) << endl;

	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;
	//if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {
	//	exit(1);
	//}

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

	//double lastDir[3];
	//lastDir[0] = local_dir[0];
	//lastDir[1] = local_dir[1];
	//lastDir[2] = local_dir[2];

	//cerr<<"\n"<<endl;
	//cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	//cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;

	double rotDiffTerm = sqrt(this->movement_dt->getDouble() * 2. * this->rotdiffconst->getDouble());
	double anglePhi   = 0;
	double angleOmega = 0; // rotates on z/y plane after first rotation
	double anglePsi   = rotDiffTerm * Util::RANDOM_GAUSSIAN(); // rotates on x/y plane after first two rotations
	if(!this->isIn2D->getBool()) {  //if in 3D, we need to have rotations on two other axis as well
		anglePhi   = rotDiffTerm * Util::RANDOM_GAUSSIAN();;
		angleOmega = rotDiffTerm * Util::RANDOM_GAUSSIAN();
	}

	Util::genRotFromAngles(rotMat, anglePhi, angleOmega, anglePsi);
	Util::applyRotation(rotMat,local_dir);

	if(!this->isIn2D->getBool()) {  // save on some multiplications when we run in 2D
		Util::applyRotation(rotMat,local_up);
	}

	this->cell_dir_vecs->at(i)->at(0) = local_dir[0];
	this->cell_dir_vecs->at(i)->at(1) = local_dir[1];
	this->cell_dir_vecs->at(i)->at(2) = local_dir[2];

	this->cell_up_vecs->at(i)->at(0) = local_up[0];
	this->cell_up_vecs->at(i)->at(1) = local_up[1];
	this->cell_up_vecs->at(i)->at(2) = local_up[2];

	//debug the angle deviation
	//cout<<" "<<acos(lastDir[0]*local_dir[0] + lastDir[1]*local_dir[1] + lastDir[2]*local_dir[2] )<< ""<<endl;

//	double scalar = lastDir[0]*local_dir[0]+lastDir[1]*local_dir[1]+lastDir[2]*local_dir[2];
//	double a = sqrt(lastDir[0]*lastDir[0]+lastDir[1]*lastDir[1]+lastDir[2]*lastDir[2]);
//	double b = sqrt(local_dir[0]*local_dir[0] + local_dir[1]*local_dir[1]+local_dir[2]*local_dir[2]);
//	cout << scalar/(a*b) << endl;

/*	cerr<<"dir["<<i<<"]: "<<cell_dir_vecs->at(i)->at(0)<<" "<<cell_dir_vecs->at(i)->at(1)<<" "<<cell_dir_vecs->at(i)->at(2)<<endl;
	cerr<<"up["<<i<<"]: "<<cell_up_vecs->at(i)->at(0)<<" "<<cell_up_vecs->at(i)->at(1)<<" "<<cell_up_vecs->at(i)->at(2)<<endl;

	cerr<<anglePhi<<" "<<angleOmega<<" "<<anglePsi<<" "<<endl;
	cerr<<acos(lastDir[0]*local_dir[0] + lastDir[1]*local_dir[1] + lastDir[2]*local_dir[2] );
	if(abs(cell_dir_vecs->at(i)->at(2))>10e-6 ) {
		cerr<< cell_dir_vecs->at(i)->at(2)*cell_dir_vecs->at(i)->at(2) + cell_dir_vecs->at(i)->at(0)*cell_dir_vecs->at(i)->at(0)  +  cell_dir_vecs->at(i)->at(1)*cell_dir_vecs->at(i)->at(1) <<endl;
		exit(1);
	}
*/

}






void ChemotaxisMovementSimulatorInWorld::setGenerateXDirTrajBeforeTumbleOutput(bool b) {
	this->generate_Xdir_output=b;
}

void ChemotaxisMovementSimulatorInWorld::setGenerateRunTumbleOutput(bool b, string filename) {
	this->generate_run_tumble_output = b;
	if(generate_run_tumble_output) {
		this->run_tumble_output_stream = new ofstream(filename.c_str(),ios_base::binary);
	}
}

/*
 * @param int event, set to zero for start of RUN, set to one for start of TUMBLE
 */
void ChemotaxisMovementSimulatorInWorld::recordRunTumble(int cellIndex,int event) {
	// OUTPUT: time ID pos Run/Tumble
	double d;
	d = this->internalTime;
	this->run_tumble_output_stream->write(reinterpret_cast<char*> (&d), sizeof(d));
	d = (double)cellIndex+1.0;  // note that we need to add one because the world is agentId = 0
	this->run_tumble_output_stream->write(reinterpret_cast<char*> (&d), sizeof(d));
	d = this->cell_positions->at(cellIndex)->at(0);
	this->run_tumble_output_stream->write(reinterpret_cast<char*> (&d), sizeof(d));
	d = (double)event;
	this->run_tumble_output_stream->write(reinterpret_cast<char*> (&d), sizeof(d));
}










