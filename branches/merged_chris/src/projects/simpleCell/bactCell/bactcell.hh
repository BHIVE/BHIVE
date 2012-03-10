/**
 * This file contains the bactCell agent.
 * @date March 18, 2010
 * @author Christopher Henry
 */

#ifndef BACT_CELL_
#define BACT_CELL_

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>
#include "../simpleCell.hh"

#include "../cartesianGrid/cartesianGrid.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {

///*
// * bactCell Database  ORIGINAL IMPLEMENTATION
// *
// */
//
//class BacteriaDatabase;
//ostream& operator<<(ostream& stream, BacteriaDatabase db);
//istream& operator>>(istream& stream, BacteriaDatabase& db);
//
//class BacteriaDatabase : public Data {
//    friend ostream& SimpleCell::operator<< (ostream& stream, BacteriaDatabase db);
//    friend istream& SimpleCell::operator>> (istream& stream, BacteriaDatabase& db);
//  public:
//    BacteriaDatabase(string name=BACTERIADATABASE, string type=BACTERIADATABASE):Data(name, type){};
//    ~BacteriaDatabase(){};
//    void printContent(ostream& out) {};
//    void unpackDataForMPI(char* buffer,int size) {};
//
//    void setDirection(vector<double> inDirection) {direction =  inDirection;};
//    void setConsumption(vector<double> inConsumption) {consumption =  inConsumption;};
//    void setSpeed(double inSpeed) {speed =  inSpeed;};
//    void setTick(double inTick) {tick =  inTick;};
//    void setLastSubstrateConcentration(double inLastSubstrateConcentration) {
//        lastSubstrateConcentration =  inLastSubstrateConcentration;}
//    void setCurrentConcentrations(vector <double> inCurrentConcentrations) {
//        currentConcentrations =  inCurrentConcentrations;}
//
//    vector<double> getDirection() {return direction;};
//    vector<double> getConsumption() {return consumption;};
//    double getSpeed() {return speed;};
//    double getTick() {return tick;};
//    double getLastSubstrateConcentration() {return lastSubstrateConcentration;};
//    vector <double> getCurrentConcentrations() {return currentConcentrations;};
//  protected:
//    vector<double> direction;
//    double speed;
//    double lastSubstrateConcentration;
//    vector<double> currentConcentrations;
//    vector<double> consumption;
//    double tick;
//};

/**
 * bactCell Database  MODIFIED FOR E.COLI SIMULATIONS
 *
 * This new database is to be used with the coarse-grained e.coli simulator.
 * That simulator currently consists of a sensory system (hard coded parameters
 * in the simulator), a signaling network (parameters of adaptation time-scale
 * and noise), and a single flagellar motor model.  These should really be split
 * into separate simulators, each acting in the same agent, so that we can
 * swap out one signaling module for another, for example, without changing
 * anything else.  There is also a movement/motion simulator that should be changed.
 *
 * @author michael, garrit
 *
 */
class BacteriaDatabase;
// forward declaration
class Ecoli_Simulator;
ostream& operator<<(ostream& stream, BacteriaDatabase db);
istream& operator>>(istream& stream, BacteriaDatabase& db);

