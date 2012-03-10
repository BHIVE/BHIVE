/*
 * commandLineParser.cpp
 *
 *  Created on: Oct 21, 2008
 *      Author: msneddon
 */

#include "NFinput.hh"
#include "../../../exception/exception.hh"


using namespace Hive;
using namespace NF;
using namespace std;


bool NF::parseArguments(int argc, const char *argv[], map<string,string> &argMap)
{
	for(int a=1; a<argc; a++)
	{
		string s(argv[a]);

		//Find the strings that start with a minus, these are the flags
		if(s.compare(0,1,"-")==0)
		{
			string sFlag = s.substr(1,s.size()-1);
			if(sFlag.empty()) {
				cout<<"Warning: possible error in arguments.  You gave a '-' with a space"<<endl;
				cout<<"directly following. This is not a valid argument."<<endl<<endl;
				continue;
			}


			//See if the flag has some other input value that follows
			string sVal;
			if((a+1)<argc) {
				sVal=argv[a+1];
			}
			if(sVal.compare(0,1,"-")==0) {
				sVal = "";
			}

			//cout<<"found:  '"<<sFlag<<"' with arg: '"<<sVal<<"' "<<endl;
			if(argMap.find(sFlag)!=argMap.end()) {
				cout<<"Found two values for the same flag: '"<<sFlag<<"' so I am stopping"<<endl;
				return false;
			}

			argMap[sFlag] = sVal;
		}
	}
	return true;
}



int NF::parseAsInt(map<string,string> &argMap,string argName,int defaultValue)
{
	if(argMap.find(argName)!=argMap.end()) {
		string strVal = argMap.find(argName)->second;
		try {
			int intVal = Util::convertToInt(strVal);
			return intVal;
		} catch (HiveException &e) {
			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as an integer."<<endl;
			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
		}
	}
	return defaultValue;
}

double NF::parseAsDouble(map<string,string> &argMap,string argName,double defaultValue)
{
	if(argMap.find(argName)!=argMap.end()) {
		string strVal = argMap.find(argName)->second;
		try {
			double doubleVal = Util::convertToDouble(strVal);
			return doubleVal;
		} catch (HiveException &e) {
			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as a double."<<endl;
			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
		}
	}
	return defaultValue;
}



bool NF::parseSequence(string numString, vector <double> &outputTimes)
{
	double startVal=0, stepVal=1, endVal=0;
	try {

		int c1 = numString.find_first_of(':');
		if(c1!=string::npos) {
			int c2 = numString.find_first_of(':',c1+1);
			if(c2!=string::npos) {
					startVal= Util::convertToDouble(numString.substr(0,c1));
					stepVal= Util::convertToDouble(numString.substr(c1+1,c2-c1-1));
					endVal= Util::convertToDouble(numString.substr(c2+1));

			} else {
					startVal= Util::convertToDouble(numString.substr(0,c1));
					endVal= Util::convertToDouble(numString.substr(c1+1));
			}
		}

	} catch(HiveException &e) {
		return false;
	}

	if(startVal>endVal) {
		cout<<"Error: start value of sequence must be <= end value."<<endl;
		return false;
	} else if(stepVal<=0) {
		cout<<"Error: step value of sequence must be >0."<<endl;
		return false;
	} else {
		if(outputTimes.size()>=1)
			if(startVal<=outputTimes.at(outputTimes.size()-1)) {
				cout<<"\n\nError in NF::creatComplexOutputDumper: output times given \n";
				cout<<"must be monotonically increasing without any repeated elements.";
				return false;
			}
		//Only if everything went as planned to we then add the output steps accordingly
		for(double d=startVal; d<=endVal; d+=stepVal) {
			outputTimes.push_back(d);
		}
		return true;
	}
	return true;
}




