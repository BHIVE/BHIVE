/**
 * @file src/hive.cpp
 * @brief The main entry point for hive
 *
 * This file contains the main function and some other things.
 *
 * @author Michael Sneddon
 * @date Started: Oct 7, 2009    Last edited: Oct 23, 2009 by Michael / Chris / Ric and Garrit
 *
 * @todo Fill in the details. Memory cheching!!!
 *
 */

#include <iostream>
#include <exception>
#include <string>
#include <time.h>
#include <fstream>
#include <mpi.h>

#include <map>

#include "agent/agent.hh"
#include "exception/exception.hh"

#include "projects/dummyAgent/dummycomposer.hh"
#include "agent/agentFactory/nfAgentFactory/nfAgentFactory.hh"
#include "agent/agentFactory/chemotaxisCellFactory/chemotaxisCellFactory.hh"
#include "agent/messageGenerator/chemotaxisMessageGenerator/chemotaxisMessageGenerator.hh"

#include "agent/data/primitive/primitiveData.hh"

#include "agent/action/chemotaxisAction/chemotaxisAction.hh"

#include "simulators/nfsim/NFsim.hh"

#include "projects/chemotaxisAgent/chemotaxiscomposer.hh"
#include "projects/chemotaxisAgent/chemotaxisnfsimactions.hh"

#include "util/util.hh"

#include "util/sparselib++/compcol_double.h"
#include "util/sparselib++/comprow_double.h"
#include "util/sparselib++/coord_double.h"
#include "util/sparselib++/ilupre_double.h"
#include "util/sparselib++/gmres.h"
#include "util/sparselib++/mvm.h"
#include "util/sparselib++/mvv.h"


#include "projects/transwell/transwellcomposer.hh"

// simpleCell model for running capillary and multiple flagella simulations
#include "projects/simpleCell/simpleCell.hh"

#include "hive.hh"

using namespace std;
using namespace Hive;

int michael = 0;

/**
 *  Main method for hive.
 *

 *  @param argc number of arguments (this is the length of argv)
 *  @param argv[] pointer to char arrays (strings) that were passed as arguments
 *  @return zero if hive ran successfully, an error code if it did not
 */
