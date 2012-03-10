/*
 * nf.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef NF_HH_
#define NF_HH_


#include "../simulator.hh"
#include "NFcore/NFcore.hh"
#include "../../util/util.hh"


using namespace Hive;

namespace NF
{

	class NFsim : public Simulator
	{
		public:
			NFsim(Agent *a, System *s);
			~NFsim();

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();
			void setAgent(Agent *ag) {};

		protected:

			void initialise();


		private:


			System *system;
			///////////////////////////////////////////////////////////////////////////
			// Invariant simulator properties, created when the simulator is created and before
			// the simulator is prepared
			//string name;         /*!< arbitrary name of the system  */
			//bool useComplex;     /*!< sets whether or not to dynamically track complexes */
			//bool useBinaryOutput; /*!< set to true to turn on binary output of data */
			//int universalTraversalLimit; /*!< sets depth to traverse molecules when updating reactant lists */
			//bool onTheFlyObservables;    /*!< sets whether or not observables are calculated on the fly */
		    //bool outputGlobalFunctionValues; /*< set to true to output the value of all global functions at each output step */
		    //int globalMoleculeLimit; /*< total number of any particular molecule that can be created, default=100,000 */

//			vector <MoleculeType *> allMoleculeTypes;  /*!< container of all MoleculeTypes in the simulation */
//			vector <ReactionClass *> allReactions;    /*!< container of all Reactions in the simulation */
//			vector <Complex * > allComplexes;         /*!< container of all complexes in the simulation */
//			queue <int> nextAvailableComplex;         /*!< queue tells us which complexes can be used next */
//
//			vector <Observable *> obsToOutput; /*!< keeps ordered list of pointers to observables for output */
//			vector <Observable *> speciesObservables;


	};

}










#endif /* NF_HH_ */
