/**
 * SC_composers.cpp
 *
 *	@date Jan 29, 2010
 *	@author michael sneddon
 */

#include "SC_composers.hh"
#include "../agentFactories/SC_agentFactories.hh"
#include "../../../communicator/serial/serialcommunicator.hh"
#include "../simpleCell.hh"

#include <vector>
#include <string>
#include <fstream> // for ifstream, ofstream, ios_base
#include <iostream>// for cin, cout, endl
#include <exception>
#include <stdexcept>

using namespace std;
using namespace Hive;
using namespace SimpleCell;


SimpleCellDefaultComposer::SimpleCellDefaultComposer(string inputFileName, string outputFileName, double eqTime) {
	cout << "creating a SimpleCellDefaultComposer." << endl;
	parseInputFile(inputFileName);
	this->eqTime = eqTime;

	this->outputFile.open((outputFileName).c_str(), ios_base::out | ios_base::binary | ios_base::trunc);
	if(!outputFile.is_open()) {
		throw(Hive::HiveException("Cannot open stream to outputFile: "+outputFileName,
							"SimpleCellDefaultComposer::SimpleCellDefaultComposer(string inputFileName, string outputFileName)"));
	}
}

SimpleCellDefaultComposer::~SimpleCellDefaultComposer()
{
	outputFile.flush();
	outputFile.close();

	cout << "deleting a SimpleCellDefaultComposer." << endl;
	delete communicator;
}

void SimpleCellDefaultComposer::setupSimulation()
{
	cout << "\n\nsetting up a simulation with a SimpleCellDefaultComposer." << endl;

	//setup the proper environment

	//create the proper signaling system for the given environment type
	int signalingType = SimpleCellConstants::SIMPLE_WINGREEN_SIGNALING;
	if(environmentType==SimpleCellConstants::CONSTANT_ENVIRONMENT) signalingType = SimpleCellConstants::NOISE_SIGNALING;

	// setup the motor and swimming type
	int motorType = SimpleCellConstants::SINGLE_MOTOR;
	int motionType = SimpleCellConstants::STANDARD_SWIMMING;



	//First, create the factories
	this->factories.push_back(new EnvironmentAgentFactory(environmentType));
	this->factories.push_back(new SimpleCellFactory(signalingType,motorType,motionType,eqTime));


	// Create the head environment agent
	this->maestro = factories.at(0)->createAgent();
	this->orchestra.push_back(maestro);


	//Create all of the SimpleCells
	for(unsigned int k=0; k<meanCheYp.size(); k++) {

		double cheYp = meanCheYp.at(k);
		double tau = this->timescale.at(k);
		double noise = this->noise.at(k);
		int nMotors = this->numberOfMotors.at(k);

		bool areCorrelated = true;
		if(this->isMotorCorrelated.at(k)<1) areCorrelated = false;

		int cellCount = this->numberOfCells.at(k);

		for (int c=0; c<cellCount; c++) {
			Agent *cellAgent = ((SimpleCellFactory *)factories.at(1))->createAgent(
					cheYp, tau, noise, nMotors, areCorrelated);
			this->orchestra.push_back(cellAgent);
		}
	}


	// set up the hierarchy among the agents
	this->setupAgentHierarchy();

	// finally add a serial communciator
	this->addSerialCommunicator();

	cout << "simulation is all setup." << endl;

}





