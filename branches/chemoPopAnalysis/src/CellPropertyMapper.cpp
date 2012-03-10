/*
 * CellPropertyMapper.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: msneddon
 */


#include <exception>
#include <sstream>
#include <stdlib.h>
#include <math.h>

#include "CellPropertyMapper.hh"


// helper function for parsing
void trim(std::string& str) {
	size_t startpos = str.find_first_not_of(" \t");
	size_t endpos = str.find_last_not_of(" \t");
	if(( string::npos == startpos ) || (string::npos == endpos)) {
		str="";
	} else {
		str = str.substr(startpos,endpos-startpos+1);
	}
}

double convertToDouble(const std::string& s)
{
	bool failIfLeftoverChars = true;
	std::istringstream i(s);
	double x;
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c))) {
		cerr<<"Cannot convert string '"<<s<<"' to double value."<<endl;
		exit(1);
	}
	return x;
}

int convertToInt(const std::string& s)
{
	bool failIfLeftoverChars = true;
	std::istringstream i(s);
	int x;
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c))){
		cerr<<"Cannot convert string '"<<s<<"' to double value."<<endl;
		exit(1);
	}
	return x;
}


double findBinKey(double value, vector <double> &binCenters)
{
	double minDiff = 9e99;
	double key = 0;
	for(unsigned int i=0; i<binCenters.size(); i++)
	{

		if( fabs((binCenters.at(i)-value)) < minDiff )
		{
			minDiff = fabs(binCenters.at(i)-value);
			key = binCenters.at(i);
		}

	}
	//cout<<"minDiff: " << minDiff<<endl;
	//cout<<"key: " <<key<<endl;
	return key;
}







CellPropertyMapper::CellPropertyMapper()
{
	populationFileName="";
	n_cells = -1;
}

CellPropertyMapper::~CellPropertyMapper() { }


void CellPropertyMapper::openFileStream(string populationFileName)
{
	this->populationFileName = populationFileName;
	try
	{
		populationFileStream.open(populationFileName.c_str());
		if (!populationFileStream.is_open()) {
			cerr<<"!! could not open stream to population file named: '"<<populationFileName<<"'"<<endl;
			exit(1);
		}

	} catch (...) {
		cerr<<"!! unknown error when openning stream to population file named: '"<<populationFileName<<"'"<<endl;
		exit(1);
	}
}

void CellPropertyMapper::closeFileStream()
{
	try
	{
		n_cells = -1;
		populationFileStream.close();
	} catch (...) {
		cerr<<"!! unknown error when openning stream to population file named: '"<<populationFileName<<"'"<<endl;
		exit(1);
	}
}


int CellPropertyMapper::get_n_cells()
{
	if(this->n_cells>=0) return this->n_cells;

	if(!populationFileStream.is_open()) {
		cerr<<"cannot get_n_cells() in CellPropertyMapper: no file is currently open."<<endl;
		exit(1);
	}

	int totalCellCount = 0;
	populationFileStream.clear();
	populationFileStream.seekg(ios_base::beg);


	const char* delimeter = ",";
	string line;
	size_t lastFound=0, nextFound=0;
	while (! populationFileStream.eof() )
	{
		getline(populationFileStream,line);
		trim(line);
		if(line.size()==0) continue;
		if(line.at(0)=='#'||line.at(0)=='%') continue;


		int numOfCells;

		//CheY-P level
		lastFound=line.find(delimeter); nextFound=0;
		string s = line.substr(0,lastFound);

		// Tau
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		// Noise
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//numOfMotors
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//isCoordinated
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//flagellaModel
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//minFlagellaToRun
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//semiWaitingTime
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		//cellSpeed
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1));
		lastFound=nextFound;

		// number of Cells
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		numOfCells = convertToInt(s);
		lastFound=nextFound;

		totalCellCount += numOfCells;
	}

	this->n_cells = totalCellCount;
	return totalCellCount;
}


void CellPropertyMapper::constructDefaultMap(map<double, vector<int> >& default_map)
{
	int local_n_cells = this->get_n_cells();

	vector<int> list;
	for(int k=0; k<local_n_cells; k++)
		list.push_back(k);

	default_map[0]=list;
}


