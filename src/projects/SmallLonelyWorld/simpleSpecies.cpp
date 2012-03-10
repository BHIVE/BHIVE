/*
 * simpleSpecies.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: Leonard Harris
 */

#include <iostream>
#include <string>
#include "simpleSpecies.hh"

using namespace std;
using namespace Hive;

SimpleSpecies::~SimpleSpecies(){
	cout << "SimpleSpecies destructor called: " << this->name << endl;
}

SimpleSpecies::SimpleSpecies(string name, double population){
	cout << "SimpleSpecies constructor called: " << name << endl;
	this->name=name;
	this->population=population;
}

void SimpleSpecies::setName(string name){
	this->name=name;
}

void SimpleSpecies::setPopulation(double newPopulation){
	this->population=newPopulation;
}

void SimpleSpecies::changePopulation(double deltaPopulation){
	this->population += deltaPopulation;
}
