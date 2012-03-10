/*
 * nfAgentFactory.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */


#include <map>

#include "nfAgentFactory.hh"
#include "../../data/primitive/primitiveData.hh"




using namespace Hive;
using namespace NF;
using namespace std;


NF_AgentFactory::NF_AgentFactory(map<string,string> &argMap) : AgentFactory()
{
	//step 1: copy over the argMap and other parameters
	for( map<string,string>::const_iterator iter = argMap.begin(); iter != argMap.end(); iter++ )
	{
		this->localArgMapCopy.insert(pair<string, string>(string(iter->first),string(iter->second) ));
	}


	//step 2: get the xml filename, or throw an error if you didn't give anything
	if(localArgMapCopy.find("xml")!=localArgMapCopy.end())
	{
		xmlFilename = argMap.find("xml")->second;
		if(xmlFilename.empty())  {
			throw HiveException("XML file specified (in -xml flag) for creating an NFsim Agent",
					"NF_AgentFactory::NF_AgentFactory(map<string,string> &argMap, bool verbose)");
		}
	} else {
		throw HiveException("No XML file specified (with -xml flag) for creating an NFsim Agent",
							"NF_AgentFactory::NF_AgentFactory(map<string,string> &argMap, bool verbose)");
	}
	if(localArgMapCopy.find("v")!=localArgMapCopy.end())
		this->verbose=true;
	else
		this->verbose=false;


	if(verbose) {
		cout<<"created NF_AgentFactory, and set to filename: "<<this->xmlFilename<<endl;
	}

}

NF_AgentFactory::~NF_AgentFactory()
{
	cout<<"killing NF_agentFactory"<<endl;
}

Agent *NF_AgentFactory::createAgent()
{
	cout<<"\t-In NF_AgentFactory: creating a new agent"<<endl;
	Agent *a;
	try {
		//Create the agent
		 a = new Agent();

		//Initialize the old NFsim System object with a reader
		cout<<"\t-In NF_AgentFactory: ";
		NF::System *s = NF::initSystemFromFlags(this->localArgMapCopy,verbose);
		s->prepareForSimulation();

		cout<<"\t-In NF_AgentFactory: NFsim System is good."<<endl;

		//Setup the database
		setupDatabase(a, s);


		//Create the simulator
		NFsim *nfsim = new NFsim(a,s);
		a->addSimulator(nfsim);


		//Create the default NFsim actions



	} catch (HiveException& e) {
		e.addTraceException("NF_AgentFactory::createAgent()");
		throw e;
	}
	return a;
}

void NF_AgentFactory::setupDatabase(Agent *a, NF::System *s)
{
	cout<<"\t-In NF_AgentFactory: setting up NFstyle database"<<endl;
	Database *db= a->getDatabase();
	for(int m=0; m<s->getNumOfMoleculeTypes(); m++)
	{
		MoleculeType *mt = s->getMoleculeType(m);
		mt->printDetails();
		NF_MoleculeTypeData *mtd = new NF_MoleculeTypeData(mt,mt->getName());
		db->addData(mtd->getName(),mtd);
	}

	for(int o=0; o<s->getNumOfObservables(); o++)
	{
		Observable *obs = s->getObservable(o);
		NF_ObservableData *od = new NF_ObservableData(obs,obs->getName());
		cout<<"===="<<obs->getName()<<" "<<obs->getCount()<<endl;
		db->addData(od->getName(),od);
	}



	////////CHEMOTAXIS ADDITION/////////////
	// add the ligand parameter
	DoubleData *lig = new DoubleData("LigandConcentration",2);
	db->addData(lig->getName(),lig);
	/////////////////////////////////////////

}


Agent * NF_AgentFactory::createAgent(Action ** as)
{
	return NULL;
}

Agent * NF_AgentFactory::duplicateAgent(Agent *ag)
{
	return NULL;
}

void NF_AgentFactory::addActionToAgentsActionSet(Action *a)
{

}

void NF_AgentFactory::addSimulatorToAgent(Simulator *s, Agent *ag)
{

}


void NF_AgentFactory::finalise()
{

}


