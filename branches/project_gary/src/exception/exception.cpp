/**
 * @file src/exception/exception.cpp
 * @brief Implements the default Hive exception object
 *
 * The object HiveException is implemented here.
 *
 * @author Michael Sneddon
 * @date Started: Oct 19, 2009    Last edited: Oct 19, 2009
 *
 *
 */


#include "exception.hh"
#include <string>

using namespace std;
using namespace Hive;


HiveException::HiveException(string message, string errorLocation)
{
	this->message=message;
	this->trace="\t"+errorLocation;

}


HiveException::~HiveException()
{

}

void HiveException::addTraceException(string traceLocation)
{
	this->trace = "\t" + traceLocation + "\n" + this->trace;
}

string HiveException::what()
{
	return message;
}

string HiveException::getFullMessage() const
{
	return message+"\nin:"+trace+"\n";
}
