/**
 * @file primitiveData.cpp
 *
 * this file contains the interfaces to the classes Data, ...
 *
 *
 * @date Oct 14, 2009  Last edited: Oct 14, 2009
 *
 * @author Michael Sneddon
 */

#include "primitiveData.hh"
#include "../../../util/util.hh"

using namespace Hive;


IntegerData::IntegerData(string dataName, int value) : Data(dataName,"IntegerType") {
	this->integerValue=value;
}
void IntegerData::printContent(ostream &out) {
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<integerValue << endl;
}
void IntegerData::printValue(ostream &out) {
	out<<integerValue;
}
void IntegerData::packDataForMPI(char*& buffer,int& size){
	int* temp = new int[1];
	temp[0] = this->integerValue;
	buffer = (char*)temp;
	size = sizeof(int);
}
void IntegerData::unpackDataForMPI(char* buffer,int size){
	int* temp = (int*)buffer;
	this->integerValue = temp[0];
	delete [] buffer;
}
void IntegerData::initializeFromString(const string s){
	try {
		this->integerValue = Util::convertToInt(s);
	} catch (HiveException& e) {
		e.addTraceException("IntegerData::initializeFromString(const string s)");
		throw e;
	}
}








DoubleData::DoubleData(string dataName, double value) : Data(dataName,"DoubleType") {
	this->doubleValue=value;
}

void DoubleData::printContent(ostream &out) {
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<doubleValue << endl;
}
void DoubleData::printValue(ostream &out) {
	out<<doubleValue;
}
void DoubleData::packDataForMPI(char*& buffer,int& size) {
	double* temp = new double[1];
	temp[0] = this->doubleValue;
	buffer = (char*)temp;
	size = sizeof(double);
}
void DoubleData::unpackDataForMPI(char* buffer,int size) {
	double* temp = (double*)buffer;
	this->doubleValue = temp[1];
	delete [] buffer;
}
void DoubleData::initializeFromString(const string s) {
	try {
		this->doubleValue = Util::convertToDouble(s);
	} catch (HiveException& e) {
		e.addTraceException("DoubleData::initializeFromString(const string s)");
		throw e;
	}
}




DoublePtrData::DoublePtrData(string dataName, double *ptr) : Data(dataName,"DoublePtrType") {
	this->double_ptr = ptr;
}
void DoublePtrData::printContent(ostream& out) {
	out << "[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<*double_ptr << endl;
}



BoolData::BoolData(string dataName, bool value) : Data(dataName,"BoolType") {
	this->boolValue=value;
}
void BoolData::printContent(ostream &out) {
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<boolValue << endl;
}
void BoolData::printValue(ostream &out) {
	if(boolValue)out<<"true"; else out<<"false";
}
void BoolData::packDataForMPI(char*& buffer,int& size) {
	bool* temp = new bool[1];
	temp[0] = this->boolValue;
	buffer = (char*)temp;
	size = sizeof(bool);
}
void BoolData::unpackDataForMPI(char* buffer,int size) {
	bool* temp = (bool*)buffer;
	this->boolValue = temp[1];
	delete [] buffer;
}
void BoolData::initializeFromString(const string s) {
	string localString = s;
	transform(localString.begin(), localString.end(), localString.begin(), ::tolower);
	if(localString=="true") {
		this->boolValue = true;
	} else if (localString=="false"){
		this->boolValue = false;
	} else {
		throw HiveException("# ERROR! : Cannot parse boolean value (must be a string of either 'true' or 'false').", "BoolData::initializeFromString(const string s) ");
	}
}


StringData::StringData(string dataName, string value) : Data(dataName, "StringType") {
	this->stringValue = value;
}
void StringData::printContent(ostream &out) {
	out << "[DATA] type=\"" << datatype << "\" name = \"" << dataName << "\" value = " << stringValue << endl;
}
void StringData::printValue(ostream &out) {
	out<<"'"<<stringValue<<"'";
}
void StringData::initializeFromString(const string s) {
	this->stringValue = s;
}

