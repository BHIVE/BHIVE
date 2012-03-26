/**
 * simpleCell.cpp
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */


#include "simpleCell.hh"
#include "../../propagator_class.hh"



using namespace std;
using namespace Hive;
using namespace SimpleCell;

namespace SimpleCell {
	void runSimpleCell(int argc, char *argv[], int rank, int size){
		PropagatorClass* pp = new PropagatorClass();
		Composer* scpc;
        size = 2;
        
		if (size == 1) {
			SimpleCellDefaultComposer* temp = new SimpleCellDefaultComposer();
			temp->compose(string(argv[2]), string(argv[3]));
			temp->addCommunicator();
			scpc = (Composer*)temp;
		} else {                // works with old params format
			SimpleCellDefaultComposer* temp = new SimpleCellDefaultComposer();
			temp->parallelCompose(string(argv[2]), string(argv[3]));
			temp->addCommunicator();
			scpc = (Composer*)temp;
		}
		pp->setup(scpc);
		pp->go(0.1, 100);
		delete pp;
		delete scpc;
	}
}
