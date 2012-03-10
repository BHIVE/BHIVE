/*
 * SimpleRxn.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: Leonard Harris
 */

#include <iostream>
#include <string.h>
#include "simpleRxn.hh"
#include "simpleSpecies.hh"
#include "../../util/util.hh"

using namespace std;
using namespace Hive;

	SimpleRxn::~SimpleRxn(){
		cout << "SimpleRxn destructor called: " << this->name << endl;
	}

	SimpleRxn::SimpleRxn(double c, int nReactants, SimpleSpecies **reactants, int nProducts,
								SimpleSpecies **products, int *stoichiometry){

		cout << "SimpleRxn constructor called.\n";

		this->c=c;

		this->nReactants=nReactants;
		this->nProducts=nProducts;

		if (this->nReactants==0){
			this->reactants=new SimpleSpecies*[1];
			this->reactants[0]=reactants[0];
		}
		else{
			this->reactants=new SimpleSpecies*[this->nReactants];
		}
		this->stoichiometry=new int[this->nReactants + this->nProducts];
		for (int i=0;i<this->nReactants;i++){
			this->reactants[i]=reactants[i];
			this->stoichiometry[i]=stoichiometry[i];
		}

		if (this->nProducts==0){
			this->products=new SimpleSpecies*[1];
			this->products[0]=products[0];
		}
		else{
			this->products=new SimpleSpecies*[this->nProducts];
		}
		for (int i=0;i<this->nProducts;i++){
			this->products[i]=products[i];
			this->stoichiometry[i+this->nReactants]=stoichiometry[i+this->nReactants];
		}

		// Determine type:
		// type 0: * -> products
		// type 1: S -> products
		// type 2: 2S -> products
		// type 3: S1 + S2 -> products

		if (this->nReactants == 0){ // * -> products
			this->type=0;
		}
		else if (this->nReactants == 1){
			if (this->stoichiometry[0] == -1){ // S -> products
				this->type=1;
			}
			else if (this->stoichiometry[0] == -2){ // 2S -> products
				this->type=2;
			}
			else{
				cout << "Error: Reaction type not recognized.\n";
				exit(1);
			}
		}
		else if (this->nReactants == 2){
			if (this->stoichiometry[0] == -1 && this->stoichiometry[1] == -1){ // S1 + S2 -> products
				this->type=3;
			}
			else{
				cout << "Error: Reaction type not recognized.\n";
				exit(1);
			}
		}
		else{
			cout << "Error: Reaction type not recognized.\n";
			exit(1);
		}

		// Get reaction string
		if (this->nReactants > 0){
			if (this->stoichiometry[0] < -1) {
				this->name += Util::toString(-this->stoichiometry[0]);
			}
			this->name += this->reactants[0]->name;
			for (int i=1;i<this->nReactants;i++){
				this->name += " + ";
				if (this->stoichiometry[i] < -1){
					this->name += Util::toString(-this->stoichiometry[i]);
				}
				this->name += this->reactants[i]->name;
			}
		}
		else{
			this->name = "*";
		}
		//
		this->name += " -> ";
		//
		if (this->nProducts > 0){
			if (this->stoichiometry[this->nReactants] > 1){
				this->name += Util::toString(this->stoichiometry[this->nReactants]);
			}
			this->name += this->products[0]->name;
			for (int i=1;i<this->nProducts;i++){
				this->name += " + ";
				if (this->stoichiometry[this->nReactants+i] > 1){
					this->name += Util::toString(this->stoichiometry[this->nReactants+i]);
				}
				this->name += this->products[i]->name;
			}
		}
		else{
			this->name += "*";
		}

		// Get rate
		this->calculateRate();
	}

	// Calculate propensity
	void SimpleRxn::calculateRate(){
		this->rate=calculateRate(c,type,reactants);
	}

	// Calculate propensity
	double SimpleRxn::calculateRate(double c, int type, SimpleSpecies **reactants){
		if (type==0){ // * -> products
			return c;
		}
		else if (type==1){ // S -> products
			return (c*reactants[0]->population);
		}
		else if (type==2){ // 2S -> products
			return (c*0.5*reactants[0]->population*(reactants[0]->population-1.0));
		}
		else if (type==3){ // S1 + S2 -> products
			return (c*reactants[0]->population*reactants[1]->population);
		}
		else{
			cout << "Error: Reaction type not recognized.\n";
			exit(1);
		}
	}
