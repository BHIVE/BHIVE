/**
 * @file dataParser.cpp
 *
 * Functions to parse a file and automatically create data objects to insert to
 * an existing agent database.
 *
 * @date Dec 2011
 *
 * @author Michael Sneddon
 */


#include "dataParser.hh"

#include <fstream>

using namespace std;
using namespace Hive;









void DataParser::parseSingleLine(const string line, string &data_type, string &data_name, string &data_value, const string line_number, const string filename)
{
	size_t delimiterSpacePos = line.find_first_of(' ');
	size_t delimiterTabPos = line.find_first_of('\t');
	string theRest = "";

	if(delimiterSpacePos!=string::npos) {
		data_type = line.substr(0,delimiterSpacePos);
		theRest = line.substr(delimiterSpacePos,line.size()-delimiterSpacePos);
	} else if (delimiterTabPos!=string::npos) {
		data_type = line.substr(0,delimiterTabPos);
		theRest = line.substr(delimiterTabPos,line.size()-delimiterTabPos);
	} else {
		throw HiveException("# ERROR! : On line "+ line_number + " of '" + filename+"', cannot parse: '"+line+"'.", "DataParser::parse(string filename, Agent *a, bool verbose)");
	}
	Util::trim(theRest);

	delimiterSpacePos = theRest.find_first_of(' ');
	delimiterTabPos = theRest.find_first_of('\t');
	if(delimiterSpacePos!=string::npos) {
		data_name = theRest.substr(0,delimiterSpacePos);
		theRest = theRest.substr(delimiterSpacePos,line.size()-delimiterSpacePos);
	} else if (delimiterTabPos!=string::npos) {
		data_name = theRest.substr(0,delimiterTabPos);
		theRest = theRest.substr(delimiterTabPos,line.size()-delimiterTabPos);
	} else {
		throw HiveException("# ERROR! : On line "+ line_number + " of '" + filename+"', cannot parse: '"+line+"'.", "DataParser::parse(string filename, Agent *a, bool verbose)");
	}

	data_value = theRest;

	Util::trim(data_type);
	Util::trim(data_name);
	Util::trim(data_value);
}






