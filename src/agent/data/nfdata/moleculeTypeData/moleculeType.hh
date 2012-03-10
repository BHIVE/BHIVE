/*
 * moleculeTypeData.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef MOLECULETYPE_HH_
#define MOLECULETYPE_HH_


#include "../../../../simulators/nfsim/NFcore/NFcore.hh"


#include <string>
#include <vector>
#include <iostream>

using namespace std;


namespace NF
{

	class System;
	class Molecule;
	class MoleculesObservable;
	class ReactionClass;
	class LocalFunction;
	class TemplateMolecule;
	class MoleculeList;






	//!  Keeps track of the types of molecules that can exist.
	/*!
      This class maintains information about a "type" of molecule in the System.
	  It keeps track of all reactions that this "type" of molecule can possibly
	  be a part of and a list of the default binding sites and states.  It also
	  keeps track and updates all Observables that pertain to this MoleculeType.
	  And perhaps most importantly, it keeps track of all Molecules that exist in
	  the simulation of this molecule "type."  It has functions which allow it
	  also to populate itself with a number of default molecules, making initializations
	  of the simulation easier.
	    @author Michael Sneddon
	 */
	class MoleculeType
	{
		public:

			MoleculeType(
					string name,
					vector <string> &compName,
					System *s);

			MoleculeType(
					string name,
					vector <string> &compName,
					vector <string> &defaultCompState,
					System *s);

			MoleculeType(
					string name,
					vector <string> &compName,
					vector <string> &defaultCompState,
					vector < vector<string> > &possibleCompStates,
					System *system);

			MoleculeType(
					string name,
					vector <string> &compName,
					vector <string> &defaultCompState,
					vector < vector<string> > &possibleCompStates,
					vector <bool> isIntegerComponent,
					System *system);


			~MoleculeType();


			/////////////  DATA METHODS  ////////////////////////////

			void printContent(ostream& out) {cout<<"printing moleculeType"<<endl; };






			string getName() const { return name; };
			int getTypeID() const { return type_id; };
			System * getSystem() const;

			//Function to access component information
			int getNumOfComponents() const { return numOfComponents; };
			string getComponentName(int cIndex) const;
			void getPossibleComponentStates(int cIndex, list <string> &nameList);
			int getDefaultComponentState(int cIndex) const { return defaultCompState[cIndex]; };

			int getCompIndexFromName(string cName) const;
			string getComponentStateName(int cIndex, int cValue);
			int getStateValueFromName(int cIndex, string stateName) const;


			//set of functions that deal with equivalent (aka symmetric) components
			int getNumOfEquivalencyClasses() const { return this->n_eqComp; };
			string *getEquivalencyClassCompNames() const { return this->eqCompOriginalName; };
			void addEquivalentComponents(vector <vector <string> > &identicalComponents);
			bool isEquivalentComponent(string cName) const;
			bool isEquivalentComponent(int cIndex) const;
			void getEquivalencyClass(int *&components, int &n_components, string cName) const;
			int getEquivalencyClassNumber(string cName) const;


			bool isIntegerComponent(string cName) const;
			bool isIntegerComponent(int cIndex) const;

			//functions that handle the observables
			int getNumOfMolObs() const { return (int)molObs.size(); };
			string getMolObsName(int obsIndex) const;
			MoleculesObservable * getMolObs(int obsIndex) const { return molObs.at(obsIndex); };
			int getMolObsCount(int obsIndex) const;
			void removeFromObservables(Molecule * m);
			void addToObservables(Molecule * m);
			void outputMolObsNames(ofstream &fout);
			void outputMolObsCounts(ofstream &fout);
			void printMolObsNames();
			void printMolObsCounts();



			void addAllToObservables();


			//function to access particular molecules or reactions (these are really only
			//used when debugging or running the walker...
			Molecule * getMolecule(int ID_molecule) const;
			int getMoleculeCount() const;

			int getReactionCount() const { return reactions.size(); };
			int getRxnIndex(ReactionClass * rxn, int rxnPosition);



			//Functions to generate molecules, remove molecules at the beginning
			//or during a running simulation
			Molecule *genDefaultMolecule();

			void addMoleculeToRunningSystem(Molecule *&mol);
			void removeMoleculeFromRunningSystem(Molecule *&m);
			void removeFromRxns(Molecule * m);



			//Adds the basic components that this MoleculeType needs to reference
			void addReactionClass(ReactionClass * r, int rPosition);
			void addMolObs(MoleculesObservable * mo) { molObs.push_back(mo); }; //could add check here to make sure observable is of this type
			int createComplex(Molecule *m);
			void addTemplateMolecule(TemplateMolecule *t);

			/* handle DOR reactions */
			//void addDORrxnClass(ReactionClass * r, int rPosition);
			//int getDORrxnCount() const { return indexOfDORrxns.size(); };
			//ReactionClass * getDORrxn(int index) const { return reactions.at(indexOfDORrxns.at(index)); };
			//int getDORreactantPosition(int index) const { return reactionPositions.at(indexOfDORrxns.at(index)); };
			//int getDORreactantIndex(int index) const { return indexOfDORrxns.at(index); };


			/* updates a molecules membership (assumes molecule is of type this) */
			void updateRxnMembership(Molecule * m);

			/* auto populate with default molecules */
			void populateWithDefaultMolecules(int moleculeCount);

			/* this method assumes all molecules in the simulation
			 * have been defined, and all reaction classes and observables
			 * have been added.  Then, this function will add those
			 * molecules to rxn lists and instantiate the counts of the observables.
			 * In general, you do not need to worry about this function because
			 * it automatically gets called by the System when you prepare the System*/
			void prepareForSimulation();


			//Debugging function that prints some useful information about this MoleculeType
			void printDetails() const;
			void printAllMolecules();


			//Functionality for local functions

			/* Type I local functions are functions that this molecule type needs to have
			 * always updated because it can react in a DOR reaction which needs the specified
			 * function.  DOR reactions identify these MoleculeTypes and add their function to
			 * the list of Type I functions for this MoleculeType.
			 *
			 * Type II local functions are those that require this MoleculeType as an observable
			 * or counter to be evaluated.  When this molecule gets updated, it has to automatically
			 * update all of its type II local functions.
			 *
			 * These functions return the index in the array indicating where these functions were
			 * added.  This allows local functions to quickly update molecules that need the local
			 * function information.
			 */
			int addLocalFunc_TypeI(LocalFunction *lf);
			int addLocalFunc_TypeII(LocalFunction *lf);
			vector <LocalFunction *> locFuncs_typeI;
			vector <LocalFunction *> locFuncs_typeII;

			int getNumOfTypeIFunctions() const {return locFuncs_typeI.size(); };
			LocalFunction *getTypeILocalFunction(int index) { return locFuncs_typeI.at(index); };
			int getNumOfTypeIIFunctions() const {return locFuncs_typeII.size(); };
			LocalFunction *getTypeIILocalFunction(int index) { return locFuncs_typeII.at(index); };

			int getNumOfDORrxns() const { return indexOfDORrxns.size(); };
			ReactionClass * getDORrxn(int dorRxnIndex) const { return reactions.at(indexOfDORrxns.at(dorRxnIndex)); };
			int getDORrxnIndex(int dorRxnIndex) const { return indexOfDORrxns.at(dorRxnIndex); };
			int getDORrxnPosition(int dorRxnIndex) const { return reactionPositions.at(indexOfDORrxns.at(dorRxnIndex)); };

			void setUpLocalFunctionListForMolecules();

		protected:

			void init(
				string name,
				vector <string> &compName,
				vector <string> &defaultCompState,
				vector < vector<string> > &possibleCompStates,
				vector <bool> isIntegerComponent,
				System *system);


			//basic info
			System *system;
			string name;
			int type_id;

			//keeps track of the key information about a MoleculeType - the component
			int numOfComponents;
			string *compName;
			vector < vector < string > > possibleCompStates;
			int *defaultCompState;
			bool *isIntegerCompState;


			//set of variables to keep track of equivalent (aka symmetric) components
			int n_eqComp;
			string *eqCompOriginalName;
			int * eqCompSizes;
			string **eqCompName;
			int **eqCompIndex;


			//Lists and vectors of everything we need to know
			MoleculeList * mList;

			vector <ReactionClass *> reactions; /* List of reactions that this type can be involved with */
			vector <int> reactionPositions;   /* the position in the reaction for this type of molecule */

			vector <int> indexOfDORrxns;


			vector <MoleculesObservable *> molObs;  /* list of things to keep track of */

			vector <TemplateMolecule *> allTemplates; /* keep track of all templates that exist of this type
															so that they are easy to delete from memory at the end */

			ReactionClass *rxn; /*used so we don't need to redeclare this at every call to updateRxnMembership */



		private:
			//Some iterators so we don't have to instantiate a new iterator every time
			vector<Molecule *>::iterator molIter;  /* to iterate over mInstances */
			vector<MoleculesObservable *>::iterator molObsIter; /* to iterate over observables */
			vector <ReactionClass *>::iterator rxnIter; /* to iterate over reactions */
	};

}




#endif /* MOLECULETYPEDATA_HH_ */