class BacteriaDatabase : public Data {
    friend ostream& SimpleCell::operator<< (ostream& stream, BacteriaDatabase db);
    friend istream& SimpleCell::operator>> (istream& stream, BacteriaDatabase& db);
  public:
    BacteriaDatabase(string name=BACTERIADATABASE, string type=BACTERIADATABASE):Data(name, type){
    	// initialising all the data
    	// very dirty and very quick
    	// note that we are not able to change any of these values in the factory.
    	celltime = 0;
    	eqtime = 0;

    	outputinterval = 10;
    	swimming_state = 0;
    	last_swimming_state = 0;

    	ligands.resize(1);
    	ligands[0] = 0;
    	accumulated_ligands.resize(1);
    	accumulated_ligands[0] = 0;

    	CellSpeed = 20;
    	RotDiffConst = 0.062;
    	n_motors = 1;
    	flagellaModelType = 0;
    	isFlagellaCoordinated = false;
    	motor_Kd = 3.06;
    	motor_g1 = 40;
    	motor_w  = 1.3;

    	MotorStates.resize(1);
    	MotorStates[0] = 0;
    	LastMotorStates.resize(1);
    	LastMotorStates[0] = 0;

    	flagellaState.resize(1);
    	flagellaState[0] = 0;
    	lastFlagellaState.resize(1);
    	lastFlagellaState[0] = 0;

    	minNumberOfMotorsInCCWneededToSwim = 1;
    	meanCheYp = 2.9;
    	CheYp.resize(1);
    	CheYp[0] = 2.9;
    	tau = 15;
    	sigma = 0.1;
    	alpha = 6.0;
    	e0 = 19;
    	e1 = -0.45*19;
    	KoffTar = 0.02e-3;
    	KonTar = 0.5e-3;
    	KoffTsr = 100e-3;
    	KonTsr = 1000;
    	TarCount = 6;
    	TsrCount = 13;

    	CCW = 0;
    	CW = 1;
    	RUN = 0;
    	TUMBLE = 1;
    	NORMAL = 3;
    	SEMI = 4;
    	CURLY = 5;
    	NONE = 6;

    	methLevel.resize(1);
    	methLevel[0] = 2;
    	act.resize(1);
    	act[0] = 0.3;

    	tracesNeeded = 1;
    	sigM = 0;
    	areMotorsCorrelated = true;

    	//starting direction
    	int X = 0; int Y=1; int Z=2;
    	pos[X] = 0; pos[Y] = 0; pos[Z] = 0;
    	dir[X]=1; dir[Y]=0; dir[Z]=0;
    	up[X]=0; up[Y]=1; up[Z]=0;

    	//Init the rotation matrix
    	rotMat[0][0]=1;   rotMat[0][1]=0;   rotMat[0][2]=0;
    	rotMat[1][0]=0;   rotMat[1][1]=1;   rotMat[1][2]=0;
    	rotMat[2][0]=0;   rotMat[2][1]=0;   rotMat[2][2]=1;

    	lastPos[X]=pos[X]; lastPos[Y] = pos[Y]; lastPos[Z] = pos[Z];
    	lastDir[X]=dir[X]; lastDir[Y]=dir[Y]; lastDir[Z]=dir[Z];
    	lastUp[X]=up[X]; lastUp[Y]=up[Y]; lastUp[Z]=up[Z];

    	Util::genUniformRandRotation3d(rotMat);
    	Util::applyRotation(rotMat,dir);
    	Util::applyRotation(rotMat,up);
    };

    ~BacteriaDatabase(){};
    void printContent(ostream& out) {};
	void printDBObservables(OutputWriter *ow);
	void setDBObservables(OutputWriter *ow);
	void packDataForMPI(char*& buffer, int& size);
	void unpackDataForMPI(char* buffer, int size);

    void setDirection(vector<double> inDirection) {direction =  inDirection;};
    void setConsumption(vector<double> inConsumption) {consumption =  inConsumption;};
    void setSpeed(double inSpeed) {speed =  inSpeed;};
    void setTick(double inTick) {tick =  inTick;};
    void setLastSubstrateConcentration(double inLastSubstrateConcentration) {
    	lastSubstrateConcentration =  inLastSubstrateConcentration;}
	void setCurrentConcentrations(vector <double> inCurrentConcentrations) {
		currentConcentrations =  inCurrentConcentrations;
		ligands[0] = currentConcentrations[0];
	}

	void setTimescale(double timescale) {
		this->tau = timescale;
	}

	void setMeanCheYp(double cheYp) {
		this->meanCheYp = cheYp;
	}

	void setNoise(double noise) {
		this->sigma = noise;
	}