void SimpleCellDefaultComposer::parseInputFile(string inputFileName)
{
	bool outputLines = true;
	cout<<"\n\nParsing input file in SimpleCellDefaultComposer...\n";
	try {

		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//open up the file
		ifstream distFile(inputFileName.c_str());


		//If the file is open, start reading in the details...
		if (distFile.is_open()) {

			//First, get environment parameters - No error checking, so make sure you are right!
			getline(distFile,line);  //throw out first line, cause it's the header
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;

			getline(distFile,line);
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;

			lastFound=line.find(delimeter); nextFound=0;
			string s = line.substr(0,lastFound); Util::trim(s);
			this->environmentType = Util::convertToInt(s);
			if(outputLines) cout<<"  --> parsed environment type = "<<environmentType<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->envParam1 = Util::convertToDouble(s);
			lastFound=nextFound;
			if(outputLines) cout<<"  --> parsed envParam1 = "<<envParam1<<"\n";

			nextFound = line.find(delimeter,lastFound+1);
			s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
			this->envParam2 = Util::convertToInt(s);
			lastFound=nextFound;
			if(outputLines) cout<<"  --> parsed envParam2 = "<<envParam2<<"\n";

			getline(distFile,line);
			if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
			lineCounter++;



			//Then we loop through the file and read / parse each element as a double valued
			//number.  As we read them in, we'll immediately dump the double to the output
			//file stream.
			while (! distFile.eof() ) {
				getline(distFile,line);
				if(line.size()==0) continue;
				if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";

				//CheY-P level
				lastFound=line.find(delimeter); nextFound=0;
				string s = line.substr(0,lastFound); Util::trim(s);
				this->meanCheYp.push_back(Util::convertToDouble(s));
				if(outputLines) cout<<"  --> parsed meanCheYp = "<<meanCheYp.back()<<"\n";


				vector <double> timescale;
				vector <double> noise;
				vector <double> numberOfMotors;
				vector <int> numberOfCells;


				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				this->timescale.push_back(Util::convertToDouble(s));
				lastFound=nextFound;
				if(outputLines) cout<<"  --> parsed timescale = "<<this->timescale.back()<<"\n";

				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				this->noise.push_back(Util::convertToDouble(s));
				lastFound=nextFound;
				if(outputLines) cout<<"  --> parsed noise = "<<this->noise.back()<<"\n";

				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				this->numberOfMotors.push_back(Util::convertToInt(s));
				lastFound=nextFound;
				if(outputLines) cout<<"  --> parsed numberOfMotors = "<<this->numberOfMotors.back()<<"\n";

				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				this->numberOfCells.push_back(Util::convertToInt(s));
				lastFound=nextFound;
				if(outputLines) cout<<"  --> parsed numberOfCells = "<<this->numberOfCells.back()<<"\n";

				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				this->isMotorCorrelated.push_back(Util::convertToInt(s));
				lastFound=nextFound;
				if(outputLines) cout<<"  --> parsed isMotorCorrelated = "<<this->numberOfCells.back()<<"\n";



				lineCounter++;
			}

		} else {
			throw(Hive::HiveException("Unable to read or process input file: "+inputFileName,
					"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
		}

	} catch (Hive::HiveException& e) {
		e.addTraceException("SimpleCellDefaultComposer::parseInputFile(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file: "+inputFileName,
				"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
	}

	cout<<"done parsing file."<<endl;
}



void SimpleCellDefaultComposer::outputNow(double currentTime)
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

void SimpleCellDefaultComposer::updateAllChemicalConc(double c)
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
void SimpleCellDefaultComposer::initializeAgentFactories() { }


void SimpleCellDefaultComposer::setupAgentHierarchy()
{
	//Create all of the SimpleCells
	for(unsigned int k=1; k<orchestra.size(); k++) {
		Agent *cellAgent = orchestra.at(k);
		maestro->addChild(cellAgent->getAgentId());
		cellAgent->setParent(maestro->getAgentId());
	}
}


Agent* SimpleCellDefaultComposer::createAgent(AgentFactory& af) {
	cout<<"for now, do not use: call SimpleCellDefaultComposer::createAgent(AgentFactory& af)"<<endl;
	exit(1);
	return af.createAgent();
}



void SimpleCellDefaultComposer::addSerialCommunicator()
{
	SerialCommunicator *postman = new SerialCommunicator();
	for (unsigned int i=0; i<this->orchestra.size(); i++) {
		postman->addAgent(orchestra[i]);
		orchestra[i]->addCommunicator(postman);
	}
	this->communicator=postman;
}
