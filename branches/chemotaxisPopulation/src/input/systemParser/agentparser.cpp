#include "agentparser.hh"

using namespace Hive;

AgentParser::AgentParser() {
	this->input = NULL;
	this->ag = NULL;
	this->input_has_been_assigned = false;
}

AgentParser::~AgentParser() { }

void AgentParser::parse(Agent* ag, ifstream& in) {
	this->setAgent(ag);
	this->setInputfile(in);
	this->doIt();
}

void AgentParser::parse(Agent* ag, string filename) {
	this->setAgent(ag);
	this->setInputfile(filename);
}

void AgentParser::parse(Agent* ag) {
	this->setAgent(ag);
	if (!this->input_has_been_assigned)
		throw HiveException("no input has been assigned to AgentParser", "AgenParser(Agent *ag");
	else
		this->doIt();
}

void AgentParser::parse() {
	if (this->ag != NULL &&
		!this->input_has_been_assigned)
		throw HiveException("neither agent nor input have been assigend to AgentParser", "AgentParser()");
	else
		this->doIt();
}

void AgentParser::setAgent(Agent* a) {
	this->ag = a;
}

void AgentParser::setInputfile(ifstream& in) {
	this->input_has_been_assigned = true;
	this->input = &in;
}

void AgentParser::setInputfile(string name) {
	input->open(name.c_str());
}

void AgentParser::doIt() {
	throw HiveException("trying to call AgentParser::doIt() for base class. not doing anything.",
			"AgentParser::doIt()");
}
