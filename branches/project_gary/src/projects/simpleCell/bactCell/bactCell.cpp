/**
 * bactCell
 *
 */

#include "bactCell.hh"
#include "../simpleCell.hh"
#include "../../../hive.hh"
#include "../../../agent/data/data.hh"
#include "../../../agent/data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"
#include "../cartesianGrid/cartesianGrid.hh"

#include <math.h>

using namespace std;
using namespace Hive;
using namespace SimpleCell;

/*
 * bactCell database
 *
 */
void BacteriaDatabase::packDataForMPI(char*& buffer, int& size) {
	stringstream ss;
	if (lastSubstrateConcentration != lastSubstrateConcentration){
		lastSubstrateConcentration = 1;
	}
	for (unsigned int i = 0; i < currentConcentrations.size(); ++i) {
		if (currentConcentrations[i] != currentConcentrations[i]){
			currentConcentrations[i] = 1;
		}
	}
    ss << tick << endl;
	ss << speed << endl;
    ss << lastSubstrateConcentration << endl;
    ss << celltime << endl;
    ss << eqtime << endl;
    ss << outputinterval << endl;
    ss << swimming_state << endl;
    ss << last_swimming_state << endl;
    ss << CellSpeed << endl;
    ss << RotDiffConst << endl;
    ss << n_motors << endl;
    ss << flagellaModelType << endl;
    ss << isFlagellaCoordinated << endl;
    ss << motor_Kd << endl;
    ss << motor_g1 << endl;
    ss << motor_w << endl;
    ss << minNumberOfMotorsInCCWneededToSwim << endl;
    ss << meanCheYp << endl;
    ss << tau << endl;
    ss << sigma << endl;
    ss << alpha << endl;
    ss << e0 << endl;
    ss << e1 << endl;
    ss << KoffTar << endl;
    ss << KonTar << endl;
    ss << KoffTsr << endl;
    ss << KonTsr << endl;
    ss << TarCount << endl;
    ss << TsrCount << endl;
    ss << CCW << endl;
    ss << CW << endl;
    ss << RUN << endl;
    ss << TUMBLE << endl;
    ss << NORMAL << endl;
    ss << SEMI << endl;
    ss << CURLY << endl;
    ss << NONE << endl;
    ss << tracesNeeded << endl;
    ss << sigM << endl;
    ss << areMotorsCorrelated << endl;
	ss << originalCPU << endl;
	ss << originalID << endl;
    ss << pos[0] << endl;
    ss << pos[1] << endl;
    ss << pos[2] << endl;
    ss << dir[0] << endl;
    ss << dir[1] << endl;
    ss << dir[2] << endl;
    ss << up[0] << endl;
    ss << up[1] << endl;
    ss << up[2] << endl;
    ss << rotMat[0][0] << endl;
    ss << rotMat[0][1] << endl;
    ss << rotMat[0][2] << endl;
    ss << rotMat[1][0] << endl;
    ss << rotMat[1][1] << endl;
    ss << rotMat[1][2] << endl;
    ss << rotMat[2][0] << endl;
    ss << rotMat[2][1] << endl;
    ss << rotMat[2][2] << endl;
    ss << lastPos[0] << endl;
    ss << lastPos[1] << endl;
    ss << lastPos[2] << endl;
    ss << lastDir[0] << endl;
    ss << lastDir[1] << endl;
    ss << lastDir[2] << endl;
    ss << lastUp[0] << endl;
    ss << lastUp[1] << endl;
    ss << lastUp[2] << endl;
	ss << direction[0] << endl;
	ss << direction[1] << endl;
	ss << direction[2] << endl;
	ss << int(currentConcentrations.size()) << endl;
	for (int i=0; i < int(currentConcentrations.size()); i++) {
		ss << currentConcentrations[i] << endl;
		ss << consumption[i] << endl;
		ss << ligands[i] << endl;
		ss << accumulated_ligands[i] << endl;
	}
	ss << int(MotorStates.size()) << endl;
	for (int i=0; i < int(MotorStates.size()); i++) {
		ss << MotorStates[i] << endl;
		ss << LastMotorStates[i] << endl;
	}
	ss << int(flagellaState.size()) << endl;
	for (int i=0; i < int(flagellaState.size()); i++) {
		ss << flagellaState[i] << endl;
		ss << lastFlagellaState[i] << endl;
	}
	ss << int(CheYp.size()) << endl;
	for (int i=0; i < int(CheYp.size()); i++) {
		ss << CheYp[i] << endl;
	}
	ss << int(methLevel.size()) << endl;
	for (int i=0; i < int(methLevel.size()); i++) {
		ss << methLevel[i] << endl;
	}
	ss << int(act.size()) << endl;
	for (int i=0; i < int(act.size()); i++) {
		ss << act[i] << endl;
	}
	//Creating the buffer from the stringstream
	buffer = strdup(ss.str().c_str());
    size = strlen(buffer);
}

