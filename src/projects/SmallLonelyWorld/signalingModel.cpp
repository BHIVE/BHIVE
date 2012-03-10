/*
 * signalingModel.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: johnsekar
 */

#include <iostream>
#include <string>
#include "simpleSpecies.hh"
#include "simpleRxn.hh"
#include "simpleGillespie.hh"
#include "signalingModel.hh"

using namespace std;
using namespace Hive;

SignalingModel::SignalingModel(){

	cout << "Creating the Cell.\n";

	this->nSpecies = 7;
	this->nRxns = 8;

	// Parameters *****
	double k_ligBind = 1.0;
	double k_ligUnbind = 1.0;
	double k_RPhos = 1.0;
	double k_YPhos = 1.0;
	double k_YDephos = 1.0;
	double L_0 = 1e3;
	double Y0_0 = 1e3;

	this->species = new SimpleSpecies*[this->nSpecies];
	this->reaction = new SimpleRxn*[this->nRxns];

	int stoich[4] = {0,0,0,0};
	//creating the species
	// 0 - L, 1 - R00, 2 - R01, 3 - R10, 4 - R11, 5 - Y0, 6 - Y1
	// R - first suffix is ligand binding
	// R - second suffix is phosphorylation state
	species[0] = new SimpleSpecies("L",L_0);	//Ligand
	species[1] = new SimpleSpecies("R00",1.0);	//Receptor - unbound - unphos
	species[2] = new SimpleSpecies("R01",0.0);	//Receptor - unbound - phos
	species[3] = new SimpleSpecies("R10",0.0);	//Receptor - bound - unphos
	species[4] = new SimpleSpecies("R11",0.0);  //Receptor - bound - phos
	species[5] = new SimpleSpecies("Y0",Y0_0);	//Messenger- unphos
	species[6] = new SimpleSpecies("Y1",0.0);	//Messenger- phos (motility signal)

	// R00 + L <-> R10: 1,0<->3 ; Ligand binding/unbinding to/from unphos Receptor
	SimpleSpecies **R00L = new SimpleSpecies*[2];
	R00L[0] = species[1];
	R00L[1] = species[0];
	SimpleSpecies **R10 = new SimpleSpecies*[1];
	R10[0] = species[3];
	stoich[0]= -1; stoich[1]= -1; stoich[2]= 1;
	reaction[0] = new SimpleRxn(k_ligBind,2,R00L,1,R10,stoich);
	stoich[0]= -1; stoich[1]= 1; stoich[2]= 1;
	reaction[1] = new SimpleRxn(k_ligUnbind,1,R10,2,R00L,stoich);

	// R01 + L <-> R11: 2,0->4 ; Ligand binding/unbinding to/from phos Receptor
	SimpleSpecies **R01L = new SimpleSpecies*[2];
	R01L[0] = species[2];
	R01L[1] = species[0];
	SimpleSpecies **R11 = new SimpleSpecies*[1];
	R11[0] = species[4];
	stoich[0]= -1; stoich[1]= -1; stoich[2]= 1;
	reaction[2] = new SimpleRxn(k_ligBind,2,R01L,1,R11,stoich);
	stoich[0]= -1; stoich[1]= 1; stoich[2]= 1;
	reaction[3] = new SimpleRxn(k_ligUnbind,1,R11,2,R01L,stoich);

	// R10 -> R11: 3->4 ; Bound receptor phosphorylation
	stoich[0]= -1; stoich[1]= 1;
	reaction[4] = new SimpleRxn(k_RPhos,1,R10,1,R11,stoich);

	// R01 + Y0 -> R00 + Y1: 2,5->1,6 ; Unbound phos receptor exchanging phos with messenger
	SimpleSpecies **R01Y0 = new SimpleSpecies*[2];
	R01Y0[0] = species[2];
	R01Y0[1] = species[5];
	SimpleSpecies **R00Y1 = new SimpleSpecies*[2];
	R00Y1[0] = species[1];
	R00Y1[1] = species[6];
	stoich[0]= -1; stoich[1]= -1; stoich[2]= 1; stoich[3]= 1;
	reaction[5] = new SimpleRxn(k_YPhos,2,R01Y0,2,R00Y1,stoich);

	// R11 + Y0 -> R10 +Y1: 4,5->3,6 ; Bound phos receptor exchanging phos with messenger
	SimpleSpecies **R11Y0 = new SimpleSpecies*[2];
	R11Y0[0] = species[4];
	R11Y0[1] = species[5];
	SimpleSpecies **R10Y1 = new SimpleSpecies*[2];
	R10Y1[0] = species[3];
	R10Y1[1] = species[6];
	stoich[0]= -1; stoich[1]= -1; stoich[2]= 1; stoich[3]= 1;
	reaction[6] = new SimpleRxn(k_YPhos,2,R11Y0,2,R10Y1,stoich);

	// Y1 -> Y0: 6->5 ; Messenger dephosphorylation
	SimpleSpecies **Y1 = new SimpleSpecies*[1];
	Y1[0] = species[6];
	SimpleSpecies **Y0 = new SimpleSpecies*[1];
	Y0[0] = species[5];
	stoich[0]= -1; stoich[1]= 1;
	reaction[7] = new SimpleRxn(k_YDephos,1,Y1,1,Y0,stoich);
}
