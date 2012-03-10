/*
 * parameterData.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */


#include "parameterData.hh"
#include "../../../../util/util.hh"

using namespace NF;


NF_ParamaterData::NF_ParamaterData(string parameterName, double value) :
		Data(parameterName,"NF_ParameterData")
{
	this->value=value;
}

NF_ParamaterData::~NF_ParamaterData()
{

}

void NF_ParamaterData::printContent(ostream &out)
{
	out<<"NFsim Parameter: "+this->dataName+" = "+Util::toString(value);
}

inline
double NF_ParamaterData::getValue() const
{
	return value;
}

inline
void NF_ParamaterData::setValue(double value)
{
	this->value=value;
}