void BacteriaDatabase::unpackDataForMPI(char* buffer, int size) {
	stringstream ss(buffer);
	ss >> tick;
	ss >> speed;
    ss >> lastSubstrateConcentration;
    ss >> celltime;
    ss >> eqtime;
    ss >> outputinterval;
    ss >> swimming_state;
    ss >> last_swimming_state;
    ss >> CellSpeed;
    ss >> RotDiffConst;
    ss >> n_motors;
    ss >> flagellaModelType;
    ss >> isFlagellaCoordinated;
    ss >> motor_Kd;
    ss >> motor_g1;
    ss >> motor_w;
    ss >> minNumberOfMotorsInCCWneededToSwim;
    ss >> meanCheYp;
    ss >> tau;
    ss >> sigma;
    ss >> alpha;
    ss >> e0;
    ss >> e1;
    ss >> KoffTar;
    ss >> KonTar;
    ss >> KoffTsr;
    ss >> KonTsr;
    ss >> TarCount;
    ss >> TsrCount;
    ss >> CCW;
    ss >> CW;
    ss >> RUN;
    ss >> TUMBLE;
    ss >> NORMAL;
    ss >> SEMI;
    ss >> CURLY;
    ss >> NONE;
    ss >> tracesNeeded;
    ss >> sigM;
    ss >> areMotorsCorrelated;
	ss >> originalCPU;
	ss >> originalID;
    ss >> pos[0];
    ss >> pos[1];
    ss >> pos[2];
    ss >> dir[0];
    ss >> dir[1];
    ss >> dir[2];
    ss >> up[0];
    ss >> up[1];
    ss >> up[2];
    ss >> rotMat[0][0];
    ss >> rotMat[0][1];
    ss >> rotMat[0][2];
    ss >> rotMat[1][0];
    ss >> rotMat[1][1];
    ss >> rotMat[1][2];
    ss >> rotMat[2][0];
    ss >> rotMat[2][1];
    ss >> rotMat[2][2];
    ss >> lastPos[0];
    ss >> lastPos[1];
    ss >> lastPos[2];
    ss >> lastDir[0];
    ss >> lastDir[1];
    ss >> lastDir[2];
    ss >> lastUp[0];
    ss >> lastUp[1];
    ss >> lastUp[2];
	direction.resize(3);
	ss >> direction[0];
	ss >> direction[1];
	ss >> direction[2];
	int vsize;
	ss >> vsize;
	currentConcentrations.resize(vsize);
	consumption.resize(vsize);
	ligands.resize(vsize);
	accumulated_ligands.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> currentConcentrations[i];
		ss >> consumption[i];
		ss >> ligands[i];
		ss >> accumulated_ligands[i];
	}
	ss >> vsize;
	MotorStates.resize(vsize);
	LastMotorStates.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> MotorStates[i];
		ss >> LastMotorStates[i];
	}
	ss >> vsize;
	flagellaState.resize(vsize);
	lastFlagellaState.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> flagellaState[i];
		ss >> lastFlagellaState[i];
	}
	ss >> vsize;
	CheYp.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> CheYp[i];
	}
	ss >> vsize;
	methLevel.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> methLevel[i];
	}
	ss >> vsize;
	act.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> act[i];
	}
}

