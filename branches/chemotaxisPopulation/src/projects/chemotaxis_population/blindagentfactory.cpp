#include "blindagentfactory.hh"

using namespace std;
using namespace Hive;
using namespace ChemoPop;


BlindAgentFactory::BlindAgentFactory()  { }

BlindAgentFactory::BlindAgentFactory(InputSystemReader *isr) { }

BlindAgentFactory::~BlindAgentFactory() { }


Agent* BlindAgentFactory::createAgent()
{
	cerr<<"BlindAgentFactory::createAgent()"<<endl;

	//Create the agent and the Database
	Agent *bond = new Agent();
	Hive::Database *db = bond->getDatabase();

	// //////////////////////////////////////////////////////////////////
	// Create the base Data objects that are needed by the blind agent
	/////////////////////////////////////////////////////////////////////

	// type name
	StringData *mytypename = new StringData("mytypename", "hans");
	db->addData(mytypename->getName(), mytypename);

	// Internal clock
	DoubleData *celltime = new DoubleData("celltime", 0.0);
	db->addData(celltime->getName(), celltime);

	// record the last dt
	DoubleData *dt = new DoubleData("dt", 0);
	db->addData(dt);

	DoubleData *noutt = new DoubleData("nextOutputTime", 0.0);
	db->addData(noutt->getName(), noutt);

	// Equilibration Time  used by chemotaxis model
	DoubleData *eqtime = new DoubleData("eqtime", cpi->getEqTime());
	db->addData(eqtime->getName(), eqtime);

	// Output frequency of data from the cell may or may not be used
	DoubleData *outputinterval_Data = new DoubleData("outputinterval",this->output_interval);
	db->addData(outputinterval_Data->getName(), outputinterval_Data);

	// at the present the world as well as the metabolism simulatots can only handle one ligand profile
	TVectorData<double> *ligands = new TVectorData<double> ("ligands","tvector_double");
	ligands->reserveSize(1);
	ligands->at(0) = 0;
	db->addData(ligands->getName(),ligands);

	// this stores how much  nutrient a cell would like to get from the world
	TVectorData<double> *appetite = new TVectorData<double> ("appetite", "tvector_double");
	appetite->reserveSize(1);
	appetite->at(0) = 0;
	db->addData(appetite->getName(), appetite);

	// parameter that sets the base effeciency at which nutrient is removed and added to energy
	DoubleData *effeciency = new DoubleData("base_effeciency_of_conversion", 1);
	db->addData(effeciency);

	DoubleData *r1 = new DoubleData("r1", 1);
	db->addData(r1);

	// Marker of the generation of the cell
	IntegerData *generationData = new IntegerData("generation",0);
	db->addData(generationData);

	/// flag that will be set by the death simulator, if the cell has to die
	BoolData *death_flag = new BoolData("death_flag", false);
	db->addData(death_flag->getName(), death_flag);

	/// flag that will be set by the birth simulator, if the cell gives rise to offspring
	BoolData *birth_flag = new BoolData("birth_flag", false);
	db->addData(birth_flag->getName(), birth_flag);

	// UPDATED METABOLISM / BIRTH / DEATH SIMULATOR
	// starting or default values, same for all cells
	double starting_essence = 0.5;
	double default_kcat = 5;
	double default_Km   = 0.1;
	double default_essence_cost_for_movement = 0.025;
	double default_mass_threshold_for_birth = 1;
	double default_essence_threshold_for_death = 0.000001;
	double default_background_death_rate_per_unit_time = 0; // not sure whether it is a good idea to have this in the cell.
    double default_yield = 1;
    bool   default_is_levy = false;
    double default_parameter_for_steplength_dist = 1;
    double default_rho = 0.3;
    double default_velocity = 2;
    double default_current_angle = 2.0;
    double default_distance_desired_to_travel = 0.0;
    double default_traveled_distance = 0.0;

	// create the data items storing these parameters
	DoubleData *essence = new DoubleData("essence",starting_essence);
	// kcat and Km are the rescaled variables
	DoubleData *kcat = new DoubleData("kcat",default_kcat);
	DoubleData *Km = new DoubleData("Km",default_Km);
	// this is the parameter alpha
	DoubleData *essence_cost_for_movement = new DoubleData("essence_cost_for_movement",default_essence_cost_for_movement);
	DoubleData *background_death_rate_per_unit_time = new DoubleData("background_death_rate_per_unit_time",default_background_death_rate_per_unit_time);
	// the user will not be able to set the birth threshold it  is always equal to one
	DoubleData *essence_threshold_for_birth = new DoubleData("essence_threshold_for_birth",default_mass_threshold_for_birth);
	DoubleData *essence_threshold_for_death = new DoubleData("essence_threshold_for_death",default_essence_threshold_for_death);
	DoubleData *velocity = new DoubleData("velocity",default_velocity);
	DoubleData *yield = new DoubleData("yield", default_yield);
	BoolData   *is_levy = new BoolData("is_levy", default_is_levy);
	DoubleData *rho = new DoubleData("rho", default_rho);
	DoubleData *parameter_for_steplength_dist =new DoubleData("parameter_for_steplength_dist", default_parameter_for_steplength_dist);

	// variables needed for the movement simulator to operate correctly
	DoubleData *current_angle = new DoubleData("current_angle", default_current_angle);
	DoubleData *distance_desired_to_travel = new DoubleData("distance_desired_to_travel", default_distance_desired_to_travel);
	DoubleData *traveled_distance = new DoubleData("traveled_distance", default_traveled_distance);

	// add the data items to the database
	db->addData(essence);
	db->addData(kcat);
	db->addData(Km);
	db->addData(essence_cost_for_movement);
	db->addData(background_death_rate_per_unit_time);
	db->addData(essence_threshold_for_birth);
	db->addData(essence_threshold_for_death);
	db->addData(velocity);
	db->addData(yield);
	db->addData(current_angle);
	db->addData(distance_desired_to_travel);
	db->addData(traveled_distance);
	db->addData(is_levy);
	db->addData(rho);
	db->addData(parameter_for_steplength_dist);


	this->cpi->setNextCellParameters(bond);



	// ///////////////////////////////////////////////////////////////////
	// add simulators to agent
	//////////////////////////////////////////////////////////////////////
	this->addSimulatorToAgent(bond);

	//////////////////////////////////////////////////////////////////////
	// add actions to agents action set
	//////////////////////////////////////////////////////////////////////
	this->addActionsToAgentsActionSet(bond);

	//////////////////////////////////////////////////////////////////////
	// add message generators to action set
	//////////////////////////////////////////////////////////////////////
	this->addMessageGeneratorsToAgent(bond);


	return bond;
}




