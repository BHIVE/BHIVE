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
	this->currentCell = 0;
}

// init e.coli cells
CellParameterInitializer::CellParameterInitializer(string inputFileName)
{
	// set some flags ... inherited from michael no idea whether this is needed
	this->isBlindAgent= false;
	this->currentCell = 0;

	// some local variables
	string line;
	string type;
	istringstream iss;

	// open up the file
	ifstream input(inputFileName.c_str());
	if (!input.is_open()) {
		throw HiveException("cannot open file named:'"+inputFileName+"' given to CellParameterInitializer","CellParameterInitializer::CellParameterInitializer");
	}
	// read as long as you encounter the type
	while (getline(input,line)) {
		Util::trim(line);
		if (line.substr(0,1) != "#" && line != "") {
			iss.clear();
			iss.str(line);
			iss >> type;
			break;
		}
	}
	cerr<<"# detected ecoli cell type as model: '" <<type<<"'"<<endl;
	// evaluate which type is called for and call the respective function
	if (type == "OLD_MODEL") {
		// set flag
		this->is_wills_methylation_model.setBool(false);
		// call parsing method
		this->parseFileOldModel(input);
	} else if (type == "WILLS_MODEL") {
		// set flag
		this->is_wills_methylation_model.setBool(true);
		// call parsing method
		this->parseFileNewModel(input);
	} else {
		// throw exception as type was not understood
		throw HiveException("unknown cell type of the ecoli given to CellParameterInitializer","CellParameterInitializer::CellParameterInitializer");
	}
}

CellParameterInitializer::~CellParameterInitializer()
{
	cells.clear();
	for (unsigned int i=0; i<uniqueCPDobjects.size(); i++) {
		delete uniqueCPDobjects[i];
		uniqueCPDobjects[i] = NULL;
	}
	uniqueCPDobjects.clear();
}