	vector<double> getDirection() {return direction;};
	vector<double> getConsumption() {return consumption;};
	double getSpeed() {return speed;};
	double getTick() {return tick;};
    double getLastSubstrateConcentration() {return lastSubstrateConcentration;};
    vector <double> getCurrentConcentrations() {return currentConcentrations;};

	void setOrigAgentID(int inID) {originalID = inID;};
	void setOrigAgentCPU(int inCPU) {originalCPU = inCPU;};
	int getOrigAgentID() {return originalID;};
	int getOrigAgentCPU() {return originalCPU;};

    /// ecoli simulator can see all our private variables
    friend class Ecoli_Simulator;
  private:
	int originalCPU;
	int originalID;
    double celltime;
    double eqtime;
    double outputinterval;
    int swimming_state;
    int last_swimming_state;
    vector<double> ligands;
    vector<double> accumulated_ligands;

    double CellSpeed;
    double RotDiffConst;
   int n_motors;
   int flagellaModelType;
   bool isFlagellaCoordinated;
   double motor_Kd;
   double motor_g1;
   double motor_w;
   vector<double> MotorStates;
   vector<double> LastMotorStates;
   vector<double> flagellaState;
   vector<double> lastFlagellaState;
   int minNumberOfMotorsInCCWneededToSwim;

   double meanCheYp;
   vector<double> CheYp;

   double tau;
   double sigma;
   double alpha;
   double e0;
   double e1;

   double KoffTar;
   double KonTar;
   double KoffTsr;
   double KonTsr;
   int TarCount;
   int TsrCount;

   int CCW;
   int CW;
   int RUN;
   int TUMBLE;
   int NORMAL;
   int SEMI;
   int CURLY;
   int NONE;

   vector<double> methLevel;
   vector<double> act;
   int tracesNeeded;
   double sigM;
   bool areMotorsCorrelated;

	//Keep track of curent 3d position, direction, and local orientation
	// (could use a quanternion and save less numbers, but I like my intuitive vectors...)
	//  also, I ran a test using double values on a 64bit linux machine to test the error
	//  that develops between the orthogonality between the up and dir vectors.  Running
	//  10 million random rotations on dir and up, I get only a ~.005 error in the degree (meaning
	//  the degree between the two vectors was ~90.005 instead of 90 exactly).  I believe this to
	//  be a reasonable error, as if we apply a new rotation every .01 seconds in the simulation,
	//  we only get an error that is greater than about 0.005 degrees after 27.7 simulation hours,
	//  which is ok for me...  (note, in the same test, the magnitude of the vectors changes by
	//  less than 0.00001 (they were 0.999991 instead of exactly 1)
	double pos[3];
	double dir[3];
	double up[3];
	double rotMat[3][3];

	double lastPos[3];
	double lastDir[3];
	double lastUp[3];

	vector<double> direction;
	double speed;
	double lastSubstrateConcentration;
	vector<double> currentConcentrations;
	vector<double> consumption;
	double tick;
};

/*
 * bactCell Simulator  ORIGINAL
 *
 */
class bactSimulator : public Simulator {
  public:
    bactSimulator();
    ~bactSimulator();
    void step(Agent* inAgent,double t);
    void setSubstrateID(int inSubstrateId){substrateID = inSubstrateId;};
  protected:
    void initialise();
    int substrateID, productID;
};

////////////////////////////////////////////////////
/*
 * e. coli coarse-grained simulator
 *
 * For now, this simulator accesses the current state
 *
 */
class Ecoli_Simulator : public Simulator {
  public:
	Ecoli_Simulator();
    ~Ecoli_Simulator();
    void step(Agent* inAgent,double t);
  protected:
    void initialise();
    //int substrateID, productID;
    //Signaling parameters, now, these parameters are the same for all cells,
    //as this info is on the
    //but this should be updated very soon to differ for each cell

    // some local variables, so that we don't have to reinitialize them
    // everytime.
    double dt;
    double sqrtOfDt;
    double internalTime;

    //signaling local variables
    double TarTerm;
    double TsrTerm;
    double m0;
    double Mdot;

