/*
 * moleculeTypeData.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */



#include "moleculeTypeData.hh"

using namespace NF;


NF_MoleculeTypeData::NF_MoleculeTypeData(MoleculeType *mt, string moleculeTypeName) :
	Data(moleculeTypeName,"NF_MoleculeTypeData")
{
	this->mt=mt;
}

NF_MoleculeTypeData::~NF_MoleculeTypeData()
{

}

void NF_MoleculeTypeData::printContent(ostream &out)
{

}

inline
MoleculeType * NF_MoleculeTypeData::getMoleculeType() const
{
	return mt;
}

inline
string NF_MoleculeTypeData::getMoleculeTypeName() const
{
	return mt->getName();
}

