


#include "mappingSet.hh"


using namespace NFcore;




MappingSet::MappingSet(unsigned int id, vector <Transformation *> &transformations)
{
	this->id = id;
	this->n_mappings = transformations.size();
	this->mappings = new Mapping *[n_mappings];
	this->isDeletion=false;
	this->clonedMappingSet=MappingSet::NO_CLONE;

	for(unsigned int t=0; t<n_mappings; t++) {
		if(transformations.at(t)->getType()==(int)TransformationFactory::REMOVE) {
			this->isDeletion=true;
			mappings[t] = new Mapping(transformations.at(t)->getType(), -1 );
		}
		else
			mappings[t] = new Mapping(transformations.at(t)->getType(), transformations.at(t)->getComponentIndex() );
	}
}
MappingSet::~MappingSet()
{
	for(unsigned int t=0; t<n_mappings; t++) {
		delete mappings[t];
	}
	delete [] mappings;
}

void MappingSet::clear() {
	this->clonedMappingSet=NO_CLONE;
	for(unsigned int t=0; t<n_mappings; t++) {
		mappings[t]->clear();
	}

}



bool MappingSet::set(unsigned int mappingIndex, Molecule *m)
{
	mappings[mappingIndex]->setMolecule(m);
	return true;
}


void MappingSet::clone(MappingSet *original, MappingSet *newClone)
{
	if(original->clonedMappingSet!=MappingSet::NO_CLONE) {
		cerr<<"Error in MappingSet!  Trying to clone a MappingSet that already has a clone!"<<endl;
		exit(10);
	}

	for(unsigned int i=0; i<original->n_mappings; i++) {
		Mapping::clone(original->mappings[i],newClone->mappings[i]);
	}
	original->clonedMappingSet=newClone->id;
}




void MappingSet::printDetails() const {
	printDetails(cout);
}

void MappingSet::printDetails(ostream &o) const
{
	o<<"MappingSet "<<id<<": has "<<n_mappings<<" mapping(s),  ";
	if(isDeletion) o<<"  and this is a deletion Mapping.";
	o<<"\n";
	o<<"  clone of this MappingSet: ";
	if(clonedMappingSet==MappingSet::NO_CLONE) o<<"none.\n";
	else o<<clonedMappingSet<<"\n";

	for(unsigned int i=0; i<n_mappings; i++) {
		o<<"  >";
		mappings[i]->printDetails(o); o<<"\n";
	}

}







// These functions defined inline with no checking in this faster version
//bool NFcore::MappingSet::set(unsigned int mappingIndex, Molecule *m)
//{
//	mappings[mappingIndex]->setMolecule(m);
//	return true;
//}
//Mapping *NFcore::MappingSet::get(unsigned int mappingIndex)
//{
//	return mappings[mappingIndex];
//}
//bool NFcore::MappingSet::clear()
//{
//	return true;
//}






//////////////////////
//  Below are the (very) slightly slower version of the above functions.  They
//  are essential for debugging because they make sure the MappingSet object is
//  properly set before it is used.  It also explicitly clears the Mappings below.
/*
NFcore::MappingSet::MappingSet(unsigned int id, vector <Transformation *> &transformations)
{
	this->id = id;
	this->isSet = false;
	this->n_mappings = transformations.size();
	this->mappings = new Mapping *[n_mappings];

	for(unsigned int t=0; t<n_mappings; t++) {
		mappings[t] = new Mapping(transformations.at(t)->getType(), transformations.at(t)->getStateOrSiteIndex() );
	}
}
NFcore::MappingSet::~MappingSet()
{
	for(unsigned int t=0; t<n_mappings; t++) {
		delete mappings[t];
	}
	delete [] mappings;
}

bool NFcore::MappingSet::set(unsigned int mappingIndex, Molecule *m)
{
	if(mappingIndex>=n_mappings) {
		cerr<<"Out of bounds access to a mapping in set function of mapping set!"<< mappingIndex<<" but max is " << n_mappings<<endl;
		return false;
	}
	mappings[mappingIndex]->setMolecule(m);
	isSet = true;
	return true;
}
Mapping *NFcore::MappingSet::get(unsigned int mappingIndex)
{
	if(mappingIndex>=n_mappings) {
		cerr<<"Out of bounds access to a mapping in get function of a mapping set!"<<endl;
		return false;
	}
	return mappings[mappingIndex];
}
bool NFcore::MappingSet::clear()
{
	isSet = false;

	//This is not entirely necessary, but for now can be used for debugging
	for(unsigned int t=0; t<n_mappings; t++) {
		mappings[t]->clear();
	}

	return true;
}
*/