void BacteriaDatabase::printDBObservables(OutputWriter *ow) {
	if (observableIDs.size() == 0) {
		observableIDs.push_back(ow->getObservableIndex("tau\tmeanCheYp\tsigma"));
		observableIDs.push_back(ow->getObservableIndex("x\ty\tz\tCheYp"));
	}
	vector<int> ids;
	ids.push_back(this->originalCPU);
	ids.push_back(this->originalID);
	vector<double> printData;
	printData.push_back(tau);
	printData.push_back(meanCheYp);
	printData.push_back(sigma);
	ow->updateObservable(observableIDs[0],ids,printData);
	vector<double> printDataTwo;
	printDataTwo.push_back(double(pos[0]));
	printDataTwo.push_back(double(pos[1]));
	printDataTwo.push_back(double(pos[2]));
	printDataTwo.push_back(CheYp[0]);
	ow->updateObservable(observableIDs[1],ids,printDataTwo);
};

void BacteriaDatabase::setDBObservables(OutputWriter *ow) {
	vector<string> ids;
	ids.push_back("CPU");
	ids.push_back("ID");
	observableIDs.push_back(ow->addObservable("tau\tmeanCheYp\tsigma","CellStats.txt",ids,0.01,true));
	observableIDs.push_back(ow->addObservable("x\ty\tz\tCheYp","CellData.txt",ids,0.05,true));
};

/*
 * bactCell Simulator
 *
 */
bactSimulator::bactSimulator(){}
bactSimulator::~bactSimulator(){}

void bactSimulator::initialise() { }
void bactSimulator::step(Agent* inAgent,double dt) {
	BacteriaDatabase* db = (BacteriaDatabase*)inAgent->getDatabase()->getDataItem(BACTERIADATABASE);
	vector<double> direction = db->getDirection();
	double speed = db->getSpeed();
	//double lastGlucoseConcentration = db->getLastGlucoseConcentration();
	//double currentGlucoseConcentration = db->getCurrentGlucoseConcentration();
    double lastSubstrateConcentration = db->getLastSubstrateConcentration();
	vector <double> currentConcentrations = db->getCurrentConcentrations();
    double currentSubstrateConcentaration =  currentConcentrations[substrateID];
    //cout << dx << " " << dy << " " << dz << endl;
    //cout << lastGlucoseConcentration << " " << currentGlucoseConcentration << endl;
	// if the current concentration is worse or the same as the last concentration then change the directions
	if (lastSubstrateConcentration >= currentSubstrateConcentaration){
		direction[0] = 1.0-Util::RANDOM(2.0);
		direction[1] = 1.0-Util::RANDOM(2.0);
		direction[2] = 1.0-Util::RANDOM(2.0);
		double fraction  = 1.0 / sqrt((direction[0]*direction[0]) + (direction[1]*direction[1]) + (direction[2]*direction[2])) * speed;
		direction[0] = direction[0] * fraction ;
		direction[1] = direction[1] * fraction ;
		direction[2] = direction[2] * fraction ;
		db->setDirection(direction);
	}
	db->setLastSubstrateConcentration(currentSubstrateConcentaration);
};

/*
 * bactCell Message Database : CellMessageToWorld
 *
 */
vector<double> CellMessageToWorld::getSpeed() {
	vector<double> speed(3);
	speed[0] = dx;
	speed[1] = dy;
	speed[2] = dz;
	return speed;
};
void CellMessageToWorld::setConsumption(double inConsumption) { consumption = inConsumption; }
double CellMessageToWorld::getConsumption() { return consumption; }

/*
 * bactCell Message Generators : GenerateCellMessageToWorld
 *
 */
void GenerateCellMessageToWorld::placeMessage(int destID,int node) {
	Message *msg = new Message();
	msg->setAction(0);
	msg->setDestination(destID,node);
	msg->setSourceID(source->getAgentId());
	CellMessageToWorld* msgDB = new CellMessageToWorld();
	//Getting cell message data from the cell agent
	BacteriaDatabase* bactDB = (BacteriaDatabase*)(source->getDatabase()->getDataItem(BACTERIADATABASE));
    vector<double> consumption = bactDB->getConsumption();
	msgDB->setConsumption(consumption[0]);
    vector<double> speed = bactDB->getDirection();
	msgDB->setSpeed(speed[0],speed[1],speed[2]);
	//Adding message database to message
	msg->setArgument(msgDB);
	//Placing message in outbox
	source->placeMessageInOutbox(msg);
}

/*
 * bactCell Actions
 *
 */