void DataParser::parse(string filename, Agent *a, bool verbose=false)
{
	// first get the agent database which we will use later and make sure it exists
	Database *db;
	if(a!=NULL) {
		db = a->getDatabase();
	} else {
		throw HiveException("# ERROR! : Given agent pointer is null!", "DataParser::parse(string filename, Agent *a, bool verbose)");
	}

	// open input stream to read the file
	ifstream input;
	input.open(filename.c_str());
	if(!input.is_open()) {
		throw HiveException("# ERROR! : Cannot open file: '"+filename+"'.", "DataParser::parse(string filename, Agent *a, bool verbose)");
	}
	if(verbose) cerr<<"# reading file: '"+filename+"' in DataParser::parse to initialize Agent "<<a->getAgentId()<<"."<<endl;


	// loop through the entire file and parse it
	istringstream iss;
	string line;
	string data_type;
	string data_name;
	string data_value;
	int lineNumber = 0;
	while (getline(input, line)) {

		// first, remember the line number and convert the line number to a string.... gotta love c++.
		lineNumber++;
		stringstream ss; ss << lineNumber; string slineNumber(ss.str());
		//cerr<<"UNFORMATED: '"<<line<<"'"<<endl;

		// eliminate anything that follows a pound because this is a comment
		size_t poundPos = line.find_first_of('#');
		if(poundPos!=string::npos) {
			line = line.substr(0,poundPos);
		}

		// get rid of trailing and leading whitespace
		Util::trim(line);
		//cerr<<"REMOVED COMMENTS: '"<<line<<"'"<<endl;

		// if the line is still not empty, then attempt to parse it
		if (line != "") {
			parseSingleLine(line,data_type,data_name,data_value,slineNumber,filename);

			// figure out what to create or add here!
			bool identifiedDataType = false;
			try {
				/////////////////////////////////////////////////
				if(data_type=="IntegerData") {
					identifiedDataType = true;
					if(verbose) cerr<<"#    detected initialization of IntegerData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
					if(db->existsDataItem(data_name)) {
						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
						if(db->getDataItem(data_name)->getType()!="IntegerType") {
							throw HiveException("# ERROR! : On line "+ slineNumber + " of '" + filename+"', trying to overwrite IntegerData, but existing data item is of type: "+db->getDataItem(data_name)->getType()+". ", "DataParser::parse(string filename, Agent *a, bool verbose)");
						}
						db->getDataItem(data_name)->initializeFromString(data_value);
					} else {
						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
						Data *d = new IntegerData(data_name,0);
						d->initializeFromString(data_value);
						db->addData(d);
					}
				/////////////////////////////////////////////////
				} else if (data_type=="DoubleData") {
					identifiedDataType = true;
					if(verbose) cerr<<"#    detected initialization of DoubleData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
					if(db->existsDataItem(data_name)) {
						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
						if(db->getDataItem(data_name)->getType()!="DoubleType") {
							throw HiveException("# ERROR! : On line "+ slineNumber + " of '" + filename+"', trying to overwrite DoubleData, but existing data item is of type: "+db->getDataItem(data_name)->getType()+". ", "DataParser::parse(string filename, Agent *a, bool verbose)");
						}
						db->getDataItem(data_name)->initializeFromString(data_value);

					} else {
						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
						Data *d = new DoubleData(data_name,0.0);
						d->initializeFromString(data_value);
						db->addData(d);
					}
				/////////////////////////////////////////////////
				} else if (data_type=="BoolData") {
					identifiedDataType = true;
					if(verbose) cerr<<"#    detected initialization of BoolData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
					if(db->existsDataItem(data_name)) {
						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
						if(db->getDataItem(data_name)->getType()!="BoolType") {
							throw HiveException("# ERROR! : On line "+ slineNumber + " of '" + filename+"', trying to overwrite IntegerData, but existing data item is of type: "+db->getDataItem(data_name)->getType()+". ", "DataParser::parse(string filename, Agent *a, bool verbose)");
						}
						db->getDataItem(data_name)->initializeFromString(data_value);
					} else {
						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
						Data *d = new BoolData(data_name,0.0);
						d->initializeFromString(data_value);
						db->addData(d);
					}
				/////////////////////////////////////////////////
				} else if (data_type=="StringData") {
					identifiedDataType = true;
					if(verbose) cerr<<"#    detected initialization of StringData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
					if(db->existsDataItem(data_name)) {
						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
						if(db->getDataItem(data_name)->getType()!="StringType") {
							throw HiveException("# ERROR! : On line "+ slineNumber + " of '" + filename+"', trying to overwrite StringData, but existing data item is of type: "+db->getDataItem(data_name)->getType()+". ", "DataParser::parse(string filename, Agent *a, bool verbose)");
						}
						db->getDataItem(data_name)->initializeFromString(data_value);
					} else {
						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
						Data *d = new StringData(data_name,"");
						d->initializeFromString(data_value);
						db->addData(d);
					}
				/////////////////////////////////////////////////
				}


//				ADD FUNCTIONALITY FOR OTHER DATA TYPES HERE
//				else if (data_type=="TVectorData<IntegerData>") {
//					identifiedDataType = true;
//					if(verbose) cerr<<"#    detected initialization of StringData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
//					if(db->existsDataItem(data_name)) {
//						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
//						db->getDataItem(data_name)->initializeFromString(data_value);
//					} else {
//						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
//						Data *d = new StringData(data_name,"");
//						d->initializeFromString(data_value);
//						db->addData(d);
//					}
//				}
//					/////////////////////////////////////////////////
//				} else if (data_type=="TVectorData<DoubleData>") {
//					identifiedDataType = true;
//					if(verbose) cerr<<"#    detected initialization of StringData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
//					if(db->existsDataItem(data_name)) {
//						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
//						db->getDataItem(data_name)->initializeFromString(data_value);
//					} else {
//						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
//						Data *d = new StringData(data_name,"");
//						d->initializeFromString(data_value);
//						db->addData(d);
//					}
//					/////////////////////////////////////////////////
//				} else if (data_type=="TVectorData<BoolData>") {
//					identifiedDataType = true;
//					if(verbose) cerr<<"#    detected initialization of StringData named: '"<<data_name<<"' with value: '"<<data_value<<"'"<<endl;
//					if(db->existsDataItem(data_name)) {
//						if(verbose) cerr<<"#    data found in database, overwriting existing value."<<endl;
//						db->getDataItem(data_name)->initializeFromString(data_value);
//					} else {
//						if(verbose) cerr<<"#    WARNING!! data not found in database, creating new entry- check if this is correct"<<endl;
//						Data *d = new StringData(data_name,"");
//						d->initializeFromString(data_value);
//						db->addData(d);
//					}
//				}


			} catch (HiveException& e) {
				e.addTraceException("On line "+ slineNumber + " of '" + filename+"', DataParser::parse(string filename, Agent *a, bool verbose)");
				throw e;
			}
			if(!identifiedDataType) {
				throw HiveException("# ERROR! : On line "+ slineNumber + " of '" + filename+"', cannot identify data type.  Check your spelling and note parser is case sensitive.", "DataParser::parse(string filename, Agent *a, bool verbose)");
			}


		}
		//else {
		//	cerr<<"SKIPPING"<<endl;
		//}
	}
	if(verbose) db->printDataBase(cerr);
}





