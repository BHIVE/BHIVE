/*
 * worldModel.cpp
 *
 *  Created on: Oct 23, 2009
 *      Author: Len
 */

#include <iostream>
#include <string>
#include "simpleSpecies.hh"
#include "simpleRxn.hh"
#include "simpleGillespie.hh"
#include "worldModel.hh"

using namespace std;
using namespace Hive;

WorldModel::WorldModel(){

	cout << "Creating the World.\n";

	this->nSpecies=21;
	this->nRxns=54;

	// Rate constants *****
	double k_LProd = 1.0;
	double k_Move = 1.0;
	double k_Redirect = 1.0;
	double k_Mot = 1.0;
	double k_LCons = 1.0;

	this->species = new SimpleSpecies*[this->nSpecies];
	this->reaction = new SimpleRxn*[this->nRxns];

	// Make box species Bij
	// i=position{0,1,2}
	// j=ligand level {0,1,2} (low, medium, high)
	int madeSpecies=0;
	int **B = new int*[3];
	B[0] = new int[3];
	B[1] = new int[3];
	B[2] = new int[3];
	int boxPop;
	for (int i=0;i<3;i++){
		for (int j=0;j<3;j++){
			if (j==0){ boxPop=1; }
			else{ boxPop=0; }
			species[madeSpecies] = new SimpleSpecies(("B"+Util::toString(i)+Util::toString(j)), boxPop);
			B[i][j] = madeSpecies;
//			cout << "\tB[" << i << "][" << j << "] = " << madeSpecies << endl;
			madeSpecies++;
		}
	}

	// Make cell species Cijk
	// i=position {0,1,2}
	// j=direction {0,1} (left, right)
	// k=motor state {0,1} (run, tumble)
	int ***C = new int**[3];
	C[0] = new int*[2];
	C[1] = new int*[2];
	C[2] = new int*[2];
	C[0][0] = new int[2];
	C[0][1] = new int[2];
	C[1][0] = new int[2];
	C[1][1] = new int[2];
	C[2][0] = new int[2];
	C[2][1] = new int[2];
	int cellPop;
	for (int i=0;i<3;i++){
		for (int j=0;j<2;j++){
			for (int k=0;k<2;k++){
				if (i==0 && j==0 && k==0) { cellPop=1; }
				else { cellPop=0; }
				species[madeSpecies] = new SimpleSpecies(("C"+Util::toString(i)+Util::toString(j)+Util::toString(k)),
															cellPop);
//				cout << "\tpopulation " << species[madeSpecies]->population << endl;
				C[i][j][k] = madeSpecies;
//				cout << "\tC[" << i << "][" << j << "][" << k << "] = " << madeSpecies << endl;
				madeSpecies++;
			}
		}
	}

//	cout << "Made " << madeSpecies << " species.\n";

	// Make reactions
	int madeRxns=0;
	int stoich[4];

	// *** Ligand Production ***
	int nReactants=1;
	int nProducts=1;

	// Bi0 -> Bi1 (i=0,1,2)
	// Bi1 -> Bi2 (i=0,1,2)
	stoich[0]=-1;
	stoich[1]=1;

	SimpleSpecies ***Bi0 = new SimpleSpecies**[3];
	Bi0[0] = new SimpleSpecies*[1];
	Bi0[1] = new SimpleSpecies*[1];
	Bi0[2] = new SimpleSpecies*[1];
	SimpleSpecies ***Bi1 = new SimpleSpecies**[3];
	Bi1[0] = new SimpleSpecies*[1];
	Bi1[1] = new SimpleSpecies*[1];
	Bi1[2] = new SimpleSpecies*[1];
	SimpleSpecies ***Bi2 = new SimpleSpecies**[3];
	Bi2[0] = new SimpleSpecies*[1];
	Bi2[1] = new SimpleSpecies*[1];
	Bi2[2] = new SimpleSpecies*[1];

	for (int i=0;i<3;i++){
		Bi0[i][0] = species[B[i][0]];
		Bi1[i][0] = species[B[i][1]];
		Bi2[i][0] = species[B[i][2]];
		reaction[madeRxns] = new SimpleRxn(k_LProd,nReactants,Bi0[i],nProducts,Bi1[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
		reaction[madeRxns] = new SimpleRxn(k_LProd,nReactants,Bi1[i],nProducts,Bi2[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
	}

//	cout << endl;

	// *** Cell Movement ***
	nReactants=1;
	nProducts=1;

	// Ci00 -> C(i-1)00 (i=0,1,2)
	stoich[0]=-1;
	stoich[1]=1;

	SimpleSpecies ***Ci00 = new SimpleSpecies**[3];
	Ci00[0] = new SimpleSpecies*[1];
	Ci00[1] = new SimpleSpecies*[1];
	Ci00[2] = new SimpleSpecies*[1];
	SimpleSpecies ***pLeft = new SimpleSpecies**[3];
	pLeft[0] = new SimpleSpecies*[1];
	pLeft[1] = new SimpleSpecies*[1];
	pLeft[2] = new SimpleSpecies*[1];

	for (int i=0;i<3;i++){
		Ci00[i][0] = species[C[i][0][0]];
		if (i==0){
			pLeft[0][0] = species[C[2][0][0]];
		}
		else{
			pLeft[i][0] = species[C[i-1][0][0]];
		}
		reaction[madeRxns] = new SimpleRxn(k_Move,nReactants,Ci00[i],nProducts,pLeft[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
	}

	// Ci10 -> C(i+1)10 (i=0,1,2)
	SimpleSpecies ***Ci10 = new SimpleSpecies**[3];
	Ci10[0] = new SimpleSpecies*[1];
	Ci10[1] = new SimpleSpecies*[1];
	Ci10[2] = new SimpleSpecies*[1];
	SimpleSpecies ***pRight = new SimpleSpecies**[3];
	pRight[0] = new SimpleSpecies*[1];
	pRight[1] = new SimpleSpecies*[1];
	pRight[2] = new SimpleSpecies*[1];

	for (int i=0;i<3;i++){
		Ci10[i][0] = species[C[i][1][0]];
		if (i==2){
			pRight[2][0] = species[C[0][1][0]];
		}
		else{
			pRight[i][0] = species[C[i+1][1][0]];
		}
		reaction[madeRxns] = new SimpleRxn(k_Move,nReactants,Ci10[i],nProducts,pRight[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
	}

//	cout << endl;

	// *** Cell Redirection ***
	nReactants=1;
	nProducts=1;

	// Ci01 <-> Ci11 (i=0,1,2)
	stoich[0]=-1;
	stoich[1]=1;

	SimpleSpecies ***Ci01 = new SimpleSpecies**[3];
	Ci01[0] = new SimpleSpecies*[1];
	Ci01[1] = new SimpleSpecies*[1];
	Ci01[2] = new SimpleSpecies*[1];
	SimpleSpecies ***Ci11 = new SimpleSpecies**[3];
	Ci11[0] = new SimpleSpecies*[1];
	Ci11[1] = new SimpleSpecies*[1];
	Ci11[2] = new SimpleSpecies*[1];

	for (int i=0;i<3;i++){
		Ci01[i][0] = species[C[i][0][1]];
		Ci11[i][0] = species[C[i][1][1]];
		reaction[madeRxns] = new SimpleRxn(k_Redirect,nReactants,Ci01[i],nProducts,Ci11[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
		reaction[madeRxns] = new SimpleRxn(k_Redirect,nReactants,Ci11[i],nProducts,Ci01[i],stoich);
//		cout << "\t" + reaction[madeRxns]->name << endl;
		madeRxns++;
	}

//	cout << endl;

	// *** Cell Motor Flip ***
	nReactants=1;
	nProducts=1;

	// Cij0 <-> Cij1 (i=0,1,2; j=0,1)
	stoich[0]=-1;
	stoich[1]=1;

	SimpleSpecies ****Cij0 = new SimpleSpecies***[3];
	SimpleSpecies ****Cij1 = new SimpleSpecies***[3];

	for (int i=0;i<3;i++){
		Cij0[i] = new SimpleSpecies**[2];
		Cij1[i] = new SimpleSpecies**[2];
		for (int j=0;j<2;j++){
			Cij0[i][j] = new SimpleSpecies*[1];
			Cij1[i][j] = new SimpleSpecies*[1];
			Cij0[i][j][0] = species[C[i][j][0]];
			Cij1[i][j][0] = species[C[i][j][1]];
			reaction[madeRxns] = new SimpleRxn(k_Mot,nReactants,Cij0[i][j],nProducts,Cij1[i][j],stoich);
//			cout << "\t" + reaction[madeRxns]->name << endl;
			madeRxns++;
			reaction[madeRxns] = new SimpleRxn(k_Mot,nReactants,Cij1[i][j],nProducts,Cij0[i][j],stoich);
//			cout << "\t" + reaction[madeRxns]->name << endl;
			madeRxns++;
		}
	}

//	cout << endl;

	// *** Ligand Consumption ***
	nReactants=2;
	nProducts=2;

	// Cijk + Bi1 -> Cijk + Bi0 (i=0,1,2; j=0,1; k=0,1)
	// Cijk + Bi2 -> Cijk + Bi1 (i=0,1,2; j=0,1; k=0,1)
	stoich[0]=-1;
	stoich[1]=-1;
	stoich[2]=1;
	stoich[3]=1;

	SimpleSpecies *****CijkBi0 = new SimpleSpecies****[3];
	SimpleSpecies *****CijkBi1 = new SimpleSpecies****[3];
	SimpleSpecies *****CijkBi2 = new SimpleSpecies****[3];

	for (int i=0;i<3;i++){
		CijkBi0[i] = new SimpleSpecies***[2];
		CijkBi1[i] = new SimpleSpecies***[2];
		CijkBi2[i] = new SimpleSpecies***[2];
		for (int j=0;j<2;j++){
			CijkBi0[i][j] = new SimpleSpecies**[2];
			CijkBi1[i][j] = new SimpleSpecies**[2];
			CijkBi2[i][j] = new SimpleSpecies**[2];
			for (int k=0;k<2;k++){
				CijkBi0[i][j][k] = new SimpleSpecies*[2];
				CijkBi1[i][j][k] = new SimpleSpecies*[2];
				CijkBi2[i][j][k] = new SimpleSpecies*[2];

				CijkBi0[i][j][k][0] = species[C[i][j][k]];
				CijkBi0[i][j][k][1] = species[B[i][0]];

				CijkBi1[i][j][k][0] = species[C[i][j][k]];
				CijkBi1[i][j][k][1] = species[B[i][1]];

				CijkBi2[i][j][k][0] = species[C[i][j][k]];
				CijkBi2[i][j][k][1] = species[B[i][2]];

				reaction[madeRxns] = new SimpleRxn(k_LCons,nReactants,CijkBi1[i][j][k],nProducts,CijkBi0[i][j][k],stoich);
//				cout << "\t" + reaction[madeRxns]->name << endl;
				madeRxns++;
				reaction[madeRxns] = new SimpleRxn(k_LCons,nReactants,CijkBi2[i][j][k],nProducts,CijkBi1[i][j][k],stoich);
//				cout << "\t" + reaction[madeRxns]->name << endl;
				madeRxns++;
			}
		}
	}

//	cout << "Made " << madeRxns << " reactions.\n";

}