void SyncCellToWorld::fire (Agent* ag,Data *d,int source) {
	// perform the correct cast of the data, or unpack it if it
	// was send as a raw MPI character buffer.
	WorldMessageToCell *message;
	if(d->getType().compare("RawBufferData")==0) {
		RawBufferData *rbd = (RawBufferData *)d;
		message = new WorldMessageToCell();
		message->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
	} else {
		message = (WorldMessageToCell *) d;
	}
	// do the correct cast of the data
    Database* db = ag->getDatabase();
	BacteriaDatabase* bactDB = (BacteriaDatabase*)db->getDataItem(BACTERIADATABASE);
//	bactDB->setCurrentGlucoseConcentration(message->getConc()[0]); /// ric old version
	bactDB->setCurrentConcentrations(message->getConc()); /// here
}

/*
 * bactCell AgentFactory
 *
 */

bactCellFactory::bactCellFactory(bool fromRandomDistribution)
{
	this->fromRandomDistribution = fromRandomDistribution;

	// set to negative one, because we won't use these
	this->meanCheYp = -1;
	this->noise = -1;
	this->timescale = -1;
}

/*
 * To generate cells with specific adaptation, noise, and bias parameters,
 * pass those values to this function, and all cells will have the same
 * values.
 *
 * timescale = timescale of adaptation in seconds (usual range is 0.5s-40s)
 * noise = signaling noise as a fraction of the mean (usual range 0 - 0.3);
 * cwbias = probability for the motor to be in the CW rotational state (usual range: 0.05-0.4)
 *
 */
bactCellFactory::bactCellFactory(double timescale, double noise, double meanCheYp)
{
	this->fromRandomDistribution = fromRandomDistribution;

	// set to negative one, because we won't use these
	this->meanCheYp = meanCheYp;
	this->noise = noise;
	this->timescale = timescale;
}


bactCellFactory::bactCellFactory(InputSystemReader *isr):AgentFactory(isr) {}
bactCellFactory::~bactCellFactory() {}


void bactCellFactory::setParameters(Agent *a)
{
	if(this->fromRandomDistribution==true)
	{
		BacteriaDatabase* db = (BacteriaDatabase*)a->getDatabase()->getDataItem(BACTERIADATABASE);

		// set these values randomly for now, we can use a realistic
		// distribution in the future.  Not worth doing, though, until
		// we can output these statistics as well.
		db->setTimescale(Util::RANDOM(39.5)+0.5);
		db->setNoise(Util::RANDOM(0.25));
		db->setMeanCheYp(Util::RANDOM(1.0)+2.1);

	} else {

		// If we have a positive timescale of adaptation, then
		// set this same value for all agents.
		if(this->timescale>0) {
			BacteriaDatabase* db = (BacteriaDatabase*)a->getDatabase()->getDataItem(BACTERIADATABASE);

			db->setTimescale(this->timescale);
			db->setNoise(this->noise);
			db->setMeanCheYp(this->meanCheYp);

		}

	}

}



Agent *bactCellFactory::createAgent(BacteriaDatabase* bactDB)
{
	Agent *a = new Agent();
	Database *db = a->getDatabase();
	db->addData(bactDB->getName(),bactDB);

    int inSubstrateID = 0;

    /// changing to use Ecoli simulator
//    bactSimulator *sim = new bactSimulator();
 //   sim->setSubstrateID(inSubstrateID);
    Ecoli_Simulator *sim = new Ecoli_Simulator();
    a->addSimulator(sim);
	addActionsToAgentsActionSet(a);

	//Creating message generator
	GenerateCellMessageToWorld* msgGen = new GenerateCellMessageToWorld(a);
	a->addMessageGenerator(msgGen);


	setParameters(a);
	return a;
};


