#include "MichealisMenten.hh"

using namespace Hive;
using namespace BatchReactor;

MichealisMenten::MichealisMenten(Agent *a) {
	this->agent = a;
	this->db    = a->getDatabase();
	this->prepare();
}

MichealisMenten::~MichealisMenten() {}

void MichealisMenten::step(double dt) {
	DoubleVectorData* ks = (DoubleVectorData*)this->getDataFromAgent(this->ksArray);
	DoubleVectorData* vmax = (DoubleVectorData*)this->getDataFromAgent(this->vmaxArray);
	DoubleVectorData* substrate = (DoubleVectorData*)this->getDataFromAgent(this->substrateArray);
	DoubleVectorData* rate = (DoubleVectorData*)this->getDataFromAgent(this->rateArray);
	for (int i=0; i < ks->getNumElements(); i++) {
		rate->setDouble(michealisMentenEquation(ks->getDouble(i),vmax->getDouble(i),substrate->getDouble(i)),i);
	}
}

//TODO: will be in core
Data* getDataFromAgent(int dataIndex) {
	return ag->getDatabase()->getDataItem(this->dataIndecies[dataIndex]);
}

void MichealisMenten::setLinksToAgentDB(vector<string> dataNamesInput) {
	this->dataNames = dataNamesInput;
	this->dataIndecies.resize(dataNamesInput.size());
}

double MichealisMenten::michealisMentenEquation(double ks,double vmax,double substrate) {
	return vmax*substrate/(substrate+ks);
}

void MichealisMenten::prepare(){};

void MichealisMenten::synchroniseWithDatabase(){};

void MichealisMenten::setAgent(Agent *ag) {
	this->agent = ag;
	for (int i=0; i < this->dataNames.size();i++) {
		this->dataIndecies[i] = ag->getDatabase()->getDataIndex(this->dataNames[i]);
	}
	this->prepare();
};

void MichealisMenten::initialise() {
	this->prepare();
}