bool NF::createSystemDumper(string paramStr, System *s, bool verbose)
{
	if(verbose) cout<<"Parsing system dump flag: "<<paramStr<<"\n";

	//First, extract out the times the user wants to dump the observables
	int b1 = paramStr.find_first_of('[');
	int b2 = paramStr.find_first_of(']');
	if(b1>b2) { cout<<"Error in NF::createSystemDumper:, ']' was found before '['."<<endl; return false; }
	if(b1<0 && b2<0) { cout<<"Error in NF::createSystemDumper:, enclosing brackets '[' and ']' were not found."<<endl; return false; }
	if(b1<0) { cout<<"Error in NF::createSystemDumper:, '[' was not found."<<endl; return false; }
	if(b2<0) { cout<<"Error in NF::createSystemDumper:, ']' was not found."<<endl; return false; }
	string timesStr=paramStr.substr(b1+1,b2-b1-1);



	//Now find path to the folder we are dumping to
	string pathToFolder = paramStr.substr(b2+1);
	if(pathToFolder.size()!=0) {
		int arrowPos = pathToFolder.find("->");
		if(arrowPos!=string::npos) {
			pathToFolder = pathToFolder.substr(arrowPos+2);
		} else {
			cout<<"Warning: path to folder ("+pathToFolder+") is not written correctly."<<endl;
			cout<<"Should be written as: [t1;t2;]->/path/to/folder/"<<endl;
			cout<<"no system dumps were scheduled."<<endl;
			return true;
		}
	}


	//Create a vector storing the output times
	string numString=""; vector <double> outputTimes;
	if(verbose) { cout<<"  scheduling system dumps at simulation times:"; }
	if(pathToFolder.size()>0) { cout<<"scheduling system dumps to directory ("+pathToFolder+")"<<endl; }
	else { cout<<"scheduling system dumps to directory (.)"<<endl; }
	for(unsigned int i=0; i<timesStr.length(); i++) {
		if(timesStr.at(i)==';') {
			if(numString.size()==0) continue;
			try {
				double doubleVal = Util::convertToDouble(numString);
				if(outputTimes.size()>0) {
					if(doubleVal<=outputTimes.at(outputTimes.size()-1)) {
						cout<<"\n\nError in NF::creatComplexOutputDumper: output times given \n";
						cout<<"must be monotonically increasing without any repeated elements.";
						return false;
					}
				}
				outputTimes.push_back(doubleVal);
			} catch (HiveException &e) {
				//could not parse it directly as a double, so first we also have to try parsing
				//it as a matlab style sequence...
				bool success = parseSequence(numString, outputTimes);
				if(!success) {
					cout<<"\nWarning in NF::creatComplexOutputDumper: could not parse time: '"<<numString<<"'"<<endl;
					cout<<"Ignoring that element."<<endl;
				}
			}
			numString="";
			continue;
		}
		numString += timesStr.at(i);
	}
	if(numString.size()!=0) {
		try {
			double doubleVal = Util::convertToDouble(numString);
			if(outputTimes.size()>0) {
				if(doubleVal<=outputTimes.at(outputTimes.size()-1)) {
					cout<<"\nError in NF::creatComplexOutputDumper: output times given ";
					cout<<"must be monotonically increasing without any repeated elements.";
					return false;
				}
			}
			outputTimes.push_back(doubleVal);
		} catch (HiveException &e) {
			//try to parse it as a matlab style sequence
			bool success = parseSequence(numString, outputTimes);
			if(!success) {
				cout<<"\nWarning in NF::creatComplexOutputDumper: could not parse time: '"<<numString<<"'"<<endl;
				cout<<"Ignoring that element."<<endl;
			}
		}
	}
	if(verbose) {
		if(outputTimes.size()==0) {
			cout<<" none given.";
		} else {
			for(unsigned int i=0; i<outputTimes.size(); i++)
				cout<<" "<<outputTimes.at(i)<<";";
		}
		cout<<endl;
	}




	//Here is where we actually create the system dumper
	DumpSystem *ds = new DumpSystem(s, outputTimes, pathToFolder, verbose);
	s->setDumpOutputter(ds);
	return true;

}