int main(int argc, char *argv[])
{
	int rank, size;
	MPI::Status status;

	MPI::Init(argc,argv);
	rank = MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();
	cerr<<" hello mpi.  i am "<< rank << " of " <<size<<endl;



	bool michael = true;
	try {
		clock_t start,finish;
		string versionNumber = "0.00a2 - (unstable release)";
		cerr<<"# entering the hive [v"+versionNumber+"]\n";
		start=clock();

		if (michael) {
			SimpleCell::runSimpleCell(argc,argv);

		} else {

			// sets the seed of the random number generators. shouldn't this be a command line argument ?!
			// reply:  yes, although we can set this anywhere for any particular simulation for now.
			if (argc == 2) {
				int hans = atoi(argv[1]);
				Util::SEED_RANDOM(hans);
			} else
				Util::SEED_RANDOM(5);

			Composer *comp = new Transwell::TranswellComposer();
			// for the validation test I, we will have seven cells in the medium
			((Transwell::TranswellComposer *)comp)->setupNumberOfCells(1);
			// generate a simulation
			comp->setupSimulation();
			// get the maestro
			cerr << "# getting the maestro ... " << endl;
			Agent *maestro = comp->getTopLevelAgent();
			SpecialAgent *output_agent = comp->getSpecialAgent(0);
			output_agent->evaluateMessageQueue();
			// propagate the for a single timestep
			cerr << "# propagating for ... " << endl;
			for (int i=0; i<3600*60*0.2; i++) {
			//	cerr << "# STEP " << i << endl;
				maestro->propagate(5.0);
				output_agent->evaluateMessageQueue();
			}
			delete comp;

		}


		finish=clock();
		double time = (double(finish)-double(start))/CLOCKS_PER_SEC;
		cerr<<"# leaving the hive.  hive time:  "<<time<<" CPU seconds."<<endl;
		MPI::Finalize();



	} catch (HiveException& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.getFullMessage()<<endl;
		MPI::Finalize();
		exit(1);
	} catch (exception& e) {
		cout.flush();
		cerr<<"# ERROR! :  "<<e.what()<<endl;
		MPI::Finalize();
		exit(2);
	}


	// just for testing whether the integration of SparseLib++ works ...
	// double val[12];
//	vector<double> val(12);
//	val[0] = 1; val[1] = 4; val[2] = 11;
//	val[3] = 2; val[4] = 5; val[5] = 7;
//	val[6] = 6; val[7] = 8; val[8] = 10;
//	val[9] = 3; val[10] = 9; val[11] = 12;

//	int rp[6];
//	vector<int> rp(6);
//	rp[0] = 0;  rp[1] = 3;  rp[2] = 6; rp[3] = 9; rp[4] = 10; rp[5] = 12;

//	int ci[12];
//	vector<int> ci(12);
//	ci[0] = 0; ci[1] = 1; ci[2] = 4;
//	ci[3] = 0; ci[4] = 1; ci[5] = 2;
//	ci[6] = 1; ci[7] = 2; ci[8] = 4;
//	ci[9] = 3; ci[10] = 0; ci[11] = 4;

	// this works. however, it may be a little risky because i do not know whether the vector stores stuff in
	// the same way as if we just give pointers ...
//	CompRow_Mat_double hans(5,5,12, &val[0], &rp[0], &ci[0]);
//	CompRow_Mat_double hans(5,5,12, val, rp, ci);
	// calculate the solution Ax = b
//	CompRow_ILUPreconditioner_double pre(hans);

//	VECTOR_double x(hans.dim(1), 0.0);
//	VECTOR_double b(hans.dim(1), 1.0);
//	b[2] = 12;
//	MATRIX_double H(6,5,0.0);
//	int maxit = 100;
//	double tol = 1e-6;
//	int restart = 5;
//	int result = GMRES(hans, x, b, pre, H, restart, maxit, tol);

	// output results
//	cout << "GMRES flag = " << result << endl;
//	cout << "iterations " << maxit << endl;
//	cout << "tolerance " << tol << endl;
//	cout << x << endl;

	// testing the result
//	cout << hans*x << endl;

//	exit(1);


//	if (michael) {

//		try {

//			//Create a cell!!
//			Chemotaxis::ChemotaxisCellFactory *ccf = new Chemotaxis::ChemotaxisCellFactory();
//			Agent *cell0 = ccf->createAgent();
//
//			//Create an NFsim simulator!!
//			map <string,string> argMap;
//			//NF::parseArguments(argc, const_cast<const char**>(argv), argMap);
//
//			argMap.insert(pair<string,string>("xml","motor.xml"));
//			Hive::NF_AgentFactory *nfaf = new Hive::NF_AgentFactory(argMap);
//
//
//			Agent *nf0 = nfaf->createAgent();
//
//			//Add NF messageGenerator
//			Chemotaxis::NFsimNotifyMotorConfToCellMessageGenerator *nnmctcmg =
//						new Chemotaxis::NFsimNotifyMotorConfToCellMessageGenerator(nf0);
//			nf0->addMessageGenerator(nnmctcmg);
//
//			//Add UpdateLigandConcentrationAction
//			Chemotaxis::UpdateLigandConcentration * ulc = new Chemotaxis::UpdateLigandConcentration();
//			nf0->addAction(ulc);
//			ulc->setAgent(nf0);
//
//			//Set up connections and the communicator
//			cell0->addChild(nf0->getAgentId());
//			nf0->setParent(cell0->getAgentId());
//			SerialCommunicator *c = new SerialCommunicator();
//			c->addAgent(cell0);
//			c->addAgent(nf0);
//			cell0->addCommunicator(c);
//			nf0->addCommunicator(c);
//
//
//
//			//RUN!!!
//			for(double i=0; i<1; i+=0.01)
//				cell0->propagate(0.01);
//
//			Message *m2 = new Message();
//			m2->setAction(1);
//			DoubleVectorData *dvd2 = new DoubleVectorData("ChemicalDataMessage",2);
//			dvd2->setDouble(5000,0);
//			dvd2->setDouble(5000,1);
//			m2->setArgument(dvd2);
//			cell0->placeMessageOnMessageQueue(m2);
//
//
//			for(double i=0; i<1; i+=0.01)
//				cell0->propagate(0.01);
//
//
//			Chemotaxis::ChemotaxisCellFactory *ccf = new Chemotaxis::ChemotaxisCellFactory();
//
//			Agent *cell = ccf->createAgent();
//			for(double i=0; i<10; i+=0.01)
//				cell->propagate(0.01);
//
//			Message *m = new Message();
//			m->setAction(0);
//
//			DoubleVectorData *dvd = new DoubleVectorData("PositionDataMessage",3);
//			dvd->setDouble(5000,0);
//			dvd->setDouble(5000,1);
//			dvd->setDouble(5000,2);
//			m->setArgument(dvd);
//			cell->placeMessageOnMessageQueue(m);
//
//
//			Message *m2 = new Message();
//			m2->setAction(1);
//			DoubleVectorData *dvd2 = new DoubleVectorData("ChemicalDataMessage",1);
//			dvd2->setDouble(5000,0);
//			dvd2->setDouble(5000,1);
//			m2->setArgument(dvd2);
//			cell->placeMessageOnMessageQueue(m2);
//
//			Message *m3 = new Message();
//			m3->setAction(2);
//			IntVectorData *ivd = new IntVectorData("MotorDataMessage",1);
//			ivd->setInt(3,0); //CW motors
//			ivd->setInt(0,1); //CCW motors
//			m3->setArgument(ivd);
//			cell->placeMessageOnMessageQueue(m3);
//
//
//			for(double i=0; i<10; i+=0.01)
//				cell->propagate(0.01);
//
//			delete cell;
//
//
//			map <string,string> argMap;
//			NF::parseArguments(argc, const_cast<const char**>(argv), argMap);
//			Hive::NF_AgentFactory *nfaf = new Hive::NF_AgentFactory(argMap);
//
//			Agent *a = nfaf->createAgent();
//			Chemotaxis::NFsimNotifyMotorConfToCellMessageGenerator *nnmctcmg =
//					new Chemotaxis::NFsimNotifyMotorConfToCellMessageGenerator(a);
//			a->addMessageGenerator(nnmctcmg);
//
//			a->propagate(100);
//			delete a;
//			delete nfaf;

/*
 *      this is the old chemotaxis simulation class.
 *


		//nfmain(argc,argv); exit(1);
		Composer *comp = new Chemotaxis::ChemotaxisComposer();
		comp->setupSimulation();
		Agent *a = comp->getTopLevelAgent();
		ofstream bugout;
		bugout.open("bugout.txt");
		ofstream chemout("blup2.txt");
		exit(1);
		for (int i=0; i<0; i++) {
			cerr << "halloele   " << i << endl;
			a->propagate(0.0001);
			((Chemotaxis::ChemotaxisComposer *)comp)->generateBugOutput(bugout);
		}
		((Chemotaxis::ChemotaxisComposer *)comp)->generateChemicalOutput(chemout);
		bugout.flush();
		bugout.close();

		} catch (HiveException& e) {
			cout.flush();
			cerr<<"ERROR! :  "<<e.getFullMessage()<<endl;
			exit(1);
		} catch (exception& e) {
			cout.flush();
			cerr<<"ERROR! :  "<<e.what()<<endl;
			exit(2);
		}
*/
	return 0;
}