Agent* BlindAgentFactory::createAgent(Action **as) {
	Agent *bond = new Agent();
	return bond;
}

Agent* BlindAgentFactory::duplicateAgent(Agent *ag) {
	Agent *bond = new Agent();
	return bond;
}

void BlindAgentFactory::finalise() {
}


void BlindAgentFactory::addActionsToAgentsActionSet(Agent *ag) {
	UpdateCellSeenLigandAction *ucsla = new UpdateCellSeenLigandAction();
	ucsla->setAgent(ag);
	ag->addAction(ucsla);

	UpdateCellAccumulatedLigandAction *ucala = new UpdateCellAccumulatedLigandAction();
	ucala->setAgent(ag);
	ag->addAction(ucala);
}


void BlindAgentFactory::addMessageGeneratorsToAgent(Agent *ag) {
	// this will need update
	BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator *banwaaavmg = new BlindAgentNotifyWorldAboutAngleAndVelocityMessageGenerator(ag);
	ag->addMessageGenerator(banwaaavmg);

	CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator *ctwadncmg =
		new CellTellWorldAboutDesiredNutrientConsumptionMessageGenerator(ag);
	ag->addMessageGenerator(ctwadncmg);

	BlindAgentRevealItselfToSpecialAgentMessageGenerator *baritsamg = new BlindAgentRevealItselfToSpecialAgentMessageGenerator(ag);
	ag->addMessageGenerator(baritsamg);


	BlindAgentNotifyWorldThatNewAgentIsBorn * banwtnaib =
			new BlindAgentNotifyWorldThatNewAgentIsBorn(ag);
	ag->addMessageGenerator(banwtnaib);

	BlindAgentNotifyWorldThatItIsDead * banwtiid =
			new BlindAgentNotifyWorldThatItIsDead(ag);
	ag->addMessageGenerator(banwtiid);
}


void BlindAgentFactory::addSimulatorToAgent(Agent *ag)
{
	/// add movement simulator to cell
	if (((BoolData*) ag->getDatabase()->getDataItem("is_levy"))->getBool()) {
		LevyRunLengthSimulator *levy = new LevyRunLengthSimulator();
		levy->setAgent(ag);
		ag->addSimulator(levy);
	} else {
		ExponentialRunLengthSimulator *expo = new ExponentialRunLengthSimulator();
		expo->setAgent(ag);
		ag->addSimulator(expo);
	}
	/// add metabolism simulator
	MetabolismForBlindAgentSimulator *sim2 = new MetabolismForBlindAgentSimulator(ag);
	ag->addSimulator(sim2);


	/// add birth simulator
	BirthSimulator *sim3 = new BirthSimulator(ag);
	ag->addSimulator(sim3);

	/// add death simulator
	DeathSimulator *sim4 = new DeathSimulator(ag);
	ag->addSimulator(sim4);


	/// must add time keeper before birth simulator!!! so that time is incremented before birth!
	TimeKeeper *ctk = new TimeKeeper(ag,"celltime");
	ag->addSimulator(ctk);


}