IntVectorData::IntVectorData(string dataName, int number_of_elements) : Data(dataName,"IntVectorType") {
	this->vec.resize(number_of_elements);
}
void IntVectorData::printContent(ostream &out) {
	out << "[DATA] type=\""<<datatype << "\" name=\""<<dataName<<"\" value=";
	for (unsigned int i=0; i<vec.size(); i++)
		out << this->vec[i] << " ";
	out << endl;
}
void IntVectorData::packDataForMPI(char*& buffer,int& size)
{
	int* temp = new int[this->vec.size()];
	for (int i=0; i < int(this->vec.size()); i++) {
		temp[i] = this->vec[i];
	}
	buffer = (char*)temp;
	size = this->vec.size()*sizeof(int);
}
void IntVectorData::unpackDataForMPI(char* buffer,int size)
{
	int* temp = (int*)buffer;
	size = size/sizeof(int);
	this->vec.clear();
	for (int i=0; i < size; i++) {
		this->vec.push_back(temp[i]);
	}
	delete [] buffer;
}






DoubleVectorData::DoubleVectorData(string dataName, int number_of_elements) : Data (dataName,"DoubleVectorType"){
	this->vec.resize(number_of_elements);
}
void DoubleVectorData::printContent(ostream &out) {
	out << "[DATA] type=\""<<datatype << "\" name=\""<<dataName<<"\" value=";
	for (unsigned int i=0; i<vec.size(); i++)
		out << this->vec[i] << " ";
	out << endl;
}
void DoubleVectorData::packDataForMPI(char*& buffer,int& size) {
	double* temp = new double[this->vec.size()];
	for (int i=0; i < int(this->vec.size()); i++) {
		temp[i] = this->vec[i];
//		cerr << this->vec[i] << endl;
	}
	buffer = (char*)temp;
	size = this->vec.size()*sizeof(double);
}
void DoubleVectorData::unpackDataForMPI(char* buffer,int size) {
//	cerr << "in unpacking of data" << endl;
	double* temp = (double*)buffer;
	size = size/sizeof(double);
	this->vec.clear();
	this->vec.resize(size);
	for (int i=0; i < size; i++) {
		this->vec[i] = temp[i];
//		cerr << this->vec[i] << endl;
	}
	delete [] buffer;
}




DoubleMatrixData::DoubleMatrixData(string dataName, int xdim, int ydim) : Data(dataName, "DoubleMatrixType") {
	this->mat.resize(xdim);
	for (int i=0; i<xdim; i++)
		mat[i].resize(ydim);
}

void DoubleMatrixData::printContent(ostream& out) {
	for (unsigned int i=0; i<mat.size(); i++) {
		for (unsigned int j=0; j<mat[i].size(); j++)
			out << mat[i][j] << " ";
		out << endl;
	}
}
void DoubleMatrixData::setDimensions(int xdim, int ydim) {
	this->mat.resize(xdim);
	for (int i=0; i<xdim; i++)
		mat[i].resize(ydim);
}
void DoubleMatrixData::packDataForMPI(char*& buffer,int& size)
{
	double* temp = new double[2+this->mat.size()*this->mat[0].size()];
	temp[0] = this->mat.size();
	temp[1] = this->mat[0].size();
	int count = 2;
	for (int i=0; i < int(this->mat.size()); i++) {
		for (int j=0; j < int(this->mat[i].size()); j++) {
			temp[count] = this->mat[i][j];
			count++;
		}
	}
	buffer = (char*)temp;
	size = (2+this->mat.size()*this->mat[0].size())*sizeof(double);
}
void DoubleMatrixData::unpackDataForMPI(char* buffer,int size)
{
	double* temp = (double*)buffer;
	double xdim = temp[0];
	double ydim = temp[1];
	this->mat.clear();
	this->mat.resize(xdim);
	int count = 2;
	for (int i=0; i < xdim; i++) {
		this->mat[i].resize(ydim);
		for (int j=0; j < ydim; j++) {
			this->mat[i][j] = temp[count];
			count++;
		}
	}
	delete [] buffer;
}






RawBufferData::RawBufferData(string dataname, char *buf, int size) : Data(dataname, "RawBufferData") {
	this->size = size;
	this->buffer = buf;
}
