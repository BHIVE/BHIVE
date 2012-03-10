/**
 * @file commandLineParser.hh
 *
 * Functions to parse command line arguments into a general map that can be
 * accessed later.
 *
 * @date Oct 14th, 2009   last edited: Oct 14th, 2009
 *
 * @author Michael Sneddon
 */


#include "commandLineParser.hh"

#include <iostream>


using namespace std;
using namespace Hive;

bool parseArguments(int argc, const char *argv[], map<string,string> &argMap)
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



int parseAsInt(map<string,string> &argMap,string argName,int defaultValue)
{
	if(argMap.find(argName)!=argMap.end()) {
		string strVal = argMap.find(argName)->second;
		try {
			int intVal = Util::convertToInt(strVal);
			return intVal;
		} catch (std::runtime_error e) {
			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as an integer."<<endl;
			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
		}
	}
	return defaultValue;
}


double parseAsDouble(map<string,string> &argMap,string argName,double defaultValue)
{
	if(argMap.find(argName)!=argMap.end()) {
		string strVal = argMap.find(argName)->second;
		try {
			double doubleVal = Util::convertToDouble(strVal);
			return doubleVal;
		} catch (std::runtime_error e) {
			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as a double."<<endl;
			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
		}
	}
	return defaultValue;
}