Agent *bactCellFactory::createAgent(double consumption, double currentGlucoseConcentration, double speed)
{
	Agent *a = new Agent();
    BacteriaDatabase* bactDB = new BacteriaDatabase();
	bactDB->setOrigAgentID(a->getAgentId());
	bactDB->setOrigAgentCPU(mpirank());
	a->getDatabase()->addData(BACTERIADATABASE,bactDB);
	bactDB->setSpeed(speed);
	bactDB->setLastSubstrateConcentration(0);
	vector<double> consumptionVect(1,consumption);
	bactDB->setConsumption(consumptionVect);
	vector<double> direction(3);
	direction[0] = 1.0-Util::RANDOM(2.0);
	direction[1] = 1.0-Util::RANDOM(2.0);
	direction[2] = 1.0-Util::RANDOM(2.0);
	double fraction  = 1.0 / sqrt((direction[0]*direction[0]) +
        (direction[1]*direction[1]) + (direction[2]*direction[2])) * speed;
	direction[0] = direction[0] * fraction ;
	direction[1] = direction[1] * fraction ;
	direction[2] = direction[2] * fraction ;
	bactDB->setDirection(direction);

    int inSubstrateID = 0;


    /// CHANGING TO USE ECOLI_SIMULATOR !!!
    // bactSimulator *sim = new bactSimulator();
    //  sim->setSubstrateID(inSubstrateID);
    Ecoli_Simulator *sim = new Ecoli_Simulator();
    a->addSimulator(sim);
	addActionsToAgentsActionSet(a);

	//Creating message generator
	GenerateCellMessageToWorld* msgGen = new GenerateCellMessageToWorld(a);
	a->addMessageGenerator(msgGen);


	setParameters(a);
	return a;
}

Agent *bactCellFactory::createAgent(xmlParsedNode* node)
{
    Agent* a = new Agent();
    BacteriaDatabase* bactDB = new BacteriaDatabase();
	bactDB->setOrigAgentID(a->getAgentId());
	bactDB->setOrigAgentCPU(mpirank());
	a->getDatabase()->addData(BACTERIADATABASE,bactDB);
    double speed, consumption, currentGlucoseConcentration;
    if (node->attr.count("speed") == 0  || node->attr.count("consumption") == 0 ||
        node->attr.count("currentSubstratConcentration") == 0) {
        cerr << "Invalid definition for bactCell agent" << endl;
        exit(1);
    }
    speed = atof(node->attr["speed"].c_str());
    consumption = atof(node->attr["consumption"].c_str());
    currentGlucoseConcentration = atof(node->attr["currentGlucoseConsumption"].c_str());
	bactDB->setSpeed(speed);
	bactDB->setLastSubstrateConcentration(0);
	vector<double> consumptionVect(1,consumption);
	bactDB->setConsumption(consumptionVect);
    int inSubstrateID = 0;///
    // now set initial orientation randomly
	vector<double> direction(3);
	direction[0] = 1.0-Util::RANDOM(2.0);
	direction[1] = 1.0-Util::RANDOM(2.0);
	direction[2] = 1.0-Util::RANDOM(2.0);
	double fraction  = 1.0 / sqrt((direction[0]*direction[0]) +
        (direction[1]*direction[1]) + (direction[2]*direction[2])) * speed;
	direction[0] = direction[0] * fraction ;
	direction[1] = direction[1] * fraction ;
	direction[2] = direction[2] * fraction ;
	bactDB->setDirection(direction);
    // Adding simulators, actions
	//addSimulatorToAgent(a);
	addActionsToAgentsActionSet(a);
//    bactSimulator *sim = new bactSimulator();
//    sim->setSubstrateID(inSubstrateID);

	/// changing to use ecoli simulator!
	Ecoli_Simulator *sim = new Ecoli_Simulator();
	a->addSimulator(sim);
    //Creating message generator
	GenerateCellMessageToWorld* msgGen = new GenerateCellMessageToWorld(a);
	a->addMessageGenerator(msgGen);


	setParameters(a);
    return a;
}

Agent *bactCellFactory::createAgent(Action **ac)
{
	return 0;
}


Agent *bactCellFactory::duplicateAgent(Agent *ag)
{
	return 0;
}

void bactCellFactory::finalise()
{

}

void bactCellFactory::addActionsToAgentsActionSet(Agent *ag)
{
	SyncCellToWorld* newAction = new SyncCellToWorld;
	ag->addAction(newAction);
}

void bactCellFactory::addSimulatorToAgent(Agent *ag) { // not used
	// //////////////////////////////////////////////
	// Set the Bacterial Simulator
    // //////////////////////////////////////////////
	bactSimulator *sim = new bactSimulator();
    ag->addSimulator(sim);
}