//
//bool parseArguments(int argc, char *argv[], map<string,string> &argMap)
//{
//	for(int a=1; a<argc; a++)
//	{
//		string s(argv[a]);
//
//		//Find the strings that start with a minus, these are the flags
//		if(s.compare(0,1,"-")==0)
//		{
//			string sFlag = s.substr(1,s.size()-1);
//			if(sFlag.empty()) {
//				cout<<"Warning: possible error in arguments.  You gave a '-' with a space"<<endl;
//				cout<<"directly following. This is not a valid argument."<<endl<<endl;
//				continue;
//			}
//
//
//			//See if the flag has some other input value that follows
//			string sVal;
//			if((a+1)<argc) {
//				sVal=argv[a+1];
//			}
//			if(sVal.compare(0,1,"-")==0) {
//				sVal = "";
//			}
//
//			//cout<<"found:  '"<<sFlag<<"' with arg: '"<<sVal<<"' "<<endl;
//			if(argMap.find(sFlag)!=argMap.end()) {
//				cout<<"Found two values for the same flag: '"<<sFlag<<"' so I am stopping"<<endl;
//				return false;
//			}
//
//			argMap[sFlag] = sVal;
//		}
//	}
//	return true;
//}
//
//
//
//int parseAsInt(map<string,string> &argMap,string argName,int defaultValue)
//{
//	if(argMap.find(argName)!=argMap.end()) {
//		string strVal = argMap.find(argName)->second;
//		try {
//			int intVal = Util::convertToInt(strVal);
//			return intVal;
//		} catch (std::runtime_error e) {
//			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as an integer."<<endl;
//			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
//		}
//	}
//	return defaultValue;
//}
//
//
//double parseAsDouble(map<string,string> &argMap,string argName,double defaultValue)
//{
//	if(argMap.find(argName)!=argMap.end()) {
//		string strVal = argMap.find(argName)->second;
//		try {
//			double doubleVal = Util::convertToDouble(strVal);
//			return doubleVal;
//		} catch (std::runtime_error e) {
//			cout<<endl<<"!!  Warning: I couldn't parse your flag '-"+argName+" "+strVal+"' as a double."<<endl;
//			cout<<"!!  Using the default value of "<<defaultValue<<endl<<endl;;
//		}
//	}
//	return defaultValue;
//}