void CellPropertyMapper::parse(vector <double> &binCenters,map <double,vector<int> > &popMap,int field)
{
	//clear the tauMap, so that we start fresh
	popMap.clear();

	if(!populationFileStream.is_open()) {
		cerr<<"cannot parse() in CellPropertyMapper: no file is currently open."<<endl;
		exit(1);
	}

	//add all elements of the binCenters to the tauMap
	for(unsigned int k=0; k<binCenters.size(); k++)
	{
		vector<int> list;
		popMap[binCenters.at(k)]=list;
	}


	//first, place our pointer at the begging of the string buffer
	populationFileStream.clear();
	populationFileStream.seekg(ios_base::beg);

	bool outputLines = false;
	bool outputParse = false;

	int cellCounter = 0;
	const char* delimeter = ",";
	string line; int lineCounter=0;
	size_t lastFound=0, nextFound=0;
	while (! populationFileStream.eof() )
	{
		getline(populationFileStream,line);
		if(outputLines) cout<<"["<<lineCounter<<"]: "<<line<<"\n";
		lineCounter++;
		if(line.size()==0) continue;
		trim(line);

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
		string s = line.substr(0,lastFound); trim(s);
		meanCheYp = convertToDouble(s);
		if(outputParse) cout<<"  Mean [CheYp] :: "<<meanCheYp<<endl;

		// Tau
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		tau = convertToDouble(s);
		lastFound=nextFound;
		if(outputParse) cout<<"  tau :: "<<tau<<endl;

		// Noise
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		noise = convertToDouble(s);
		if(outputParse) cout<<"  noise :: "<<noise<<endl;
		lastFound=nextFound;

		//numOfMotors
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		numOfMotors = convertToInt(s);
		if(outputParse) cout<<"  numOfMotors :: "<<numOfMotors<<endl;
		lastFound=nextFound;

		//isCoordinated
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		if(convertToInt(s)==0) isCoordinated = false; else isCoordinated = true;
		if(outputParse) cout<<"  isCoordinated :: "<<isCoordinated<<endl;
		lastFound=nextFound;

		//flagellaModel
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		flagellaModel = convertToInt(s);
		if(outputParse) cout<<"  flagellaModel :: "<<flagellaModel<<endl;
		lastFound=nextFound;

		//minFlagellaToRun
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		minFlagellaToRun = convertToInt(s);
		if(outputParse) cout<<"  minFlagellaToRun :: "<<minFlagellaToRun<<endl;
		lastFound=nextFound;

		//semiWaitingTime
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		semiWaitingTime = convertToDouble(s);
		if(outputParse) cout<<"  semiWaitingTime :: "<<semiWaitingTime<<endl;
		lastFound=nextFound;

		//cellSpeed
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		cellSpeed = convertToDouble(s);
		if(outputParse) cout<<"  cellSpeed :: "<<semiWaitingTime<<endl;
		lastFound=nextFound;

		// number of Cells
		nextFound = line.find(delimeter,lastFound+1);
		s = line.substr(lastFound+1,nextFound-(lastFound+1)); trim(s);
		numOfCells = convertToInt(s);
		if(outputParse) cout<<"  numOfCells :: "<<numOfCells<<endl;
		lastFound=nextFound;


		//Identify what field we are matching, and what bin to place
		//this set of cells into
		double key=0;
		if(field==CellPropertyMapper::TAU) {
			key = findBinKey(tau,binCenters);

		} else if(field==CellPropertyMapper::NOISE) {
			key = findBinKey(noise,binCenters);

		} else if(field==CellPropertyMapper::NUMOFMOTORS) {
			key = findBinKey(numOfMotors,binCenters);

		} else if(field==CellPropertyMapper::ISCOORDINATED) {
			key = findBinKey((double)isCoordinated,binCenters);

		} else if(field==CellPropertyMapper::FLAGELLAMODEL) {
			key = findBinKey((double)flagellaModel,binCenters);

		} else if(field==CellPropertyMapper::MINFLAGELLATORUN) {
			key = findBinKey((double)minFlagellaToRun,binCenters);

		} else if(field==CellPropertyMapper::SEMIWAITINGTIME) {
			key = findBinKey(semiWaitingTime,binCenters);

		} else if(field==CellPropertyMapper::CELLSPEED) {
			key = findBinKey(cellSpeed,binCenters);

		} else if(field==CellPropertyMapper::MEANCHEYP) {
			key = findBinKey(meanCheYp,binCenters);

		}


		vector <int> *cellIds = &(popMap.at(key));
		int newTotalCells = cellCounter + numOfCells;
		for (int c=cellCounter; c<newTotalCells; c++) {
			cellIds->push_back(cellCounter++);
		}

		if(outputParse)cout<<"currentCellCounter: "<<cellCounter<<endl;
	}
	if(outputLines) cout<<"EOF"<<endl<<endl<<endl;

}




// creates a map where the key is the values of the binCenters and
// the data is a vector of integers giving the position of the cell
// in the other output data files.
void CellPropertyMapper::constructTauMap(vector <double> &binCenters, map <double,vector<int> > &tauMap)
{
	//parse file with correct field flag...
	parse(binCenters,tauMap,CellPropertyMapper::TAU);
}

void CellPropertyMapper::constructCheYpMap(vector <double> &binCenters, map <double,vector<int> > &cheYpMap)
{
	//parse file with correct field flag...
	this->parse(binCenters,cheYpMap,CellPropertyMapper::MEANCHEYP);
}

void CellPropertyMapper::constructNoiseMap(vector <double> &binCenters, map <double,vector<int> > &noiseMap)
{
	//parse file with correct field flag...
	parse(binCenters,noiseMap,CellPropertyMapper::NOISE);
}



void CellPropertyMapper::printMap(map <double,vector<int> > &popMap)
{
	map<double,vector<int> >::iterator iter;
	for(iter=popMap.begin(); iter!=popMap.end(); iter++)
	{
		cout<<"["<<iter->first<<"]: ";
		vector <int> *ids = &iter->second;
		for(unsigned int k=0; k<ids->size(); k++)
		{
			cout<<" "<<ids->at(k);
		}
		cout<<endl;
	}
}