    // movement simulator functions
	static const int X;
	static const int Y;
	static const int Z;

	void changeDirRandom(BacteriaDatabase* db);
	void changeDirDistribution(BacteriaDatabase* db);
	void changeDirRotDiffusion(BacteriaDatabase* db, double elapsedTime);
	void swimToNewPosition(BacteriaDatabase* db, double elapsedTime);
	void moveToNewPosition(BacteriaDatabase* db, double elapsedTime);

	Util::GammaSampler *gs;
};

/*
 * bactCell Message Database
 *
 */
class CellMessageToWorld : public Data {
  public:
    CellMessageToWorld(string name="cell_message", string type="cell_message"):Data(name, type){};
    ~CellMessageToWorld() {};
    void printContent(ostream& out) {};
	void printDBObservables(OutputWriter *ow) {};
	void setDBObservables(OutputWriter *ow) {};
    void setSpeed(double x,double y,double z) {dx=x;dy=y;dz=z;};
    vector<double> getSpeed();
    void setConsumption(double inConsumption);
    double getConsumption();
    void unpackDataForMPI(char* buffer,int size) {};
  protected:
    double dx;
    double dy;
    double dz;
    double consumption;
};
/*
 * bactCell Message Generators
 *
 */
class GenerateCellMessageToWorld : public MessageGenerator {//chris,ric
    public:
    GenerateCellMessageToWorld(Agent *ag, int inCommunicationRound = 0) :
        MessageGenerator(ag, true, inCommunicationRound) {}
    void placeMessage(int destID,int node=0);
};

/*
 * bactCell Actions
 *
 */
class SyncCellToWorld : public Action {
    public:
        void fire (Agent* ag,Data *d,int source);
};

/*
 * bactCell Factory
 *
 */
class bactCellFactory : public AgentFactory {
  public:

	bactCellFactory() {

		// set to negative one or false for now
		this->fromRandomDistribution = false;
		this->meanCheYp = -1;
		this->noise = -1;
		this->timescale = -1;

	};

	/*
	 * To generate cells from a completely random distribution, set the
	 * flag to true.  Otherwise, setting this to false will create a
	 * default cell based on the defaults in the bactCellFactory.
	 */
    bactCellFactory(bool fromRandomDistribution);

	/*
	 * To generate cells with specific adaptation, noise, and bias parameters,
	 * pass those values to this function, and all cells will have the same
	 * values.
	 *
	 * timescale = timescale of adaptation in seconds (usual range is 0.5s-40s)
	 * noise = signaling noise as a fraction of the mean (usual range 0 - 0.3);
	 * meanCheYp = based on the mean level of this signaling protein, we can
	 *  set the probability for the motor to be in the CW rotational state (usual range
	 *  for CheYp concentration is: 2.2 to 3.1, which produces biases roughly between
	 *  0 and 0.5)
	 *
	 */
    bactCellFactory(double timescale, double noise, double meanCheYp);

    bactCellFactory(InputSystemReader* isr);
    ~bactCellFactory();
    Agent *createAgent() { cout<<"cannot call SimpleCell::createAgent()"<<endl; exit(1); };
    Agent *createAgent(BacteriaDatabase* bactDB);
    Agent *createAgent(double consumption,double currentGlucoseConcentration,double speed);
    Agent *createAgent(Action **ac);
    Agent *createAgent(xmlParsedNode* node);
    Agent *duplicateAgent(Agent *ag);


    // since we have so many create agent functions, we'll just stuff all the initialiser
    // work into this setParameters function, which will override tau, noise, bias as
    // need be.
    void setParameters(Agent *a);


    void finalise();
  protected:
    void addActionsToAgentsActionSet(Agent *ag);
    void addSimulatorToAgent(Agent *ag);

    /* set to true to create cells with some random distribution of tau,noise,bias
     * or false otherwise, which will then use default values
     */
    bool fromRandomDistribution;
    double timescale;
    double noise;
    double meanCheYp;

  private:
};

}
#endif /* BACTCELL_ */
