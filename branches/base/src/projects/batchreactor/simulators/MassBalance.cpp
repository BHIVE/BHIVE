#include "MassBalance.hh"

using namespace Hive;
using namespace BatchReactor;

MassBalance::MassBalance(Agent *a) {
	this->agent = a;
	this->db    = a->getDatabase();
	this->prepare();
}

MassBalance::~MassBalance() {}

void MassBalance::step(double dt) {
	IntegerVectorData* agBoundaryIndecies = (IntegerVectorData*)this->getDataFromAgent(this->boundaryIndecies);
	DoubleVectorData* agBoundaryArray = (DoubleVectorData*)this->getDataFromAgent(this->boundaryArray);
	DoubleVectorData* agSubstrateConcentrationArray = (DoubleVectorData*)this->getDataFromAgent(this->substrateConcentrationArray);
	DoubleData* agCellVolume = (DoubleData*)this->getDataFromAgent(this->cellVolume);
	for (int i=0; i < agBoundaryIndecies->getNumElements(); i++) {
		double conc = agSubstrateConcentrationArray->getDouble(agBoundaryIndecies->getInteger(i));
		conc = (agCellVolume->getDouble()*conc+agBoundaryArray->getDouble(i))/agCellVolume->getDouble();
		agSubstrateConcentrationArray->setDouble(conc,agBoundaryIndecies->getInteger(i));
	}
}

//TODO: should be in core
Data* MassBalance::getDataFromAgent(int dataIndex) {
	return ag->getDatabase()->getDataItem(this->dataIndecies[dataIndex]);
}

void MassBalance::setLinksToAgentDB(vector<string> dataNamesInput) {
	this->dataNames = dataNamesInput;
	this->dataIndecies.resize(dataNamesInput.size());
}

void MassBalance::setAgent(Agent *ag) {
	this->agent = ag;
	for (int i=0; i < this->dataNames.size();i++) {
		this->dataIndecies[i] = ag->getDatabase()->getDataIndex(this->dataNames[i]);
	}
	this->prepare();
};

void MassBalance::initialise() {
	this->prepare();
}
void MassBalance::prepare(){};
void MassBalance::synchroniseWithDatabase(){};