void CellParameterInitializer::parseFileOldModel(ifstream& distFile) {
	// flags for the parsing
	bool outputLines = false;
	bool outputParse = true;

	//cout<<"\n\nParsing input file in SimpleCellDefaultComposer...\n";
	try {
		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

		//open up the file ... no longer needed 20.10.2010
		// 		ifstream distFile(inputFileName.c_str());

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
				double tauAdapt;
				double tauNoise;
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
				tauAdapt = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  tau adapt :: "<<tauAdapt<<endl;

				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				tauNoise = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  tau noise :: "<<tauNoise<<endl;

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
				cpd->tauAdapt = tauAdapt;
				cpd->tauNoise = tauNoise;
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
				uniqueCPDobjects.push_back(cpd);
				for(int k=0; k<numOfCells; k++) {
					cells.push_back(cpd);
				}


			}

		} else {
			throw(Hive::HiveException("Unable to read or process input file ",
					"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
		}

	} catch (Hive::HiveException& e) {
		e.addTraceException("SimpleCellDefaultComposer::parseInputFile(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file ",
				"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
	}
	this->number_cells = cells.size();
}

void CellParameterInitializer::parseFileNewModel(ifstream& distFile) {
	// flags for the parsing
	bool outputLines = false;
	bool outputParse = false;

	//cout<<"\n\nParsing input file in SimpleCellDefaultComposer...\n";
	try {
		//Open the population file and create the cells
		const char* delimeter = ",";
		string line; int lineCounter=0;
		size_t lastFound=0, nextFound=0;

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
				double kr;
				double Kr;
				double kb;
				double Kb;
				double noise_parameter;
				double meth_init;
				double cheyp_mean;
				int numOfMotors;
				bool isCoordinated;
				int flagellaModel;
				int minFlagellaToRun;
				double semiWaitingTime;
				double cellSpeed;
				int numOfCells;

				// kr
				lastFound=line.find(delimeter); nextFound=0;
				string s = line.substr(0,lastFound); Util::trim(s);
				kr = Util::convertToDouble(s);
				if(outputParse) cout<<"  kr :: "<< kr <<endl;

				// Kr
				nextFound=line.find(delimeter, lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				Kr = Util::convertToDouble(s);
				lastFound = nextFound;
				if(outputParse) cout<<"  Kr :: "<< Kr <<endl;

				// kb
				nextFound=line.find(delimeter, lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				kb = Util::convertToDouble(s);
				lastFound = nextFound;
				if(outputParse) cout<<"  kb :: "<< kb <<endl;

				// Kb
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				Kb = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  Kb :: "<< Kb <<endl;

				// Noise parameter
				nextFound = line.find(delimeter,lastFound+1);
				s = line.substr(lastFound+1,nextFound-(lastFound+1)); Util::trim(s);
				noise_parameter = Util::convertToDouble(s);
				lastFound=nextFound;
				if(outputParse) cout<<"  noise_parameter :: "<< noise_parameter <<endl;

				// meth_init
				nextFound = line.find(delimeter, lastFound+1);
				s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
				meth_init = Util::convertToDouble(s);
				lastFound = nextFound;
				if (outputParse) cout << "  meth_init :: " << meth_init << endl;

				// mean cheyp
				nextFound = line.find(delimeter, lastFound+1);
				s = line.substr(lastFound+1, nextFound-(lastFound+1)); Util::trim(s);
				cheyp_mean = Util::convertToDouble(s);
				lastFound = nextFound;
				if(outputParse) cout << " cheyp_mean :: " << cheyp_mean << endl;

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
				if(outputParse) cout<<"  cellSpeed :: "<<cellSpeed<<endl;


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
				cpd->kr = kr;
				cpd->Kr = Kr;
				cpd->kb = kb;
				cpd->Kb = Kb;
				cpd->noise_parameter = noise_parameter;
				cpd->meth_init = meth_init;
				cpd->cheyp_mean = cheyp_mean;
				cpd->n_motors = numOfMotors;
				cpd->isFlagellaCoordinated = isCoordinated;
				cpd->flagellaModelType = flagellaModel;
				cpd->minNumberOfMotorsInCCWneededToSwim = minFlagellaToRun;
				cpd->minTimeInSemi = semiWaitingTime;
				cpd->speed = cellSpeed;
				cpd->isChemotactic = isChemotactic;

				//save the info for each cell in the cells array - no need to recreate
				//a cpd for each cell, just pass a pointer
				uniqueCPDobjects.push_back(cpd);
				for(int k=0; k<numOfCells; k++) {
					cells.push_back(cpd);
				}


			}

		} else {
			throw(Hive::HiveException("Unable to read or process input file ",
					"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
		}

	} catch (Hive::HiveException& e) {
		e.addTraceException("SimpleCellDefaultComposer::parseInputFile(string inputFileName)");
		throw(e);
	} catch (...) {
		throw(Hive::HiveException("Error in reading or processing input file ",
				"SimpleCellDefaultComposer::parseInputFile(string inputFileName)"));
	}
	this->number_cells = cells.size();
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

	// add flag about methylation model
	BoolData *is_wills_methylation_model = new Hive::BoolData("is_wills_methylation_model",
				this->is_wills_methylation_model.getBool());
	db->addData(is_wills_methylation_model);


	// add special cell accumulated data
	TVectorData<double> *cell_acc_ligand = new TVectorData<double> ("cell_acc_ligand","tvector_double");
	cell_acc_ligand->reserveSize(1);
	for(int al=0; al<1; al++) {
		cell_acc_ligand->at(al)=0;
	}
	db->addData(cell_acc_ligand);

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


	// the following information has to be added, if we are dealing with the old model
	if (!this->is_wills_methylation_model.getBool()) {
		//Set Signaling parameters
		Data *tauAdapt_Data = new Hive::DoubleData("tauAdapt",cpd->tauAdapt);
		db->addData(tauAdapt_Data);

		Data *tauNoise_Data = new Hive::DoubleData("tauNoise",cpd->tauNoise);
		db->addData(tauNoise_Data);

		Data *sigma_Data = new Hive::DoubleData("sigma",cpd->sigma);
		db->addData(sigma_Data);
	} else {						// end of data needed for old model
		// now it is the new model!
		// add specific data for r&b model
		Data *kr = new Hive::DoubleData("kr", cpd->kr);
		Data *Kr = new Hive::DoubleData("Kr", cpd->Kr);
		Data *kb = new Hive::DoubleData("kb", cpd->kb);
		Data *Kb = new Hive::DoubleData("Kb", cpd->Kb);
		Data *noise_parameter = new Hive::DoubleData("noise_parameter", cpd->noise_parameter);
		Data *meth_init = new Hive::DoubleData("meth_init", cpd->meth_init);
		Data *cheyp_mean = new Hive::DoubleData("cheyp_mean", cpd->cheyp_mean);
		db->addData(kr);
		db->addData(Kr);
		db->addData(kb);
		db->addData(Kb);
		db->addData(noise_parameter);
		db->addData(meth_init);
		db->addData(cheyp_mean);

		Data *tauNoise_Data = new Hive::DoubleData("tauNoise",cpd->tauNoise);
		db->addData(tauNoise_Data);
	}

	// Set other receptor parameters
	Data *alpha_Data = new Hive::DoubleData("alpha",cpd->alpha);
	db->addData(alpha_Data->getName(),alpha_Data);

	/// change 10-31-2010: in will's r&b model the alpha value is not 6.0 but 5.7.
	/// is this always the case? anyways if change it here to that value
	if (this->is_wills_methylation_model.getBool()) {
		((Hive::DoubleData *) alpha_Data)->setDouble(5.7);
	}

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

	Data *a0_data = new Hive::DoubleData("a0", cpd->a0);
	db->addData(a0_data->getName(), a0_data);

	Data *y0_data = new Hive::DoubleData("y0", cpd->y0);
	db->addData(y0_data->getName(), y0_data);

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

	motor_Kd = 3.15; // 3.06;
	motor_g1 = 28; //40;
	motor_w = 1.3;

	minNumberOfMotorsInCCWneededToSwim = 2;
	minTimeInSemi = 0.2;


	// Signaling and Noise Parameters
	tauAdapt = 15;
	tauNoise = 30;
	sigma = 0.1;
	meanCheYp = 2.7;


	// Receptor Cluster Parameters
	alpha = 6.0;

	/// old values
	//	e0 = 1.0*(19);
	//	e1 = -0.45*(19);

	e0 =  5.36989;    // 4.77; // 3.29;
	e1 =  -6.80748;   // -6.66; /// -2.81;


	KoffTar   = 0.0223e-3;  /// 0.024e-3;   /// 0.011e-3;    // 0.01e-3; // This is 0.02 mM or 0.00002 M
	KonTar    = 0.885e-3;   /// 0.84e-3;    /// 0.025e-3; // mM
	KoffTsr   = 153.1e-3;   /// 123.1e-3;   /// 42.8e-3; // mM
	KonTsr    = 241.9e-3;   /// 186.7e-3;   /// 65.6e-3; //Yes, 1000 M, or 10^6 mM
	TarCount  = 6;
	TsrCount  = 13;

	a0 = 0.165;
	y0 = 4.77;
}

CellParameterData::~CellParameterData() { }
