#include <iostream>
#include "NFcore.hh"
#include <queue>


using namespace std;
using namespace NF;

int Molecule::uniqueIdCount = 0;





// Molecule Constructor
//
//
Molecule::Molecule(MoleculeType * parentMoleculeType, int listId)
{
	if(DEBUG) cout<<"-creating molecule instance of type " << parentMoleculeType->getName() << endl;
	this->parentMoleculeType = parentMoleculeType;

	//First initialize the component states and bonds
	this->numOfComponents = parentMoleculeType->getNumOfComponents();
	this->component = new int [parentMoleculeType->getNumOfComponents()];
	for(int c=0; c<numOfComponents; c++)
		component[c] = parentMoleculeType->getDefaultComponentState(c);

	this->bond = new Molecule * [numOfComponents];
	this->indexOfBond = new int [numOfComponents];
	this->hasVisitedBond = new bool [numOfComponents];
	for(int b=0; b<numOfComponents; b++) {
		bond[b]=0; indexOfBond[b]=NOBOND;
		hasVisitedBond[b] = false;
	}


	hasVisitedMolecule = false;
	hasEvaluatedMolecule = false;
	isMatchedTo=0;
	rxnListMappingId = 0;
	nReactions = 0;
	useComplex = parentMoleculeType->getSystem()->isUsingComplex();
	isPrepared = false;
	isObservable = 0;
	localFunctionValues=0;
	//isDead = true;

	//register this molecule with moleculeType and get some ID values
	ID_complex = this->parentMoleculeType->createComplex(this);
	ID_type = this->parentMoleculeType->getTypeID();
	ID_unique = Molecule::uniqueIdCount++;
	this->listId = listId;
	isAliveInSim = false;
}

// Molecule Deconstructor
//
//
Molecule::~Molecule()
{
	if(DEBUG) cout <<"   -destroying molecule instance of type " << parentMoleculeType->getName() << endl;
	delete [] bond;

	parentMoleculeType = 0;


	delete [] isObservable;
	delete [] component;
	delete [] indexOfBond;
	delete [] rxnListMappingId;
	delete [] hasVisitedBond;

	if(localFunctionValues!=0)
		delete [] localFunctionValues;
}


void Molecule::prepareForSimulation()
{
	if(isPrepared) return;
	nReactions = parentMoleculeType->getReactionCount();
	this->rxnListMappingId = new int[nReactions];
	for(int r=0; r<nReactions; r++)
		rxnListMappingId[r] = -1;
	isPrepared = true;

	//We do not belong to any observable... yet.
	isObservable=new int [parentMoleculeType->getNumOfMolObs()];
	for(int o=0;o<parentMoleculeType->getNumOfMolObs(); o++) {
		isObservable[o]=0;
	}


}


string Molecule::getMoleculeTypeName() const
{
	return parentMoleculeType->getName();
}
MoleculeType * Molecule::getMoleculeType() const
{
	return parentMoleculeType;
}
Complex * Molecule::getComplex() const
{
	return parentMoleculeType->getSystem()->getComplex(ID_complex);
}


void Molecule::setUpLocalFunctionList()
{
	localFunctionValues=new double[parentMoleculeType->getNumOfTypeIFunctions()];
	for(int lf=0; lf<parentMoleculeType->getNumOfTypeIFunctions(); lf++) {
		localFunctionValues[lf]=0;
	}
}



//Used so that this molecule can remember what its local function was
//evaluated to.  Only TypeI local functions are set up in this way
void Molecule::setLocalFunctionValue(double newValue,int localFunctionIndex) {
	if(localFunctionIndex<0 || localFunctionIndex>=parentMoleculeType->getNumOfTypeIFunctions()) {
		cout<<"Error in Molecule: trying to set the value of a local function, but the\n";
		cout<<"index provided was out of bounds!  I shall quit now."<<endl;
		exit(1);
	}
	localFunctionValues[localFunctionIndex] = newValue;
}


double Molecule::getLocalFunctionValue(int localFunctionIndex) {
	if(localFunctionIndex<0 || localFunctionIndex>=parentMoleculeType->getNumOfTypeIFunctions()) {
		cout<<"Error in Molecule: trying to set the value of a local function, but the\n";
		cout<<"index provided was out of bounds!  I shall quit now."<<endl;
		exit(1);
	}
	return localFunctionValues[localFunctionIndex];
}



void Molecule::updateRxnMembership()
{
	parentMoleculeType->updateRxnMembership(this);
}

