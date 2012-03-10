/*
 * test.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: Len
 */

#include <iostream>
#include <string>
#include <time.h>
#include <math.h>

#include "../../agent/agent.hh"

#include "../../projects/dummyAgent/dummycomposer.hh"

#include "simpleSpecies.hh"
#include "simpleRxn.hh"
#include "simpleGillespie.hh"
#include "tester.hh"
#include "../../agent/agent.hh"

#include "../../util/util.hh"

#include "tester.hh"

using namespace std;
using namespace Hive;

Tester::Tester(){

	// *** Test simpleSpecies ***

	SimpleSpecies **firstSpecies = new SimpleSpecies*[1];
	firstSpecies[0] = new SimpleSpecies("A",1000.0);
	cout << firstSpecies[0]->name + " " << firstSpecies[0]->population << endl;

	SimpleSpecies **nextSpecies = new SimpleSpecies*[2];
	nextSpecies[0] = new SimpleSpecies("B",100.0);
	nextSpecies[1] = new SimpleSpecies("C",10.0);
	for (int i=0;i<2;i++){
		cout << nextSpecies[i]->name + " " << nextSpecies[i]->population << endl;
	}

	SimpleSpecies **nullSpecies = new SimpleSpecies*[1];
	nullSpecies[0] = new SimpleSpecies("null",0.0);

	// *** Test simpleRxn ***
	// A -> B
	int stoich[3] = {-1, 1};
	SimpleSpecies **speciesB = new SimpleSpecies*[1];
	speciesB[0] = nextSpecies[0];
	SimpleRxn firstRxn(1.0,1,firstSpecies,1,speciesB,stoich);
	cout << firstRxn.name << endl;
	cout << "type = " << firstRxn.type << endl;
	cout << "c = " << firstRxn.c << endl;
	for (int i=0;i<firstRxn.nReactants;i++){
		cout << "X[" << i << "] = " << firstRxn.reactants[i]->population << endl;
 	}
	cout << "Rate = " << firstRxn.rate << endl << endl;

	////
	int nRxn = 5;
	SimpleRxn **nextRxn = new SimpleRxn*[nRxn];
	////

	// A -> null
	stoich[0] = -1;
	nextRxn[0] = new SimpleRxn(2.0,1,firstSpecies,0,nullSpecies,stoich);

	// null -> A
	stoich[0] = 1;
	nextRxn[1] = new SimpleRxn(300.0,0,nullSpecies,1,firstSpecies,stoich);

	// 2A -> B
	stoich[0] = -2;
	stoich[1] = 1;
	nextRxn[2] = new SimpleRxn(0.001,1,firstSpecies,1,speciesB,stoich);

	// B -> 2A
	stoich[0] = -1;
	stoich[1] = 2;
	nextRxn[3] = new SimpleRxn(3.0,1,speciesB,1,firstSpecies,stoich);

	// C + B -> A
	stoich[0] = -1;
	stoich[1] = -1;
	stoich[2] = 3;
	nextRxn[4] = new SimpleRxn(2.0,2,nextSpecies,1,firstSpecies,stoich);

	for (int i=0;i<nRxn;i++){
		cout << endl << nextRxn[i]->name << endl;
		cout << "type = " << nextRxn[i]->type << endl;
		cout << "c = " << nextRxn[i]->c << endl;
		for (int j=0;j<nextRxn[i]->nReactants;j++){
			cout << "X[" << j << "] = " << nextRxn[i]->reactants[j]->population << endl;
	 	}
		cout << "Rate = " << nextRxn[i]->rate << endl;
	}

	// Try out some random numbers
	long seed = 12L;
	Util::SEED_RANDOM( seed );
	double r = Util::RANDOM_CLOSED();
	cout << "\nrandom number: " << r << endl;

	// Test out the simulator
	SimpleSpecies **allSpecies = new SimpleSpecies*[3];
	allSpecies[0] = firstSpecies[0];
	allSpecies[1] = nextSpecies[0];
	allSpecies[2] = nextSpecies[1];
	cout << endl;
	for (int i=0;i<3;i++){
		cout << allSpecies[i]->name + ": " << allSpecies[i]->population << endl;
	}
//	Agent *dummyAgent = new Agent();
	SimpleGillespie firstSim(3,allSpecies,5,nextRxn,false);
	cout << "Our first tau is " << firstSim.nextStep() << endl;

	// Our first simulation
	SimpleGillespie nextSim(3,allSpecies,5,nextRxn,false);
	int nSteps=5;
	cout << "\n***Our first simulation of " << nSteps << " steps.***\n\n";
	nextSim.run(nSteps);

	// Clean up
	cout << endl;
	delete firstSpecies[0];
	delete [] firstSpecies;
	delete nextSpecies[0];
	delete nextSpecies[1];
	delete [] nextSpecies;
	delete nullSpecies[0];
	delete [] nullSpecies;
	delete [] speciesB;
	delete [] allSpecies;

	for (int i=0;i<nRxn;i++){
		delete nextRxn[i];
	}
	delete [] nextRxn;
	cout << endl;

//	delete dummyAgent;
}
