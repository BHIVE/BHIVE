/*
 * SC_parallelComposers.cpp
 *
 *  Created on: Feb 23, 2010
 *      Author: msneddon
 */

#include "SC_composers.hh"
#include "../agentFactories/SC_agentFactories.hh"
#include "../../../communicator/serial/serialcommunicator.hh"
#include "../../../communicator/ronald/reagan.hh"
#include "../simpleCell.hh"
#include "../../../parallel/parallel.hh"

#include <vector>
#include <string>
#include <fstream> // for ifstream, ofstream, ios_base
#include <iostream>// for cin, cout, endl
#include <exception>
#include <stdexcept>

using namespace std;
using namespace Hive;
using namespace SimpleCell;




SimpleCellDefaultParallelComposer::SimpleCellDefaultParallelComposer(string inputFileName, string outputFileName, double eqTime) {
	//cout << "creating a SimpleCellDefaultComposer." << endl;
	this->inputFileName = inputFileName;
	this->eqTime = eqTime;

//	this->outputFile.open((outputFileName).c_str(), ios_base::out | ios_base::binary | ios_base::trunc);
//	if(!outputFile.is_open()) {
//		throw(Hive::HiveException("Cannot open stream to outputFile: "+outputFileName,
//							"SimpleCellDefaultComposer::SimpleCellDefaultComposer(string inputFileName, string outputFileName)"));
//	}
}

SimpleCellDefaultParallelComposer::~SimpleCellDefaultParallelComposer()
{
//	outputFile.flush();
//	outputFile.close();

	cout << "deleting a SimpleCellDefaultComposer." << endl;
	delete communicator;
}



void SimpleCellDefaultParallelComposer::setupSimulation()
{

	// first retrieve and parse the input file buffer
	vector <string> buffer;
	parallel_file_input(inputFileName,buffer);
	this->parseInputFileBuffer(buffer);


	// count how many cells we need to make
	int totalCellCount = 0;
	for(unsigned int k=0; k<numberOfCells.size(); k++) {
		totalCellCount += numberOfCells.at(k);
	}

	// calculate the number of cells that you have to compose
	int number_cells_to_compose = (totalCellCount/Hive::mpisize());

//	cerr<<"node: "<<Hive::mpirank()<<" cells: "<<number_cells_to_compose<<endl;

	// calculate what startid these agents will have
	int startid = Hive::mpirank() * number_cells_to_compose;
	this->total_number_of_cells = Hive::mpisize() * number_cells_to_compose;



	// setup agentfactories
	environmentType = SimpleCellConstants::CONSTANT_ENVIRONMENT;

	// setup the motor and swimming type
	int motorType = SimpleCellConstants::SINGLE_MOTOR;
	int motionType = SimpleCellConstants::STANDARD_SWIMMING;
	int signalingType = SimpleCellConstants::SIMPLE_WINGREEN_SIGNALING;

	if (Hive::mpirank() == 0)
		this->factories.push_back(new EnvironmentAgentFactory(environmentType));
	this->factories.push_back(new SimpleCellFactory(signalingType,motorType,motionType,eqTime));

	// create agents
	int agent_id = 0;
	if (Hive::mpirank() == 0) {
		this->maestro = factories.at(0)->createAgent();
		this->orchestra.push_back(maestro);
		this->orchestra[0]->setAgentID(agent_id++);
		this->orchestra[0]->setNodeID(Hive::mpirank());
	}

	// create the cell agents
	int start = startid;
//	cerr << Hive::mpirank() << " " << start << " " << start+number_cells_to_compose << endl;
	int agentidtogive = 0;
	for (int i=start; i<start+number_cells_to_compose; i++) {
		// now we have to figure out which parameters we will have to use for setting up the the cells
		int accumulator = 0;
		int index = 0;
		do {
			accumulator += numberOfCells[index++];
		} while (accumulator < i);
		index--;
		double cheYp = meanCheYp[index];
		double tau   = timescale[index];
		double noise = this->noise[index];
		int nMotors  = numberOfMotors[index];
		bool areCorrelated = true;
		if(this->isMotorCorrelated.at(index)<1) areCorrelated = false;

		// create agents
		if(Hive::mpirank() == 0)
			this->orchestra.push_back(((SimpleCellFactory*)factories.at(1))->createAgent(cheYp, tau, noise, nMotors, areCorrelated));
		else
			this->orchestra.push_back(((SimpleCellFactory*)factories.at(0))->createAgent(cheYp, tau, noise, nMotors, areCorrelated));
		if (Hive::mpirank() == 0)
			this->orchestra[orchestra.size()-1]->setAgentID(agentidtogive+1);
		else
			this->orchestra[orchestra.size()-1]->setAgentID(agentidtogive);
		agentidtogive+=1;
		this->orchestra[orchestra.size()-1]->setNodeID(Hive::mpirank());

	}

//	cerr << Hive::mpirank() << " " << orchestra.size() << endl;
//	if (Hive::mpirank() == 0) {
//		for (int i=0; i<orchestra.size(); i++)
//			cerr << orchestra[i]->getAgentId() << endl;
//	}
	// add hierarchy to agents.
	this->setupAgentHierarchy();
	// add communicator ...
	this->addSerialCommunicator();
}