void Molecule::updateTypeIIFunctions()
{
	//cout<<"exit!!"<<endl; exit(1);
	for(int i=0; i<parentMoleculeType->getNumOfTypeIIFunctions(); i++) {
		parentMoleculeType->getTypeIILocalFunction(i)->evaluateOn(this, LocalFunction::SPECIES);
	}
}
void Molecule::updateDORRxnValues()
{
	ReactionClass *rxn=0; int rxnIndex=-1, rxnPos=-1;
	//cout<<"Looping over :"<<parentMoleculeType->getNumOfDORrxns()<<" dor rxns "<<endl;
	for(int i=0; i<parentMoleculeType->getNumOfDORrxns(); i++) {
		rxn=parentMoleculeType->getDORrxn(i);
		rxnIndex=parentMoleculeType->getDORrxnIndex(i);
		rxnPos=parentMoleculeType->getDORrxnPosition(i);

		//cout<<"found DOR rxn: "<<r->getName()<<endl;

		if(isPrepared) {
			//If we are in this reaction, then we have to update our value...
			if(this->getRxnListMappingId(rxnIndex)>=0) {

				//Careful here!  remember to update the propensity of this
				//reaction in the system after we notify of the rate factor change!
				double oldA = rxn->get_a();
				rxn->notifyRateFactorChange(this,rxnPos,this->getRxnListMappingId(rxnIndex));
				this->parentMoleculeType->getSystem()->update_A_tot(rxn,oldA,rxn->update_a());
			}
		}
	}

}

///////////////
//  MOLECULE_DEPENDENT_UPDATE_ADDITION
//void Molecule::addDependentUpdateMolecule(Molecule *m) {
//	for(molIter=dependentUpdateMolecules.begin();molIter!=dependentUpdateMolecules.end();molIter++)
//		if((*molIter)->getUniqueID()==m->getUniqueID())
//			return;
//	dependentUpdateMolecules.push_back(m);
//}
//void Molecule::removeDependentUpdateMolecule(Molecule *m) {
//	for(molIter=dependentUpdateMolecules.begin();molIter!=dependentUpdateMolecules.end();molIter++)
//		if((*molIter)->getUniqueID()==m->getUniqueID()) {
//			dependentUpdateMolecules.erase(molIter);
//		}
//}
////////////////



//void Molecule::updateDORs()
//{
//
//	for(int r=0; r<parentMoleculeType->getDORrxnCount(); r++)
//	{
//
//		ReactionClass * DORrxn = parentMoleculeType->getDORrxn(r);
//		int dorRxnIndex = parentMoleculeType->getDORreactantIndex(r);
//		int dorRxnPos = parentMoleculeType->getDORreactantPosition(r);
//
//	//	cout<<" identified DOR RXN index: "<<dorRxnIndex<<endl;
//	//	cout<<" identified DOR RXN pos: "<<dorRxnPos<<endl;
//		DORrxn->notifyRateFactorChange(this, dorRxnPos, rxnListMappingId[dorRxnIndex]);
//	}
//
//}

//double Molecule::getDORvalueFromGroup(string groupName, int valueIndex)
//{
////	for(listenerIter = listeners.begin(); listenerIter != listeners.end(); listenerIter++ )
////	{
////		if(groupName==(*listenerIter)->getGroupName())
////			return (*listenerIter)->getValue(valueIndex);
////	}
//
//	cerr<<"Error!! trying to get DOR value for a group, but no name match!"<<endl;
//	cerr<<"    Looking for group: "<<groupName<<" from molecule ";
//	cerr<<this->getMoleculeTypeName()<<"_"<<this->getUniqueID()<<endl;
//	exit(1);
//}



void Molecule::removeFromObservables()
{
	parentMoleculeType->removeFromObservables(this);
}
void Molecule::addToObservables()
{
	parentMoleculeType->addToObservables(this);
}




void Molecule::setComponentState(int cIndex, int newValue)
{
	this->component[cIndex]=newValue;

	//if(listeners.size()>0) cout<<"Molecule State has changed..."<<endl;
	//Let all the listeners know that the state of a molecule has changed...
	//for(listenerIter = listeners.begin(); listenerIter != listeners.end(); listenerIter++ )
	//	(*listenerIter)->notify(this,stateIndex);
}
void Molecule::setComponentState(string cName, int newValue) {
	this->component[this->parentMoleculeType->getCompIndexFromName(cName)]=newValue;
}


