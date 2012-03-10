/*
 * chemotaxisAction.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: msneddon
 */



#include "chemotaxisAction.hh"
#include "../../messageGenerator/chemotaxisMessageGenerator/chemotaxisMessageGenerator.hh"
#include "../../data/primitive/primitiveData.hh"
#include "../../../exception/exception.hh"

using namespace Chemotaxis;
using namespace Hive;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

OverwritePositionAction::OverwritePositionAction() :
		Action(ChemotaxisActionIDs::OVERWRITE_POSITION_ACTION_ID)
{
	agent=NULL;
}

OverwritePositionAction::~OverwritePositionAction()
{
}


void OverwritePositionAction::fire(Data *d)
{
	if(this->agent==NULL)
		throw HiveException("Action was never assigned an agent.", "OverwritePositionAction::fire(Data *d)");
	if(d->getType().compare("DoubleVectorType")!=0)
		throw HiveException("Data passed to fire is not of type DoubleVectorType.", "OverwritePositionAction::fire(Data *d)");

	DoubleVectorData *newDvd=(DoubleVectorData *)d;

	DoubleVectorData *dvd=(DoubleVectorData *)agent->getDatabase()->getDataItem("Position");
	int X=0, Y=1, Z=2;
	dvd->setDouble(newDvd->getDouble(X),X+1);
	dvd->setDouble(newDvd->getDouble(Y),Y+1);
	dvd->setDouble(newDvd->getDouble(Z),Z+1);

	//cout<<"firing!"<<endl;
	//dvd->printContent(cout);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


UpdateSeenChemicalConcentrationAction::UpdateSeenChemicalConcentrationAction() :
	Action(ChemotaxisActionIDs::UPDATE_SEEN_CHEMICAL_CONCENTRATION_ACTION_ID)
{
	agent=NULL;
}

UpdateSeenChemicalConcentrationAction::~UpdateSeenChemicalConcentrationAction()
{}

void UpdateSeenChemicalConcentrationAction::fire(Data *d)
{
	if(this->agent==NULL)
		throw HiveException("Action was never assigned an agent.", "UpdateSeenChemicalConcentrationAction::fire(Data *d)");
	if(d->getType().compare("DoubleVectorType")!=0)
		throw HiveException("Data passed to fire is not of type DoubleVectorType.", "UpdateSeenChemicalConcentrationAction::fire(Data *d)");

	DoubleVectorData *newDvd=(DoubleVectorData *)d;

	DoubleVectorData *dvd=(DoubleVectorData *)agent->getDatabase()->getDataItem("Chemical");
	DoubleVectorData *dvd_old = (DoubleVectorData *)agent->getDatabase()->getDataItem("LastChemical");
	// copy current chemical on to lastchemical ...
	dvd_old->setDouble(newDvd->getDouble(0),0);
	dvd_old->setDouble(newDvd->getDouble(1),1);

	dvd->setDouble(newDvd->getDouble(0),0);
	dvd->setDouble(newDvd->getDouble(1),1);
	dvd->setDouble(newDvd->getDouble(2),2);
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


UpdateMotorConformationAction::UpdateMotorConformationAction() :
		Action(ChemotaxisActionIDs::UPDATE_MOTOR_CONFORMATION_ACTION_ID)
{
	agent=NULL;
}


/// destructor
UpdateMotorConformationAction::~UpdateMotorConformationAction()
{

}

void UpdateMotorConformationAction::fire(Data *d)
{
	if(this->agent==NULL)
		throw HiveException("Action was never assigned an agent.", "UpdateSeenChemicalConcentrationAction::fire(Data *d)");
	if(d->getType().compare("IntVectorType")!=0)
		throw HiveException("Data passed to fire is not of type DoubleVectorType.", "OverwritePositionAction::fire(Data *d)");

	IntVectorData *newIvd=(IntVectorData *)d;

	IntegerData *idCW=(IntegerData *)agent->getDatabase()->getDataItem("NumberCW_Motors");
	IntegerData *idCCW=(IntegerData *)agent->getDatabase()->getDataItem("NumberCCW_Motors");
	idCW->setInteger(newIvd->getInt(0));
	idCCW->setInteger(newIvd->getInt(1));
}
