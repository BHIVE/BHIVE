/**
 * simpleCell.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */


#include "simpleCell.hh"
#include "../../parallel_propagator.hh"



using namespace std;
using namespace Hive;
using namespace SimpleCell;

namespace SimpleCell {

	void runBiasTester(SimpleCellDefaultComposer * scdc);
	void runForager(SimpleCellDefaultComposer * scdc, double eqTime);
    void runStepper(SimpleCellDefaultComposer * scdc);


	void runSimpleCell(int argc, char *argv[], int rank, int size)
	{
		if (size == 1)
			runSimpleCellSerial(argc, argv);
		else if (size > 1)
			runSimpleCellParallel(argc, argv, rank, size);
	}

	void runSimpleCellParallel(int argc, char *argv[], int rank, int size) {
		double eqTime = 300;
		SimpleCellDefaultParallelComposer *scpc = new SimpleCellDefaultParallelComposer(argv[2],argv[3],eqTime);

		scpc->setupSimulation();

		ParallelPropagator *pp = new ParallelPropagator((Composer *)scpc);

		pp->go(0.1,0.1);

		delete pp;
		delete scpc;

	}


	void runSimpleCellSerial(int argc, char *argv[]) {

		cout<<"Running SimpleCell.\n";
		cout<<"Parameters: \n";
		for (int i=0; i<argc; i++)
			cout<<"  ["<<i<<"]"<<argv[i]<<"\n";
		cout<<endl;

		//Create the system
		double eqTime = 300;
		SimpleCellDefaultComposer * scdc = new SimpleCellDefaultComposer(argv[2],argv[3],eqTime);
		scdc->addSerialCommunicator();
		scdc->setupSimulation();

		//Grab the environment
//		Agent *environment = scdc->getTopLevelAgent();



//		//Simulate 1000 second traces...
//		double dt=0.01; int steps = (int)(10/dt);
//		for(int i=0; i<steps; i++) {
//			environment->propagate(dt);
//			scdc->outputNow(i*dt);
//		}
//
//		//Simulate 1000 seconds, output only every 10 seconds, for exploration plots
//		double dt=0.01; int steps = (int)(1000/dt);
//		int outputStep = (int)(10/dt); int nextOutputTime = outputStep;
//		for(int i=0; i<steps; i++) {
//			environment->propagate(dt);
//			cout<<((double)i)*dt<<endl;
//			if(i>=nextOutputTime) {
//				scdc->outputNow(i*dt);
//				nextOutputTime+=outputStep;
//			}
//		}
//		scdc->outputNow(steps*dt);

//		//Simulate 600 seconds, output only at the end
//		double dt=0.01; int steps = (int)(600/dt);
//		for(int i=0; i<steps; i++) {
//			environment->propagate(dt);
//			cout<<((double)i)*dt<<endl;
//		}
//		scdc->outputNow(steps*dt);


		//runBiasTester(scdc);
		//runForager(scdc,eqTime);
		runStepper(scdc);

	//	cerr << "done" << endl;

		delete scdc;
	}



	void runBiasTester(SimpleCellDefaultComposer * scdc)
	{
		Agent * environment = scdc->getTopLevelAgent();

		//Simulate 1000 seconds, output only every 10 seconds, for exploration plots
		double dt=0.01; int steps = (int)(5000/dt);
		scdc->outputNow(0);
		for(int i=0; i<steps; i++) {
			environment->propagate(dt);
			cout<<((double)i)*dt<<endl;
			scdc->outputNow(i*dt);
		}
		scdc->outputNow(steps*dt);

	}


	void runForager(SimpleCellDefaultComposer * scdc, double eqTime)
	{
		Agent * environment = scdc->getTopLevelAgent();


		//First, simulate for the eqTime, output
		double dt=0.01; int steps = (int)(eqTime/dt);
		scdc->outputNow(-eqTime);
		for(int i=0; i<steps; i++) {
			environment->propagate(dt);
			cout<<"eq: "<<((double)i)*dt<<endl;
		}

		//Simulate 1000 seconds, output only every 10 seconds, for exploration plots
		steps = (int)(600/dt);
		int outputStep = (int)(5/dt); int nextOutputTime = outputStep;
		scdc->outputNow(0);
		for(int i=0; i<steps; i++) {
			environment->propagate(dt);
			cout<<((double)i)*dt<<endl;
			if(i>=nextOutputTime) {
				scdc->outputNow(i*dt);
				nextOutputTime+=outputStep;
			}
		}
		scdc->outputNow(steps*dt);

	}







    void runStepper(SimpleCellDefaultComposer * scdc)
    {
    	double baseTime = 0;
    	Agent * environment = scdc->getTopLevelAgent();

    	//Step to plot a dose response to increasing steps of attractant
    	double dt=0.01; int steps = (int)(300/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}

 		scdc->updateAllChemicalConc(0.01);
    	steps = (int)(600/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}

    	scdc->updateAllChemicalConc(0.1);
    	steps = (int)(600/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}

    	scdc->updateAllChemicalConc(1.0);
    	steps = (int)(600/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}

    	scdc->updateAllChemicalConc(10.0);
    	steps = (int)(600/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}

    	scdc->updateAllChemicalConc(100.0);
    	steps = (int)(600/dt);
    	for(int i=0; i<steps; i++) {
    		environment->propagate(dt);baseTime+=dt;
    		scdc->outputNow(baseTime);
    	}
//
//    	scdc->updateAllChemicalConc(1000.0);
//    	steps = (int)(600/dt);
//    	for(int i=0; i<steps; i++) {
//    		environment->propagate(dt);baseTime+=dt;
//    		scdc->outputNow(baseTime);
//    	}
//
//    	scdc->updateAllChemicalConc(10000.0);
//    	steps = (int)(600/dt);
//    	for(int i=0; i<steps; i++) {
//    		environment->propagate(dt);baseTime+=dt;
//    		scdc->outputNow(baseTime);
//    	}


    }









	const int SimpleCellConstants::CONSTANT_ENVIRONMENT = 0;
	const int SimpleCellConstants::LINEAR_ENVIRONMENT = 1;
	const int SimpleCellConstants::EXPONENTIAL_ENVIRONMENT = 2;
	const int SimpleCellConstants::CAPILLARY_ENVIRONMENT = 3;


	const int SimpleCellConstants::NOISE_SIGNALING = 20;
	const int SimpleCellConstants::SIMPLE_WINGREEN_SIGNALING = 21;

	const int SimpleCellConstants::SINGLE_MOTOR = 31;
	const int SimpleCellConstants::VOTING_MOTORS = 32;
	const int SimpleCellConstants::CONFORMATION_MOTORS = 33;

	const int SimpleCellConstants::STANDARD_SWIMMING = 41;
	const int SimpleCellConstants::TURNER_TUMBLE_ANGLE_SWIMMING = 42;



	const int SimpleCellConstants::CCW=0;
	const int SimpleCellConstants::CW=1;

	const int SimpleCellConstants::RUN=0;
	const int SimpleCellConstants::TUMBLE=1;


	const int SimpleCellConstants::NORMAL = 3;
	const int SimpleCellConstants::SEMI = 4;
	const int SimpleCellConstants::CURLY = 5;
	const int SimpleCellConstants::NONE = 6;


}