void Molecule::printDetails() {
	this->printDetails(cout);
}
void Molecule::printDetails(ostream &o)
{
	int degree = 0;
	o<<"++ Molecule instance of type: " << parentMoleculeType->getName();
	o<< " (uId="<<ID_unique << ", tId=" << ID_type << ", cId" << ID_complex<<", degree="<<degree<<")"<<endl;
	o<<"      components: ";
	for(int c=0; c<numOfComponents; c++)
	{
		if(c!=0)o<<"                  ";
		o<< parentMoleculeType->getComponentName(c) <<"=";
		o<<parentMoleculeType->getComponentStateName(c,component[c]);
		o<<"\tbond=";
		if(bond[c]==NOBOND) o<<"empty";
		else o<<bond[c]->getUniqueID();
		o<<endl;
	}

	o.flush();
	if(parentMoleculeType->getNumOfTypeIFunctions()>0) {
		o<<"      loc funcs:";
		for(int lf=0; lf<parentMoleculeType->getNumOfTypeIFunctions(); lf++) {
			if(lf!=0) o<<"                  ";
			o<<"  "<<parentMoleculeType->getTypeILocalFunction(lf)->getNiceName();
			o<<"="<<localFunctionValues[lf]<<"\n";
		}
	}
}

//Get the number of molecules this molecule is bonded to
int Molecule::getDegree()
{
	int degree = 0;
	for(int c=0; c<numOfComponents; c++)
		if(bond[c]!=NOBOND) degree++;
	return degree;
}

bool Molecule::isBindingSiteOpen(int cIndex) const
{
	if(bond[cIndex]==NOBOND) return true;
	return false;
}

bool Molecule::isBindingSiteBonded(int cIndex) const
{
	if(bond[cIndex]==NOBOND) return false;
	return true;
}

Molecule * Molecule::getBondedMolecule(int cIndex) const
{
	return bond[cIndex];
}



void Molecule::bind(Molecule *m1, int cIndex1, Molecule *m2, int cIndex2)
{
	if(m1->bond[cIndex1]!=NOBOND || m2->bond[cIndex2]!=NOBOND) {
		cerr<<endl<<endl<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
		cerr<<"Your universal traversal limit was probably set too low, so some updates were not correct!\n\n";

		cerr<<"Trying to bond "<< m1->getMoleculeTypeName() << "_"<<m1->getUniqueID()<<"(";
		cerr<<m1->getMoleculeType()->getComponentName(cIndex1)<<") & ";
		cerr<< m2->getMoleculeTypeName()<<"_"<<m2->getUniqueID()<<"(";
		cerr<<m2->getMoleculeType()->getComponentName(cIndex2)<<")\n";
		cerr<<" to sites that are already occupied!  Check rxn rules!!\n";
		cerr<<"\n";
		m1->printDetails(cerr);
		m2->printDetails(cerr);
		exit(1);
	}

	m1->bond[cIndex1] = m2;
	m2->bond[cIndex2] = m1;

	m1->indexOfBond[cIndex1] = cIndex2;
	m2->indexOfBond[cIndex2] = cIndex1;

	//Handle Complexes
	if(m1->useComplex)
	{
		if(m1->getComplex()!=m2->getComplex())
			m1->getComplex()->mergeWithList(m2->getComplex());
	}
}

void Molecule::bind(Molecule *m1, string compName1, Molecule *m2, string compName2)
{
	int cIndex1 = m1->getMoleculeType()->getCompIndexFromName(compName1);
	int cIndex2 = m2->getMoleculeType()->getCompIndexFromName(compName2);
	Molecule::bind(m1, cIndex1, m2, cIndex2);
}


void Molecule::unbind(Molecule *m1, int cIndex)
{
	//get the other molecule bound to this site
	//cout<<"I am here. "<<bSiteIndex<<endl;
	Molecule *m2 = m1->bond[cIndex];
	if(m2==NULL)
	{
		cerr<<endl<<endl<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
		cerr<<"Your universal traversal limit was probably set too low, so some updates were not correct!"<<endl;
		cerr<<"Trying to unbind a binding site that is not bound!!  Check rxn rules, and traversal limits! Quitting."<<endl;
		cerr<<endl<<endl<<"The molecule is:"<<endl;
		m1->printDetails(cerr);
		cerr<<endl<<"The site trying to be unbound was: ";
		cerr<<m1->getMoleculeType()->getComponentName(cIndex)<<endl;
		exit(3);
	}


	int cIndex2 = m1->indexOfBond[cIndex];

	//break the bond (older compilers don't let you assign NOBOND to type molecule)
	m1->bond[cIndex] = 0; //NOBOND;
	m2->bond[cIndex2] = 0; //NOBOND;

	m1->indexOfBond[cIndex] = NOINDEX;
	m2->indexOfBond[cIndex2] = NOINDEX;

	//Handle Complexes
	if(m1->useComplex)
	{
		m1->getComplex()->updateComplexMembership(m1);
	}

	//cout<<" UnBinding!  mol1 complex: ";
	//m1->getComplex()->printDetails();
}

