#include "movementdecisionsimulator.hh"

using namespace std;
using namespace Hive;
using namespace Heatbug;


MovementDecisionSimulator::MovementDecisionSimulator() {
	this->dt                    = 0;
	this->preferred_temperature = 0;
	local_temperature_around_me = NULL;
	move_direction              = NULL;
}

MovementDecisionSimulator::~MovementDecisionSimulator() { }

// a bug moves randomly about the grid with a preference for that neighboring
// site which has a temperature closest to the bug's prefered temperature.
void MovementDecisionSimulator::step(double t) {
	/*/ local variables

	int result;
	double total_weight, rand;
	vector<double> temp_diff(5);

	// calculate the inverse of the temperature differences
	temp_diff[0] = 1./abs(local_temperature_around_me->at(0).getDouble()-preferred_temperature);
	temp_diff[1] = 1./abs(local_temperature_around_me->at(1).getDouble()-preferred_temperature);
	temp_diff[2] = 1./abs(local_temperature_around_me->at(2).getDouble()-preferred_temperature);
	temp_diff[3] = 1./abs(local_temperature_around_me->at(3).getDouble()-preferred_temperature);
	temp_diff[4] = 1./abs(local_temperature_around_me->at(4).getDouble()-preferred_temperature);

	// calculate the total weight
	total_weight = temp_diff[0]+temp_diff[1]+temp_diff[2]+temp_diff[3]+temp_diff[4];

	// draw a random number
	rand = Util::RANDOM(total_weight);

	// decide which direction the bug is moving
	// note that i unroll the loop here.
	rand-= temp_diff[0];
	if (rand < 0)
		result = 0;
	else {
		rand -= temp_diff[1];
		if (rand < 0)
			result = 1;
		else {
			rand -= temp_diff[2];
			if (rand < 0)
				result = 2;
			else {
				rand -= temp_diff[3];
				if (rand < 0)
					result = 3;
				else
					result = 4;
			}
		}
	}

/*/
	PyObject*    main_module, *global_dict, *expression;
	int result;
	char *cProg = (char*)this->pythonProgram.c_str();
	  PyRun_SimpleString(cProg);
	  main_module = PyImport_AddModule("__main__");
	  global_dict = PyModule_GetDict(main_module);
	  expression = PyDict_GetItemString(global_dict, "move");
	  PyObject *pResult = PyObject_CallFunction(expression,"dddddd",
			  preferred_temperature,
			  local_temperature_around_me->at(0).getDouble(),
			  local_temperature_around_me->at(1).getDouble(),
			  local_temperature_around_me->at(2).getDouble(),
			  local_temperature_around_me->at(3).getDouble(),
			  local_temperature_around_me->at(4).getDouble());
	  PyArg_Parse( pResult, "i", &result );
	// store the result in the correct variable.
	this->move_direction->setInteger(result);
}

void MovementDecisionSimulator::prepare() {
	Database *db = this->agent->getDatabase();
	this->local_temperature_around_me = (TVectorData<DoubleData>*) db->getDataItem("local_temperature_around_me");
	this->preferred_temperature       = ((DoubleData*) db->getDataItem("preferred_temperature"))->getDouble();
	this->move_direction              = (IntegerData *) db->getDataItem("move_direction");
	this->pythonProgram               = ((StringData *) db->getDataItem("pythonProgram"))->getString();
}

void MovementDecisionSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->prepare();
}

void MovementDecisionSimulator::setTimeStep(double t) {
	this->dt = t;
}

void MovementDecisionSimulator::synchroniseWithDatabase() {}

void MovementDecisionSimulator::initialise() { }
