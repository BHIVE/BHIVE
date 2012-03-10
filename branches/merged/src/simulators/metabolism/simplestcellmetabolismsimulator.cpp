#include "simplestcellmetabolismsimulator.hh"

using namespace Hive;
using namespace std;

SimplestCellMetabolismSimulator::SimplestCellMetabolismSimulator(Agent *ag) : Simulator() {
	this->agent = ag;
	this->db = ag->getDatabase();
}

SimplestCellMetabolismSimulator::~SimplestCellMetabolismSimulator() { }


void SimplestCellMetabolismSimulator::initialise() {
	this->ligands = (TVectorData<double>* ) db->getDataItem("ligands");
	this->appetite = (TVectorData<double>* ) db->getDataItem("appetite");
	this->consumption_rates = (TVectorData<double>* ) db->getDataItem("consumption_rates");
}


void SimplestCellMetabolismSimulator::step(double dt) {
	// just iterating over all the seen ligands and calculating the dLigand
	// result is stored in appetite vector
	for (int i=0; i<this->ligands->size(); i++)
		this->appetite->at(i) = dt * this->consumption_rates->at(i)*this->ligands->at(i);
	// irrespective of the appetite the cell has we still need to subtract a beta term from it!

}


void SimplestCellMetabolismSimulator::prepare() { }


void SimplestCellMetabolismSimulator::synchroniseWithDatabase() { }


void SimplestCellMetabolismSimulator::setAgent(Agent *ag) {
	this->agent = ag;
	this->db = ag->getDatabase();
	this->initialise();
}
