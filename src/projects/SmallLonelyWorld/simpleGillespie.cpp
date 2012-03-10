/*
 * simpleGillespie.cpp
 *
 *  Created on: Oct 20, 2009
 *      Author: Leonard Harris
 */

#include <math.h>
#include <float.h>
#include "simpleSpecies.hh"
#include "simpleRxn.hh"
#include "simpleGillespie.hh"

using namespace std;
using namespace Hive;

// Constructor
SimpleGillespie::SimpleGillespie(int nSpecies, SimpleSpecies **species, int nRxns,
		SimpleRxn **reaction, bool debug) : OurSimulator(){
	cout << "simpleGillespie constructor called.\n";
//	this->agent=a;
//	this->db=this->agent->getDatabase();
	this->nSpecies=nSpecies;
	this->species=species;
	this->nRxns=nRxns;
	this->reaction=reaction;
	this->debug=debug;
}

/// Destructor
SimpleGillespie::~SimpleGillespie(){
	cout << "simpleGillespie destructor called.\n";
}

void SimpleGillespie::step(double t){
	this->run(t);
}

void SimpleGillespie::run(double endTime){
	this->run(0.0,endTime);
}

void SimpleGillespie::run(double startTime, double endTime){
	double t = startTime;
	while (t < endTime){
		t += this->nextStep(endTime-t);
	}
}

void SimpleGillespie::run(int nSteps){
	int step=1;
	double t=0.0;
	while (step <= nSteps){
		step++;
		t += this->nextStep();
		if (debug==true){
			cout << "Step " << step << endl;
			cout << "Time: " << t << endl;
			cout << endl;
		}
	}
}

double SimpleGillespie::nextStep(){
	return this->nextStep(DBL_MAX);
}

double SimpleGillespie::nextStep(double maxTau){

	double tau;
	int firingRxn;

	// Calculate tau
	double sumRates = 0.0;
	for (int i=0;i<this->nRxns;i++){
		sumRates += reaction[i]->rate;
	}
	double r1 = Util::RANDOM_CLOSED();
	tau = -log(r1)/sumRates;

	if (tau <= maxTau){

		// Identify firing reaction
		double r2 = Util::RANDOM_CLOSED();
//		cout << "r2*sumRates = " << (r2*sumRates) << endl;
		double partialSum = reaction[0]->rate;
		firingRxn=0;
		while (partialSum < (r2*sumRates)) {
//			cout << "partialSum = " << partialSum << endl;
			firingRxn++;
			partialSum += reaction[firingRxn]->rate;
		}
//		cout << "partialSum = " << partialSum << endl;
		if (debug==true){
			cout << "Our firing reaction is " << firingRxn << ": " + reaction[firingRxn]->name
			<< endl;
		}

		// Update populations
		for (int i=0;i<reaction[firingRxn]->nReactants;i++){
			reaction[firingRxn]->reactants[i]->changePopulation(reaction[firingRxn]->
					stoichiometry[i]);
			if (debug==true){
				cout << "Change species " + reaction[firingRxn]->reactants[i]->name + " by " <<
				reaction[firingRxn]->stoichiometry[i] << endl;
				cout << "Population of " + reaction[firingRxn]->reactants[i]->name + " is now " <<
				reaction[firingRxn]->reactants[i]->population << endl;
			}
		}
		for (int i=0;i<reaction[firingRxn]->nProducts;i++){
			reaction[firingRxn]->products[i]->changePopulation(reaction[firingRxn]->
					stoichiometry[i+reaction[firingRxn]->nReactants]);
			if (debug==true){
				cout << "Change species " + reaction[firingRxn]->products[i]->name + " by " <<
				reaction[firingRxn]->stoichiometry[i+reaction[firingRxn]->nReactants] << endl;
				cout << "Population of " + reaction[firingRxn]->products[i]->name + " is now " <<
				reaction[firingRxn]->products[i]->population << endl;
			}
		}

		// Update rates for all reactions (unoptimized)
		for (int i=0;i<nRxns;i++){
			reaction[i]->calculateRate();
			if (debug==true){
				cout << "Rate for reaction " + reaction[i]->name + " is now " << reaction[i]->rate
				<< endl;
			}
		}
	}
	else{ tau = maxTau; }

	return tau;
}

void SimpleGillespie::setAgent(Agent *a){
	this->agent = a;
	this->db = this->agent->getDatabase();
}

void SimpleGillespie::prepare(){
	cout << "Simple Gillespie simulator of agent " << this->agent->getAgentId()
	<< " is preparing." << endl;
}

void SimpleGillespie::synchroniseWithDatabase(){
	cout << "Simple Gillespie simulator of agent: " << this->agent->getAgentId()
	<< " is synching with database." << endl;
}

void SimpleGillespie::initialise(){}
