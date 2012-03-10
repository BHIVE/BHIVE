

#include "transformation.hh"

using namespace NF;



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
StateChangeTransform::StateChangeTransform(int cIndex, int newValue) :
	Transformation(TransformationFactory::STATE_CHANGE)
{
	this->cIndex = cIndex;
	this->newValue = newValue;
}
void StateChangeTransform::apply(Mapping *m, MappingSet **ms)
{
	m->getMolecule()->setComponentState(cIndex,newValue);
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
IncrementStateTransform::IncrementStateTransform(unsigned int cIndex) :
	Transformation(TransformationFactory::INCREMENT_STATE)
{
	this->cIndex = cIndex;
}
void IncrementStateTransform::apply(Mapping *m, MappingSet **ms)
{
	int oldValue = m->getMolecule()->getComponentState(cIndex);
	m->getMolecule()->setComponentState(cIndex,oldValue+1);
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
DecrementStateTransform::DecrementStateTransform(unsigned int cIndex) :
	Transformation(TransformationFactory::DECREMENT_STATE)
{
	this->cIndex = cIndex;
}
void DecrementStateTransform::apply(Mapping *m, MappingSet **ms)
{
	int oldValue = m->getMolecule()->getComponentState(cIndex);
	m->getMolecule()->setComponentState(cIndex,oldValue-1);
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
BindingTransform::BindingTransform(int cIndex, int otherReactantIndex, int otherMappingIndex) :
	Transformation(TransformationFactory::BINDING)
{
	this->cIndex=cIndex;
	this->otherReactantIndex=otherReactantIndex;
	this->otherMappingIndex=otherMappingIndex;
}

void BindingTransform::apply(Mapping *m, MappingSet **ms)
{
	Mapping *m2 = ms[this->otherReactantIndex]->get(this->otherMappingIndex);
	//Currently, this is set to block all binding events that happen internally to a single
	//molecule.  I think this is reasonable to do...
	if(m->getMolecule()->getUniqueID()==m2->getMolecule()->getUniqueID()) { // && m->getIndex() == m2->getIndex()) {
		System::NULL_EVENT_COUNTER++;
	} else {
		Molecule::bind(m->getMolecule(),m->getIndex(), m2->getMolecule(), m2->getIndex());
	}

}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////


void BindingSeparateComplexTransform::apply(Mapping *m, MappingSet **ms)
{
	Mapping *m2 = ms[this->otherReactantIndex]->get(this->otherMappingIndex);
	//cout<<"complex ID: "<<m->getMolecule()->getComplexID()<<" "<<m2->getMolecule()->getComplexID()<<endl;

	if(m->getMolecule()->getComplexID()!=m2->getMolecule()->getComplexID()) {
		Molecule::bind(m->getMolecule(),m->getIndex(), m2->getMolecule(), m2->getIndex());
	} else {
		System::NULL_EVENT_COUNTER++;
	}
}


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
UnbindingTransform::UnbindingTransform(int cIndex) :
	Transformation(TransformationFactory::UNBINDING)
{
	this->cIndex=cIndex;
}
void UnbindingTransform::apply(Mapping *m, MappingSet **ms)
{   //cout<<"unbinding.."<<endl;
	Molecule::unbind(m->getMolecule(),m->getIndex());
}






///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
AddMoleculeTransform::AddMoleculeTransform(SpeciesCreator *sc) :
	Transformation(TransformationFactory::ADD)
{
	this->sc=sc;
}
AddMoleculeTransform::~AddMoleculeTransform()
{
	delete sc;
}
void AddMoleculeTransform::apply(Mapping *m, MappingSet **ms)
{
	this->sc->create();
}



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
RemoveMoleculeTransform::RemoveMoleculeTransform(int removalType) :
	Transformation(TransformationFactory::REMOVE) {
	this->removalType=removalType;
}


void RemoveMoleculeTransform::apply(Mapping *m, MappingSet **ms)
{
	cout<<"!! Warning: calling apply on a RemoveMoleculeTransform!  This cannot be handled here!"<<endl;
	cout<<"!! This function should not be called.  The TransformationSet object should handle this!"<<endl;
}


/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
LocalFunctionReference::LocalFunctionReference(string PointerName, int scope, TemplateMolecule *tm)
	: Transformation(TransformationFactory::LOCAL_FUNCTION_REFERENCE) {

	this->PointerName=PointerName;
	this->scope=scope;
	this->tm=tm;

}




///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
NF::Transformation * TransformationFactory::genEmptyTransform()
{
	return new EmptyTransform();
}
NF::Transformation * TransformationFactory::genStateChangeTransform(unsigned int stateIndex, int newStateValue)
{
	return new StateChangeTransform(stateIndex, newStateValue);
}
NF::Transformation * TransformationFactory::genBindingTransform1(unsigned int bSiteIndex, unsigned int otherReactantIndex, unsigned int otherMappingIndex)
{
	return new BindingTransform(bSiteIndex, otherReactantIndex, otherMappingIndex);
}
NF::Transformation * TransformationFactory::genBindingSeparateComplexTransform1(unsigned int bSiteIndex, unsigned int otherReactantIndex, unsigned int otherMappingIndex)
{
	return new BindingSeparateComplexTransform(bSiteIndex, otherReactantIndex, otherMappingIndex);
}

NF::Transformation * TransformationFactory::genBindingTransform2(unsigned int bSiteIndex)
{
	return new EmptyTransform(bSiteIndex);
}
NF::Transformation * TransformationFactory::genUnbindingTransform(unsigned int bSiteIndex)
{
	return new UnbindingTransform(bSiteIndex);
}
NF::Transformation * TransformationFactory::genAddMoleculeTransform(SpeciesCreator *sc)
{
	return new AddMoleculeTransform(sc);
}
NF::Transformation * TransformationFactory::genRemoveMoleculeTransform(int removalType)
{
	return new RemoveMoleculeTransform(removalType);
}


NF::Transformation * TransformationFactory::genIncrementStateTransform(unsigned int cIndex)
{
	return new IncrementStateTransform(cIndex);
}
NF::Transformation * TransformationFactory::genDecrementStateTransform(unsigned int cIndex)
{
	return new DecrementStateTransform(cIndex);
}

Transformation * TransformationFactory::genLocalFunctionReference(string PointerName, int type, TemplateMolecule *tm)
{
	return new LocalFunctionReference(PointerName, type, tm);
}