void SimpleCellDefaultParallelComposer::parseInputFileBuffer(vector <string> &buffer)
{
//	if(Hive::mpirank()==0){
//		cerr<<"bs"<<buffer.size()<<endl;
//	for (int i=0; i<buffer.size(); i++)
//		cerr<<"'"<<buffer.at(i)<<"'"<<endl;
//	}

	try {
		const char* delimeter = ",";
		string line;
		size_t lastFound=0, nextFound=0;

		// throw out the first line by starting at position 1
		unsigned int currentLineIndex = 1;

		// read in the environment information, which is the same for all agents
		line = buffer.at(currentLineIndex); currentLineIndex++;


		lastFound=line.find(delimeter); nextFound=0;
		string s = line.substr(0,lastFound); Util::trim(s);
		this->environmentType = Util::convertToInt(s);

		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
		this->envParam1 = Util::convertToDouble(s);
		lastFound=nextFound;

		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
		this->envParam2 = Util::convertToInt(s);
		lastFound=nextFound;


		// throw out the next line by going to the next line index
		currentLineIndex++;


		//Then we loop through the file and read / parse each element as a double valued
		//number.  As we read them in, we'll immediately dump the double to the output
		//file stream.
		while (currentLineIndex < buffer.size() ) {
			line = buffer.at(currentLineIndex);
			currentLineIndex++;
			if(line.size()==0) continue;

			//CheY-P level
			lastFound=line.find(delimeter); nextFound=0;
			string s = line.substr(0,lastFound); Util::trim(s);
			this->meanCheYp.push_back(Util::convertToDouble(s));

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->timescale.push_back(Util::convertToDouble(s));
			lastFound=nextFound;
			//if(outputLines) cout<<"  --> parsed timescale = "<<this->timescale.back()<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->noise.push_back(Util::convertToDouble(s));
			lastFound=nextFound;
			//if(outputLines) cout<<"  --> parsed noise = "<<this->noise.back()<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->numberOfMotors.push_back(Util::convertToInt(s));
			lastFound=nextFound;
			//if(outputLines) cout<<"  --> parsed numberOfMotors = "<<this->numberOfMotors.back()<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->numberOfCells.push_back(Util::convertToInt(s));
			lastFound=nextFound;
			//if(outputLines) cout<<"  --> parsed numberOfCells = "<<this->numberOfCells.back()<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->isMotorCorrelated.push_back(Util::convertToInt(s));
			lastFound=nextFound;
			//if(outputLines) cout<<"  --> parsed isMotorCorrelated = "<<this->numberOfCells.back()<<"\n";



		}



	} catch (Hive::HiveException& e) {
		e.addTraceException("SimpleCellDefaultComposer::parseInputFile(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file buffer",
				"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
	}
}




void SimpleCellDefaultParallelComposer::outputNow(double currentTime)
{
	// output format:
	// cellNumber, time, cellState, CheYp, motState[0], x,y,z


	//Create all of the SimpleCells
	for(unsigned int k=1; k<orchestra.size(); k++) {
		Agent *cellAgent = orchestra.at(k);
		Database *db = cellAgent->getDatabase();

		// cellNumber
		double kDouble = (double)k;
		outputFile.write((char *)&kDouble,sizeof(double));

		// time
		outputFile.write((char *)&currentTime,sizeof(double));


		//cell state
		double cellState = (double)((IntegerData *)db->getDataItem("CurrentCellState"))->getInteger();
		outputFile.write((char *)&cellState, sizeof(double));


		//cheYp
		double ypConc = ((DoubleVectorData *)db->getDataItem("CheYp"))->getDouble(0);
		outputFile.write((char *)&ypConc, sizeof(double));

		//cheYp
		double motState = (double)((IntVectorData *)db->getDataItem("MotorStates"))->getInt(0);
		outputFile.write((char *)&motState, sizeof(double));

		DoubleVectorData *curPos = ((DoubleVectorData *)db->getDataItem("Position"));
		double x = curPos->getDouble(1);
		double y = curPos->getDouble(2);
		double z = curPos->getDouble(3);

		outputFile.write((char *)&x, sizeof(double));
		outputFile.write((char *)&y, sizeof(double));
		outputFile.write((char *)&z, sizeof(double));


		double lig = ((DoubleVectorData *)db->getDataItem("Chemical"))->getDouble(0);
		outputFile.write((char *)&lig, sizeof(double));


	}

}

void SimpleCellDefaultParallelComposer::updateAllChemicalConc(double c)
{
	cout<<"In SimpleCellDefaultComposer, updating all concentrations to : "<<c<<'M'<<endl;
	for (unsigned int i=1; i<this->orchestra.size(); i++)
	{
		Hive::Message *m = new Message();
		DoubleVectorData *dvd = new DoubleVectorData("NewChemical",1);
		dvd->setDouble(c,0);
		m->setArgument(dvd);
		m->setAction(0);
		m->setDestinationID(i);

		communicator->sendMessage(m);
	}
	cout<<"all messages sent.\n";
}


// Not called for the SimpleCellDefaultComposer
void SimpleCellDefaultParallelComposer::initializeAgentFactories() { }


void SimpleCellDefaultParallelComposer::setupAgentHierarchy()
{
	int number_cells_per_rank = this->total_number_of_cells/Hive::mpisize();

	if (Hive::mpirank()==0) {
		for (int i=0; i<Hive::mpisize(); i++)
			for (int k=0; k<number_cells_per_rank; k++)
				if (i == 0)
					this->orchestra[0]->addChild(k+1,i);
				else
					this->orchestra[0]->addChild(k,i);
	}

	int start;
	if (Hive::mpirank() == 0)
		start = 1;
	else
		start = 0;

	for (int i=start; i<start+number_cells_per_rank; i++)
		this->orchestra[i]->setParent(0,0);

}


Agent* SimpleCellDefaultParallelComposer::createAgent(AgentFactory& af) {
	cout<<"for now, do not use: call SimpleCellDefaultComposer::createAgent(AgentFactory& af)"<<endl;
	exit(1);
	return af.createAgent();
}



void SimpleCellDefaultParallelComposer::addSerialCommunicator()
{
	Reagan *ronnie = new Reagan();
	for (unsigned int i=0; i<this->orchestra.size(); i++) {
		ronnie->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(ronnie);
	}
	this->communicator = ronnie;
}

