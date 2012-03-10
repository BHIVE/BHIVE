#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include "global_data.hh"

using namespace std;
using namespace Hive;

/*
DESCRIPTION:
Constructor
*/
GlobalData::GlobalData() {
	currentPropagator = NULL;
	currentWriter = NULL;
};

/*
DESCRIPTION:
Destructor
*/
GlobalData::~GlobalData() {
	if (currentPropagator != NULL) {
		delete currentPropagator;
	}
	if (currentWriter != NULL) {
		delete currentWriter;
	}
	if (outputStream.is_open()) {
		outputStream.close();
	}
};

/*
DESCRIPTION:
Instantiates all objects in GlobalData
OUTPUT: 
Returns an error code as an integer equal to SUCCESS if successful and FAIL is failed
*/
int GlobalData::initialize(int argc, char *argv[]) {
	vector<string> filenames;
	for (int i=0; i < argc; i++) {
		string temp(argv[i]);
		if (temp.compare("-p") == 0 && i < (argc-1)) {
			filenames.push_back(argv[i+1]);
			i++;
		} else if (temp.compare("-i") == 0 && i < (argc-1)) {
			ifstream Input(argv[i+1]);
			if (Input.is_open()) {
				do {
					string temptemp = this->getFileline(Input);
					if (temptemp.length() > 0) {
						filenames.push_back();
					}
				} while(!Input.eof());
				Input.close();
			}
			i++;
		}
	}
	if (this->loadParameters(filenames) != SUCCESS) {
		return FAIL;
	}
	this->currentPropagator = new PropagatorClass();
	this->currentWriter = new OutputWriter(this->getParameter("max printing CPU"),Hive::mpirank(),Hive::mpisize());
	if (Hive::mpirank() == 0) {
		outputStream.open(this->getParameter("output stream"));
		outputStream << this->getParameter("output stream heading") << endl;
	}
}

/*
DESCRIPTION:

INPUT: 
1.) A vector of the files containing parameter values listed in the order they should be read
OUTPUT: 
Returns an error code as an integer equal to SUCCESS if successful and FAIL is failed
*/
int GlobalData::loadParameters(vector<string> filenames) {
	//Reading in each file listed, noting that files read later will overwrite parameters in earlier files
	for (int i=0; i < int(filenames.size()); i++) {
		ifstream Input(filenames[i].data());
		if (Input.is_open()) {
			//Reading in all parameters and storing them in a map
			do {
				vector<string> Strings = this->getStringsFileline(Input, "|",false);
				if (int(Strings.size()) >= 2) {
					Parameters[ Strings[0] ] = Strings[1];
				}
			} while(!Input.eof());
			Input.close();
		}
	}
	return SUCCESS;
}

/*
DESCRIPTION:

OUTPUT: 

*/
OutputWriter* GlobalData::getWriter() {
	return currentWriter;
};

/*
DESCRIPTION:

OUTPUT: 

*/
PropagatorClass* GlobalData::getPropagator() {
	return currentPropagator;
};

/*
DESCRIPTION:

INPUT: 

OUTPUT: 

*/
string GlobalData::getParameter(string parameter) {
	return parameters[parameter];
};

/*
DESCRIPTION:

OUTPUT: 

*/
ofstream& GlobalData::getOutputStream() {
	return outputStream;
};

string GlobalData::getFileline(ifstream &input) {
	string buff; 
	getline( input, buff );
	return buff;
}

vector<string> GlobalData::setStringsFileline(ifstream &input, const char* delim, bool treatConsecutiveDelimAsOne) {
	string buff = this->getFileline(input);
	return this->stringToStrings(buff, delim, treatConsecutiveDelimAsOne);
}

vector<string> GlobalData::stringToStrings(string fullString, const char* delim, bool treatConsecutiveDelimAsOne) {
	vector<string> newVect;
	string buff(fullString);
	int location;
	do {
		location = int(buff.find_first_of(delim));
		if (location != -1) {
			if (location == 0) {
				if (!treatConsecutiveDelimAsOne) {
					string newString;
					newVect.push_back(newString);
				}
				buff = buff.substr(location+1, buff.length()-(location+1));
			} else {
				string newString = buff.substr(0, location);
				newVect.push_back(NewString);
				buff = buff.substr(location+1, buff.length()-(location+1));
			}
		}
	} while(location != -1);
	if (buff.length() != 0 || !treatConsecutiveDelimAsOne) {
		newVect.push_back(buff);
	}
	return newVect;
}