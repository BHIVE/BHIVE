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

using namespace Hive;


IntegerData::IntegerData(string dataName, int value) : Data(dataName,"IntegerType") {
	this->integerValue=value;
}

void IntegerData::printContent(ostream &out)
{
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<integerValue << endl;
}





DoubleData::DoubleData(string dataName, double value) : Data(dataName,"DoubleType") {
	this->doubleValue=value;
}
void DoubleData::printContent(ostream &out)
{
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<doubleValue << endl;
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
void BoolData::printContent(ostream &out)
{
	out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" value="<<boolValue << endl;
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


DoubleVectorData::DoubleVectorData(string dataName, int number_of_elements) : Data (dataName,"DoubleVectorType"){
	this->vec.resize(number_of_elements);
}

void DoubleVectorData::printContent(ostream &out) {
	out << "[DATA] type=\""<<datatype << "\" name=\""<<dataName<<"\" value=";
	for (unsigned int i=0; i<vec.size(); i++)
		out << this->vec[i] << " ";
	out << endl;
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
