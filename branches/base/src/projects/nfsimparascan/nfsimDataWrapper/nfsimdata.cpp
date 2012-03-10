#include "nfsimdata.hh"

using namespace Hive;
using namespace nfsimparascan;

NFsimData::NFsimData(string n):Data(n, "nfsimdatatype") {
	this->sys = 0;
}

NFsimData::~NFsimData() {}

void NFsimData::printContent(ostream& out) {
	throw HiveException("# error: not implemented", "# in NFsimData::printContent(ofstream& out)");
}

void NFsimData::packDataForMPI(char*& buffer, int& size) {
	throw HiveException("# error: not implemented", "# in NFsimData::packDataForMPI(char*& buffer,int& size)");
}

void NFsimData::unpackDataForMPI(char* buffer, int size) {
	throw HiveException("# error: not implemented", "# in NFsimData::unpackDataForMPI(char* buffer,int size)");
}

System* NFsimData::getNFsimSystem() {
	return this->sys;
}

void NFsimData::setNFsimSystem(System *s) {
	this->sys = s;
}

void NFsimData::copyContentsInto(Data *destination) {
	 throw(HiveException("Cannot copy data item, because copyContents is not implemented for this Data",
			 "Data::copyContents(Data *)"));
};
