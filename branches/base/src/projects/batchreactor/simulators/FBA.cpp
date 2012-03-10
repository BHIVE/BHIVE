#include "FBA.hh"

using namespace Hive;
using namespace BatchReactor;

FBA::FBA(Agent *a) {
	this->agent = a;
	this->db    = a->getDatabase();
	this->prepare();
}

FBA::~FBA() {}

void FBA::step(double dt) {
	//TODO:
	IntegerData* agBiomassIndex = (IntegerData*)this->getDataFromAgent(this->biomassIndex);
	DoubleVectorData* agSubstrateConcentrationArray = (DoubleVectorData*)this->getDataFromAgent(this->substrateConcentrationArray);
	DoubleData* agMaxBiomass = (DoubleData*)this->getDataFromAgent(this->maxBiomass);
	IntegerData* agDivision = (IntegerData*)this->getDataFromAgent(this->division);
	DoubleData* agCellVolume = (DoubleData*)this->getDataFromAgent(this->cellVolume);
	double biomass = agSubstrateConcentrationArray->getDouble(agBiomassIndex->getInteger());
	if (agDivision->getInteger() == 1) {
		biomass = biomass/2;
		agSubstrateConcentrationArray->setDouble(biomass,agBiomassIndex->getInteger());
		agDivision->setInteger(0);
	}
	//TODO: need to have a coefficient linking biomass to volume
	agCellVolume->setDouble(biomass);
	divide->setInteger(cellDivision(biomass->getDouble(),maxBiomass->getDouble()));
	if(biomass > agMaxBiomass->getDouble()){
		agDivision->setInteger(1);
	}
}

//TODO: should be in core
Data* FBA::getDataFromAgent(int dataIndex) {
	return ag->getDatabase()->getDataItem(this->dataIndecies[dataIndex]);
}

void FBA::setLinksToAgentDB(vector<string> dataNamesInput) {
	this->dataNames = dataNamesInput;
	this->dataIndecies.resize(dataNamesInput.size());
}

void FBA::setAgent(Agent *ag) {
	this->agent = ag;
	for (int i=0; i < this->dataNames.size();i++) {
		this->dataIndecies[i] = ag->getDatabase()->getDataIndex(this->dataNames[i]);
	}
	this->prepare();
};

void FBA::initialise() {
	this->prepare();
}
void FBA::prepare(){};
void FBA::synchroniseWithDatabase(){};
