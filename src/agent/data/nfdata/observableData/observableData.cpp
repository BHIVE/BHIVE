/*
 * observableData.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */






#include "observableData.hh"

using namespace NF;


NF_ObservableData::NF_ObservableData(Observable *ob, string observableName) :
		Data(observableName,"NF_ObservableData")
{
	this->obs=ob;
}

NF_ObservableData::~NF_ObservableData()
{

}

void NF_ObservableData::printContent(ostream &out)
{
	out<<"NF_ObservableData::printContent() = ";
	out<<obs->getName()<<" "<<obs->getType()<<endl;
}


Observable * NF_ObservableData::getObservable() const
{
	return obs;
}


string NF_ObservableData::getObservableName() const
{
	return obs->getName();
}


int NF_ObservableData::getObservableValue() const
{
	return obs->getCount();
}


