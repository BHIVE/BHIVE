/*
 * cellparameterinitializer.cpp
 *
 *  Created on: Apr 19, 2010
 *      Author: msneddon
 */



#include "cellparameterinitializer.hh"
#include "../../exception/exception.hh"
#include "../../util/util.hh"


#include <fstream> // for ifstream, ofstream, ios_base
#include <iostream>// for cin, cout, endl
#include <exception>
#include <stdexcept>

using namespace std;
using namespace Hive;
using namespace ChemoPop;



// init blind agents
CellParameterInitializer::CellParameterInitializer(int n_blind_agents)
{

	//Default Parameters (NOT FROM FILE!!!)
	this->isBlindAgent= true;
	this->number_cells=n_blind_agents;
}

// init e.coli cells
CellParameterInitializer::CellParameterInitializer(string inputFileName)
{
	//Default Parameters (NOT FROM FILE!!!)
	//this->n_cells=20;
	//for(int k=0; k<n_cells; k++)
	//	cells.push_back(new CellParameterData ());
	this->isBlindAgent= false;

	// READ FILE HERE!!!!

	bool outputLines = false;
	bool outputParse = false;
	//cout<<"\n\nParsing input file in SimpleCellDefaultComposer...\n";
	try {

		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//open up the file
		ifstream distFile(inputFileName.c_str());


		//If the file is open, start reading in the details...
		if (distFile.is_open()) {


			//Then we loop through the file and read / parse each element as a double valued
			//number.  As we read them in, we'll immediately dump the double to the output
			//file stream.
			while (! distFile.eof() ) {
				getline(distFile,line);
				if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
				lineCounter++;
				if(line.size()==0) continue;
				Util::trim(line);

				//skip comments
				if(line.at(0)=='#'||line.at(0)=='%') continue;


				// local values saved as we read the file
				double meanCheYp;
				double tau;
				double noise;
				int numOfMotors;
				bool isCoordinated;
				int flagellaModel;
				int minFlagellaToRun;
				double semiWaitingTime;
				double cellSpeed;
				int numOfCells;



				//CheY-P level
				lastFound=line.find(delimeter); nextFound=0;
				string s = line.substr(0,lastFound); Util::trim(s);
				meanCheYp = Util::convertToDouble(s);
				if(outputParse) cout<<"  Mean [CheYp] :: "<<meanCheYp<<endl;

				// Tau
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				tau = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  tau :: "<<tau<<endl;

				// Noise
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				noise = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  noise :: "<<noise<<endl;

				//numOfMotors
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				numOfMotors = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  numOfMotors :: "<<numOfMotors<<endl;

				//isCoordinated
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				if(Util::convertToInt(s)==0) isCoordinated = false; else isCoordinated = true;
				lastFound=nextFound;
				if(outputParse) cout<<"  isCoordinated :: "<<isCoordinated<<endl;

				//flagellaModel
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				flagellaModel = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  flagellaModel :: "<<flagellaModel<<endl;

				//minFlagellaToRun
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				minFlagellaToRun = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  minFlagellaToRun :: "<<minFlagellaToRun<<endl;

				//semiWaitingTime
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				semiWaitingTime = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  semiWaitingTime :: "<<semiWaitingTime<<endl;


				//cellSpeed
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				cellSpeed = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  cellSpeed :: "<<semiWaitingTime<<endl;


				// number of Cells
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				numOfCells = Util::convertToInt(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  numOfCells :: "<<numOfCells<<endl;


				// isChemotactic = if
				nextFound = line.find(delimeter,lastFound+1);
				int isChemotactic = 0;
				if(string::npos!=nextFound)
				{
					s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
					isChemotactic = Util::convertToInt(s);
					lastFound=nextFound;
					if(outputParse) {
						if(isChemotactic==0) cout<<"  chemotaxis is off"<<endl;
						else if(isChemotactic==1) cout<<"  chemotaxis is on"<<endl;
						else {
							cout<<"  error!  I don't know whether chemotaxis should be on or off!"<<endl;
							cout<<"  so by default, chemotaxis is on."<<endl;
							isChemotactic = 1;
						}
					}
				} else {
					if(outputParse) cout<<"  by default, chemotaxis is on :: "<<endl;
				}


				// Now create a cell parameter data object with the new information
				CellParameterData *cpd = new CellParameterData();
				cpd->meanCheYp = meanCheYp;
				cpd->tau = tau;
				cpd->sigma = noise;
				cpd->n_motors = numOfMotors;
				cpd->isFlagellaCoordinated = isCoordinated;
				cpd->flagellaModelType = flagellaModel;
				cpd->minNumberOfMotorsInCCWneededToSwim = minFlagellaToRun;
				cpd->minTimeInSemi = semiWaitingTime;
				cpd->speed = cellSpeed;
				cpd->isChemotactic = isChemotactic;

				//save the info for each cell in the cells array - no need to recreate
				//a cpd for each cell, just pass a pointer
				vector <int> uniqueCPDobjects;
				uniqueCPDobjects.push_back(cells.size());
				for(int k=0; k<numOfCells; k++) {
					cells.push_back(cpd);
				}


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


	this->number_cells = cells.size();

	//cout<<"done parsing file."<<endl;
	//exit(1);



}

CellParameterInitializer::~CellParameterInitializer()
{
	for (unsigned int i=0; i<cells.size(); i++) {
		delete cells[i];
		cells[i] = NULL;
	}
}

double CellParameterInitializer::getEqTime() const { return CellParameterData::eqTime; }
double CellParameterInitializer::getOutputInterval() const { return CellParameterData::outputInterval; }

void CellParameterInitializer::setNextCellParameters(Agent *cell)
{
	if(isBlindAgent) {
		throw HiveException("Cannot set parameters for another cell, as the cells are blind agents, not e.coli cells.",
								"CellParameterInitializer::setNextCellParameters(Agent *cell)");
	}


	if(currentCell>=this->number_cells) {
		throw HiveException("Cannot set parameters for another cell, as all cells that have been specified are already created.",
								"CellParameterInitializer::setNextCellParameters(Agent *cell)");
	}
	CellParameterData *cpd = this->cells.at(currentCell);
	Database *db = cell->getDatabase();


	// Set whether or not the cell is chemotactic
	IntegerData *isChemotacticData = new Hive::IntegerData("isChemotactic",cpd->isChemotactic);
	db->addData(isChemotacticData->getName(),isChemotacticData);



	// Set the swimming characteristics of the cell
	Data *speedData = new Hive::DoubleData("CellSpeed",cpd->speed);
	db->addData(speedData->getName(),speedData);

	Data *rotDiffConstData = new Hive::DoubleData("RotDiffConst",cpd->rotDiffConst);
	db->addData(rotDiffConstData->getName(),rotDiffConstData);



	// Set motor parameters
	Data *n_motor_Data = new Hive::IntegerData("n_motors",cpd->n_motors);
	db->addData(n_motor_Data->getName(),n_motor_Data);

	Data *flagellaModelType_Data = new Hive::IntegerData("flagellaModelType",cpd->flagellaModelType);
	db->addData(flagellaModelType_Data->getName(),flagellaModelType_Data);

	Data *isFlagellaCoordinated_Data = new Hive::BoolData("isFlagellaCoordinated",cpd->isFlagellaCoordinated);
	db->addData(isFlagellaCoordinated_Data->getName(),isFlagellaCoordinated_Data);

	Data *motor_Kd_Data = new Hive::DoubleData("motor_Kd",cpd->motor_Kd);
	db->addData(motor_Kd_Data->getName(),motor_Kd_Data);

	Data *motor_g1_Data = new Hive::DoubleData("motor_g1",cpd->motor_g1);
	db->addData(motor_g1_Data->getName(),motor_g1_Data);

	Data *motor_w_Data = new Hive::DoubleData("motor_w",cpd->motor_w);
	db->addData(motor_w_Data->getName(),motor_w_Data);


	TVectorData<int> *motorState = new TVectorData<int> ("MotorStates","tvector_integer");
	motorState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		motorState->at(m)=CellParameterData::CW;
	}
	db->addData(motorState->getName(),motorState);

	TVectorData<int> *lastmotorState = new TVectorData<int> ("LastMotorStates","tvector_integer");
	lastmotorState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		lastmotorState->at(m)=CellParameterData::CW;
	}
	db->addData(lastmotorState->getName(),lastmotorState);



	// Set flagella output
	TVectorData<int> *flagellaState = new TVectorData<int> ("FlagellaStates","tvector_integer");
	flagellaState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		flagellaState->at(m)=CellParameterData::NORMAL;
	}
	db->addData(flagellaState->getName(),flagellaState);

	TVectorData<int> *lastFlagellaState = new TVectorData<int> ("LastFlagellaStates","tvector_integer");
	lastFlagellaState->reserveSize(cpd->n_motors);
	for(int m=0; m<cpd->n_motors; m++) {
		lastFlagellaState->at(m)=CellParameterData::NORMAL;
	}
	db->addData(lastFlagellaState->getName(),lastFlagellaState);


	//Add data objects needed by the flagella models
	Data *minNumberOfMotorsInCCWneededToSwim_Data = new Hive::IntegerData("minNumberOfMotorsInCCWneededToSwim",cpd->minNumberOfMotorsInCCWneededToSwim);
	db->addData(minNumberOfMotorsInCCWneededToSwim_Data->getName(),minNumberOfMotorsInCCWneededToSwim_Data);


	//Only add this data if we are using the conformation model
	//Intermediate flagella objects needed by the conformation model
	if(cpd->flagellaModelType==CellParameterData::FLAGELLA_CONFORMATION_MODEL)
	{

		Data *minTimeInSemiData = new Hive::DoubleData("minTimeInSemi",cpd->minTimeInSemi);
		db->addData(minTimeInSemiData->getName(),minTimeInSemiData);

		//flagella state information
		TVectorData<double> *intervalTime = new TVectorData<double> ("intervalTime","tvector_double");
		intervalTime->reserveSize(cpd->n_motors);
		for(int m=0; m<cpd->n_motors; m++) {
			intervalTime->at(m)=1;
		}
		db->addData(intervalTime->getName(),intervalTime);

		TVectorData<double> *timeInSemi= new TVectorData<double> ("timeInSemi","tvector_double");
		timeInSemi->reserveSize(cpd->n_motors);
		for(int m=0; m<cpd->n_motors; m++) {
			timeInSemi->at(m)=1;
		}
		db->addData(timeInSemi->getName(),timeInSemi);
	}

	// Set CheYp values for the cell (or multiple if we need multiple traces per cell)
	Data *meanCheYp_Data = new Hive::DoubleData("meanCheYp",cpd->meanCheYp);
	db->addData(meanCheYp_Data->getName(),meanCheYp_Data);

	TVectorData<double> *ypConc;
	TVectorData<double> *methLevel;
	if(!cpd->isFlagellaCoordinated) {
		ypConc = new TVectorData<double>("CheYp","tvector_double");
		methLevel = new TVectorData<double>("methLevel","tvector_double");
		ypConc->reserveSize(cpd->n_motors);
		methLevel->reserveSize(cpd->n_motors);
		for (int i=0; i<cpd->n_motors; i++) {
			ypConc->at(i)=cpd->meanCheYp;
			methLevel->at(i)=2.0;
		}
	} else {
		ypConc = new TVectorData<double>("CheYp","tvector_double");
		ypConc->reserveSize(1);
		ypConc->at(0)=cpd->meanCheYp;

		methLevel = new TVectorData<double>("methLevel","tvector_double");
		methLevel->reserveSize(1);
		methLevel->at(0)=2.0;
	}
	db->addData(ypConc->getName(),ypConc);
	db->addData(methLevel->getName(),methLevel);

	//Set Signaling parameters
	Data *tau_Data = new Hive::DoubleData("tau",cpd->tau);
	db->addData(tau_Data->getName(),tau_Data);

	Data *sigma_Data = new Hive::DoubleData("sigma",cpd->sigma);
	db->addData(sigma_Data->getName(),sigma_Data);

	// Set other receptor parameters
	Data *alpha_Data = new Hive::DoubleData("alpha",cpd->alpha);
	db->addData(alpha_Data->getName(),alpha_Data);

	Data *e0_Data = new Hive::DoubleData("e0",cpd->e0);
	db->addData(e0_Data->getName(),e0_Data);

	Data *e1_Data = new Hive::DoubleData("e1",cpd->e1);
	db->addData(e1_Data->getName(),e1_Data);

	Data *KoffTar_Data = new Hive::DoubleData("KoffTar",cpd->KoffTar);
	db->addData(KoffTar_Data->getName(),KoffTar_Data);

	Data *KonTar_Data = new Hive::DoubleData("KonTar",cpd->KonTar);
	db->addData(KonTar_Data->getName(),KonTar_Data);

	Data *KoffTsr_Data = new Hive::DoubleData("KoffTsr",cpd->KoffTsr);
	db->addData(KoffTsr_Data->getName(),KoffTsr_Data);

	Data *KonTsr_Data = new Hive::DoubleData("KonTsr",cpd->KonTsr);
	db->addData(KonTsr_Data->getName(),KonTsr_Data);

	Data *TarCount_Data = new Hive::IntegerData("TarCount",cpd->TarCount);
	db->addData(TarCount_Data->getName(),TarCount_Data);

	Data *TsrCount_Data = new Hive::IntegerData("TsrCount",cpd->TsrCount);
	db->addData(TsrCount_Data->getName(),TsrCount_Data);

	// advance to the next cell
	this->currentCell++;
}

const int CellParameterData::CCW=0;
const int CellParameterData::CW=1;

const int CellParameterData::RUN=0;
const int CellParameterData::TUMBLE=1;

const int CellParameterData::NORMAL = 3;
const int CellParameterData::SEMI = 4;
const int CellParameterData::CURLY = 5;
const int CellParameterData::NONE = 6;


double CellParameterData::outputInterval = 10;
double CellParameterData::eqTime = 200;

CellParameterData::CellParameterData() {
	//Default Settings for All Cells
  
	// Motion parameters
	speed = 20;
	rotDiffConst = 0.062;


	// Flagella Parameters
	flagellaModelType = CellParameterData::FLAGELLA_CONFORMATION_MODEL;
	isFlagellaCoordinated = true;
	n_motors = 4;

	motor_Kd = 3.06;
	motor_g1 = 40;
	motor_w = 1.3;

	minNumberOfMotorsInCCWneededToSwim = 2;
	minTimeInSemi = 0.2;


	// Signaling and Noise Parameters
	tau = 15;
	sigma = 0.1;
	meanCheYp = 2.9;


	// Receptor Cluster Parameters
	alpha = 6.0;

	e0 = 1.0*(19);
	e1 = -0.45*(19);

	KoffTar   = 0.02e-3; // This is 0.02 mM or 0.00002 M
	KonTar    = 0.5e-3; // mM
	KoffTsr   = 100e-3; // mM
	KonTsr    = 1000; //Yes, 1000 M, or 10^6 mM
	TarCount  = 6;
	TsrCount  = 13;
}

CellParameterData::~CellParameterData() { }
