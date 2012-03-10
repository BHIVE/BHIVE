/**
 * @file chemotaxisCellFactory.hh
 *
 * This class implements the ChemotaxisCellFactory that creates chemotaxis
 * cell agents with a movement simulator and a basic flux balance simulator
 *
 * @todo make sure the cell speed and other variables are correct
 *
 * @date Oct 21, 2009  Last edited: Oct 23, 2009 Michael, Garrit, Chris
 * @author Michael Sneddon
 */

#include "chemotaxisCellFactory.hh"
#include "../../../simulators/cellMovement/chemotaxisMovement/chemotaxisMovement.hh"
#include "../../action/chemotaxisAction/chemotaxisAction.hh"
#include "../../messageGenerator/chemotaxisMessageGenerator/chemotaxisMessageGenerator.hh"
#include "../../data/data.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"

#include "../../../simulators/fba/FBASimulator.hh"
#include "../../../projects/chemotaxisAgent/fba_data.hh"
#include "../../../projects/simpleCell/simpleCell.hh"


using namespace Hive;
using namespace Chemotaxis;


ChemotaxisCellFactory::ChemotaxisCellFactory() : AgentFactory()
{
	// Setup the default speed and rotDiffusionConst, measured from Berg
	this->speed = 0.5; // 20um/s
	this->rotDiffConst = 0.06205; //rad/s???? check these units, but it works

	this->startX = 0;
	this->startY = 0;
	this->startZ = 0;

	this->movementDt = 0.01;
}




ChemotaxisCellFactory::ChemotaxisCellFactory(double speed, double rotDiffusionConst)  : AgentFactory()
{
	this->speed = speed;
	this->rotDiffConst = rotDiffusionConst;

	this->startX = 0;
	this->startY = 0;
	this->startZ = 0;

	this->movementDt = 0.01;
}

ChemotaxisCellFactory::~ChemotaxisCellFactory()
{

}


Agent *ChemotaxisCellFactory::createAgent()
{
	Agent *a = new Agent();
	Database *db = a->getDatabase();

	//Setup the database
	Data *speedData = new Hive::DoubleData("CellSpeed",speed);
	Data *rotDiffConstData = new Hive::DoubleData("RotDiffConst",rotDiffConst);
	//Data *posXData = new Hive::DoubleData("xPos",startX);
	//Data *posYData = new Hive::DoubleData("yPos",startY);
	//Data *posZData = new Hive::DoubleData("zPos",startZ);
	DoubleVectorData *position = new Hive::DoubleVectorData("Position",4);
	position->setDouble(a->getAgentId(),0);
	position->setDouble(startX,1);
	position->setDouble(startY,2);
	position->setDouble(startZ,3);
	Data *movementDtData = new Hive::DoubleData("MovementDt",movementDt);

	Data *cellMoveStateData = new Hive::IntegerData("CurrentMovementState",SimpleCell::SimpleCellConstants::RUN);
	Data *lastCellMoveStateData = new Hive::IntegerData("LastMovementState",SimpleCell::SimpleCellConstants::TUMBLE);

	Data *motorCWcount = new Hive::IntegerData("NumberCW_Motors",0);
	Data *motorCCWcount = new Hive::IntegerData("NumberCCW_Motors",1);


	DoubleVectorData *chemicalConc = new Hive::DoubleVectorData("Chemical",3);
	chemicalConc->setDouble(0.0,0);
	chemicalConc->setDouble(0.0,1);
	chemicalConc->setDouble(0.0,2);

	DoubleVectorData *lastChemicalConc = new Hive::DoubleVectorData("LastChemical",2);
	lastChemicalConc->setDouble(0.0,0);
	lastChemicalConc->setDouble(0.0,1);

	/////////// add the created data to the Database
	db->addData(speedData->getName(),speedData);
	db->addData(rotDiffConstData->getName(),rotDiffConstData);
	//db->addData(posXData->getName(),posXData);
	//db->addData(posYData->getName(),posYData);
	//db->addData(posZData->getName(),posZData);
	db->addData("Position",position);
	db->addData(movementDtData->getName(),movementDtData);
	db->addData(cellMoveStateData->getName(),cellMoveStateData);
	db->addData(lastCellMoveStateData->getName(),lastCellMoveStateData);
	db->addData(motorCWcount->getName(),motorCWcount);
	db->addData(motorCCWcount->getName(),motorCCWcount);
	db->addData(chemicalConc->getName(),chemicalConc);
	db->addData(lastChemicalConc->getName(),lastChemicalConc);



	//Add the actions to this agent
	addActionsToAgentsActionSet(a);

	//Add the message generators
	CellNotifyLigandConcToNFsimMessageGenerator *cnlctnmg = new CellNotifyLigandConcToNFsimMessageGenerator(a);
	a->addMessageGenerator(cnlctnmg);

	Chemotaxis::CellNotifyDeltaChemicalToWorldMessageGenerator *cndctwmg = new CellNotifyDeltaChemicalToWorldMessageGenerator(a);
	a->addMessageGenerator(cndctwmg);

	Chemotaxis::CellNotifyPosToWorldMessageGenerator *cnptwmg = new CellNotifyPosToWorldMessageGenerator(a);
	a->addMessageGenerator(cnptwmg);







	//Add the simulator
	ChemotaxisMovementSimulator *cellSimulator = new ChemotaxisMovementSimulator(0);
	cellSimulator->setAgent(a);
	a->addSimulator(cellSimulator);




	//FBA Data and simulator
	FBAData *fbaData = new FBAData();
	fbaData->load("test/FBAModel.txt");
	db->addData(fbaData->getName(),fbaData);

	FBASimulator *fbaSim = new FBASimulator();
	fbaSim->setAgent(a);
	a->addSimulator(fbaSim);



	return a;
}

Agent *ChemotaxisCellFactory::createAgent(Hive::Action ** as)
{
	throw HiveException("Cannot call createAgent(Hive::Action ** as)","ChemotaxisCellFactory::createAgent(Hive::Action ** as)");
	return NULL;
}

Agent *ChemotaxisCellFactory::duplicateAgent(Hive::Agent *ag)
{
	throw HiveException("Cannot call duplicateAgent(Hive::Action ** as)","ChemotaxisCellFactory::duplicateAgent(Hive::Action ** as)");
	return NULL;
}

void ChemotaxisCellFactory::finalise()
{

}


void ChemotaxisCellFactory::addActionsToAgentsActionSet(Hive::Agent *ag)
{
	// ACTION ID = 0
	OverwritePositionAction *opa = new OverwritePositionAction();
	ag->addAction(opa);
	opa->setAgent(ag);

	// ACTION ID = 1
	UpdateSeenChemicalConcentrationAction *uscca = new UpdateSeenChemicalConcentrationAction();
	ag->addAction(uscca);
	uscca->setAgent(ag);

	// ACTION ID = 2
	UpdateMotorConformationAction *umca = new UpdateMotorConformationAction();
	ag->addAction(umca);
	umca->setAgent(ag);
}

void ChemotaxisCellFactory::addSimulatorToAgent(Hive::Agent *ag)
{

}