void Molecule::unbind(Molecule *m1, char * compName)
{
	int cIndex = m1->getMoleculeType()->getCompIndexFromName(compName);
	Molecule::unbind(m1,cIndex);
}








queue <Molecule *> Molecule::q;
queue <int> Molecule::d;
list <Molecule *>::iterator Molecule::molIter;

void Molecule::breadthFirstSearch(list <Molecule *> &members, Molecule *m, int depth)
{
	if(m==0) {
		cerr<<"Error in Molecule::breadthFirstSearch, m is null.\n";
		cerr<<"Likely an internal error where a MappingSet is on a list and\n";
		cerr<<"is not actually mapped to any molecule!";
		exit(3);
	}

	//Create the queues (for effeciency, now queues are a static attribute of Molecule...)
	//queue <Molecule *> q;
	//queue <int> d;
	int currentDepth = 0;

	//cout<<"traversing on:"<<endl;
	//m->printDetails();

	//First add this molecule
	q.push(m);
	members.push_back(m);
	d.push(currentDepth+1);
	m->hasVisitedMolecule=true;

	//Look at children until the queue is empty
	while(!q.empty())
	{
		//Get the next parent to look at (currentMolecule)
		Molecule *cM = q.front();
		currentDepth = d.front();
		q.pop();
		d.pop();

		//Make sure the depth does not exceed the limit we want to search
		if((depth!=ReactionClass::NO_LIMIT) && (currentDepth>=depth)) continue;

		//Loop through the bonds
		int cMax = cM->numOfComponents;
		for(int c=0; c<cMax; c++)
		{
			//cM->getComp
			if(cM->isBindingSiteBonded(c))
			{
				Molecule *neighbor = cM->getBondedMolecule(c);
				//cout<<"looking at neighbor: "<<endl;
				//neighbor->printDetails();
				if(!neighbor->hasVisitedMolecule)
				{
					neighbor->hasVisitedMolecule=true;
					members.push_back(neighbor);
					q.push(neighbor);
					d.push(currentDepth+1);
					//cout<<"adding... to traversal list."<<endl;
				}
			}
		}
	}


	//clear the has visitedMolecule values
	for( molIter = members.begin(); molIter != members.end(); molIter++ )
  		(*molIter)->hasVisitedMolecule=false;
}


//void Molecule::traverseBondedNeighborhoodForUpdate(list <Molecule *> &members, int traversalLimit)
//{
//	traverseBondedNeighborhood(members,traversalLimit);
//	for(molIter=dependentUpdateMolecules.begin();molIter!=dependentUpdateMolecules.end();molIter++)
//	{
//		bool isPresent=false;
//		for( molIter2 = members.begin(); molIter2 != members.end(); molIter2++ ) {
//			if((*molIter)==(*molIter2)) { isPresent = true; break;}
//		}
//		if(!isPresent) members.push_back((*molIter));
//	}
//
//}


void Molecule::traverseBondedNeighborhood(list <Molecule *> &members, int traversalLimit)
{
	//always call breadth first search, it is a bit faster
	//if(traversalLimit>=0)
		Molecule::breadthFirstSearch(members, this, traversalLimit);
	//else
	//	this->depthFirstSearch(members);
}


//Isn't ever called really, but is availabe.  Note that it cannot use traversal limits
//because it is depth first
void Molecule::depthFirstSearch(list <Molecule *> &members)
{
	if(this->hasVisitedMolecule==true) {
		return;
	}

	this->hasVisitedMolecule=true;
	members.push_back(this);

	int cMax = this->numOfComponents;
	for(int c=0; c<cMax; c++)
	{
		if(hasVisitedBond[c]==true) continue;
		if(this->isBindingSiteBonded(c))
		{
			Molecule *neighbor = this->getBondedMolecule(c);
			neighbor->hasVisitedBond[indexOfBond[c]]=true;
			hasVisitedBond[c]=true;
			neighbor->depthFirstSearch(members);
		}
	}

	//clear things out
	hasVisitedMolecule = false;
	for(int c=0; c<numOfComponents; c++)
		hasVisitedBond[c] = false;
}



void Molecule::printMoleculeList(list <Molecule *> &members)
{
	cout<<"List of molecules contains: "<<endl;
	list <Molecule *>::iterator molIter;
	for( molIter = members.begin(); molIter != members.end(); molIter++ ) {
		cout<<"   -"<<(*molIter)->getMoleculeTypeName();
		cout<<"_u"<<(*molIter)->getUniqueID()<<endl;
	}
}






