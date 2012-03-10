/**
 * @file  database.cpp
 *
 * This file contains the implementation for the Database class.
 *
 *
 * @author Michael Sneddon
 *
 * @date Started: Oct 14th, 2009   Last edited: Jan 19, 2010 by garrit
 *
 */


#include "database.hh"

using namespace std;
using namespace Hive;


Database::Database()
{
	//cerr << "# creating a database." << endl;
}

	/// Destructor
Database::~Database()
{
	//cerr << "# killing a database" << endl;
	//cerr << "# there are " << real_database.size() << " entries in the database" << endl;
	clearData();
	//cerr << "# database is now clear" << endl;
}

void Database::clearData() {
	for (unsigned int i=0; i<this->real_database.size(); i++) {
		if (real_database[i] != NULL) {
			delete real_database[i];
			real_database[i] = NULL;
		}
	}
//	real_database.resize(0);
	real_database.clear();
	this->data_position_identifyer.clear();
}

void Database::setup() {
	cerr << "# setting up the database" << endl;
}


void Database::dumpDataBase(OutputWriter *ow)
{
	cout<<"-------------------------------\n";
	cout<<"dumping a database, here ya go:"<<endl;
	for(unsigned d=0; d<real_database.size(); d++)
	{
		real_database.at(d)->printContent(cout);
		cout<<"\n";
	}
	cout<<"-------------------------------\n";
}

void Database::printDataBase(ostream &o)
{
	o<<"-------------------------------\n";
	o<<"dumping a database, here ya go:"<<endl;
	o<<"database has: "<<real_database.size()<<" elements."<<endl;
	for(unsigned d=0; d<real_database.size(); d++)
	{
		o<<"DATA]: "<<real_database.at(d)->getType()<<"\t\t"<<real_database.at(d)->getName()<<endl;
		//o<<real_database.at(d)->getName()<<endl;
		//real_database.at(d)->printContent(o);
		//o<<"\n";
	}
	o<<"-------------------------------\n";
}


	/** adds data to the database
	 * @param name Name of the variable to be added
	 * @param ptr Pointer to the data to be added
	 */
void Database::addData(string name, Data *data)
{
	this->real_database.push_back(data);
	this->data_position_identifyer[name]=real_database.size()-1;
}


void Database::addData(Data *data)
{
	this->real_database.push_back(data);
	this->data_position_identifyer[data->getName()]=real_database.size()-1;
}


bool Database::doesDataItemExist(string name) {
	if (data_position_identifyer.find(name)!= data_position_identifyer.end())
		return true;
	else
		return false;
}

	/** removes data item form the database
	 * @param name Name of data item to be removed.
	 */
void Database::eraseDataItem(string name)
{
	cerr <<"# can't delete data from the database... yet." << endl;
}

	/**
	 * returns a data item
	 * @param name identifyer of data item
	 * @return pointer to data item
	 */
Data* Database::getDataItem(string name)
{
	if(data_position_identifyer.find(name)!=data_position_identifyer.end()) {
		return real_database.at(data_position_identifyer.find(name)->second);
	}
	else {
		cerr << "Could not find the data you were looking for in the database" << endl;
		cerr << "You were looking for a Data named: "<<name<<endl;
		return 0;
	}
}

	/** set up a database by using an input reader
	 * @param ir Pointer to an InputReader
	 */
void Database::readDataBase(InputDataReader *ir)
{
	cerr << "creating the database from an inputDataReader." << endl;
}


void Database::returnPointerToData(string name, Data *&ptr)
{
	if(data_position_identifyer.find(name)!=data_position_identifyer.end()) {
		ptr = real_database.at(data_position_identifyer.find(name)->second);
	}
	else {
		cerr << "Could not find the data you were looking for in the database" << endl;
		cerr << "You were looking for a Data named: "<<name<<endl;
		ptr=0;
	}
}

bool Database::existsDataItem(string name)
{
	return data_position_identifyer.find(name)!=data_position_identifyer.end();
}

bool Database::existsDataType(string type)
{
	for(unsigned int k=0; k<real_database.size(); k++)
	{
		if(real_database.at(k)->getType().compare(type)==0)
			return true;
	}
	return false;
}

