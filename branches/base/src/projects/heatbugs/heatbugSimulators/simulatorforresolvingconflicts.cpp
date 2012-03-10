#include "simulatorforresolvingconflicts.hh"

using namespace Heatbug;

SimulatorForResolvingConflicts::SimulatorForResolvingConflicts() {
	this->dt                    = 0;
	this->bug_grid              = NULL;
	this->bug_positions         = NULL;
	this->desired_bug_positions = NULL;
	this->desired_bug_grid      = NULL;
	this->tmp_bug_grid          = NULL;
	this->tmp_bug_positions     = NULL;
//	this->test();

}

SimulatorForResolvingConflicts::~SimulatorForResolvingConflicts() { }

// checks whether bugs that wish to move to position x,y have already been moved or not
bool SimulatorForResolvingConflicts::checkMoveFlags(int x, int y) {
	int num  = 0;
	int agId = 0;

	// iterate over all particles that wish to move to x,y
	for (int i=0; i<desired_bug_grid->at(x)->at(y)->size(); i++) {
		agId = desired_bug_grid->at(x)->at(y)->at(i).getInteger();
		num += tmp_bug_moved->at(agId-1);
	}

	// check whether all have agents have the same move-flags
	if (num > 0 && num < desired_bug_grid->at(x)->at(y)->size()) {
		throw HiveException("something is rotten in the SimulatorForResolvingConflicts",
				"bool SimulatorForResolvingConflicts::checkMoveFlags(int x, inty)");
	}
	return num;
}

// sets bug back to his original position
// note that the given parameter is not the agent Id but the
// index at which the bug is stored in the positions vectors.
void SimulatorForResolvingConflicts::getBack(int bug_index) {
	// get the old bug_position
	int x = bug_positions->at(bug_index)->at(0).getInteger();
	int y = bug_positions->at(bug_index)->at(1).getInteger();

	// set the temporary bug position
	tmp_bug_grid->at(x)->at(y)              = bug_index+1;
	tmp_bug_positions->at(bug_index)->at(0) = x;
	tmp_bug_positions->at(bug_index)->at(1) = y;
	tmp_bug_moved->at(bug_index)            = true;

	// tell the other bugs trying to occupy the position x,y to move back to their original place
	int help;
	for (int i=0; i<desired_bug_grid->at(x)->at(y)->size(); i++) {
		help = desired_bug_grid->at(x)->at(y)->at(i).getInteger();
		if (!tmp_bug_moved->at(help-1))
			getBack(help-1);
	}
}

void SimulatorForResolvingConflicts::step(double t) {
	// local variables
	int chosen_ag_id;
	int index;
	int x;
	int y;

	// -----------------------------------
	// FIRST STEP FOR RESOLVING CONFLICTS
	// -----------------------------------
	// if a bug decides to stay on his place, this has the highest priority and all
	// other bugs that want to move to that place have to go back to their old places.
	for (int i=0; i<this->num_bugs; i++) {
		if (bug_positions->at(i)->at(0).getInteger() == desired_bug_positions->at(i)->at(0).getInteger() &&
			bug_positions->at(i)->at(1).getInteger() == desired_bug_positions->at(i)->at(1).getInteger())
			// only put it back if it has not been moved yet
			if (!tmp_bug_moved->at(i))
				this->getBack(i);
	}

	// ------------------------------------
	// SECOND STEP FOR RESOLVING CONFLICTS
	// ------------------------------------
	// if serveral bugs attempt to go to the same position we randomly choose one. the other agents
	// are sent back to their original positions.
	// iterate over the desired grid
	for (int i=0; i<gridsize; i++) {
		for (int j=0; j<gridsize; j++) {
			// if there are several agents attempting to move to the same position, check whether they
			// have not been moved yet
			if (this->desired_bug_grid->at(i)->at(j)->size() > 1 &&
				!this->checkMoveFlags(i,j)) {
				// randomly choose one of the agents
				index        = (int)(desired_bug_grid->at(i)->at(j)->size() * Util::RANDOM_CLOSED());
				chosen_ag_id = desired_bug_grid->at(i)->at(j)->at(index).getInteger();
				// move the chosen agents
				tmp_bug_grid->at(i)->at(j)                   = chosen_ag_id;
				tmp_bug_positions->at(chosen_ag_id-1)->at(0) = i;
				tmp_bug_positions->at(chosen_ag_id-1)->at(1) = j;
				tmp_bug_moved->at(chosen_ag_id-1) = true;
				// tell the remaining particles to get back to their old position
				for (int k=0; k<desired_bug_grid->at(i)->at(j)->size(); k++)
					if (desired_bug_grid->at(i)->at(j)->at(k).getInteger() != chosen_ag_id)
						this->getBack(desired_bug_grid->at(i)->at(j)->at(k).getInteger()-1);
			}
		}
	}

	// ------------------------------------
	// THIRD STEP FOR RESOLVING CONFLICTS
	// ------------------------------------
	// the bugs that have not been moved can move to the position they would like to move to.
	for (int i=0; i<num_bugs; i++) {
		if (!tmp_bug_moved->at(i)) {
			x = desired_bug_positions->at(i)->at(0).getInteger();
			y = desired_bug_positions->at(i)->at(1).getInteger();
			tmp_bug_positions->at(i)->at(0) = x;
			tmp_bug_positions->at(i)->at(1) = y;
			tmp_bug_grid->at(x)->at(y)      = i+1;
			tmp_bug_moved->at(i)            = true;
		}
	}

	// ------------------------------------
	// FOURTH STEP FOR RESOLVING CONFLICTS
	// ------------------------------------
	// copy results on to the correct variables and clear temporary data structures for the
	// next round. also make sure that the desired_data structures are reset such that they can be filled
	// up again by the message generators.
	for (int i=0; i<gridsize; i++) {
		for (int j=0; j<gridsize; j++) {
			// update the bug_grid
			bug_grid->at(i)->at(j).setInteger(tmp_bug_grid->at(i)->at(j));
			// reset size of desired_bug_grid
			desired_bug_grid->at(i)->at(j)->reserveSize(0);
			if (desired_bug_grid->at(i)->at(j)->size() != 0)
				cerr << "how can that be" << endl;
			// set entry in tmp_bug_grid to zero
			tmp_bug_grid->at(i)->at(j) = 0;
		}
	}

	for (int i=0; i<num_bugs; i++) {
		// copy the bug_positions
		bug_positions->at(i)->at(0).setInteger(tmp_bug_positions->at(i)->at(0));
		bug_positions->at(i)->at(1).setInteger(tmp_bug_positions->at(i)->at(1));
		// set the flag back
		tmp_bug_moved->at(i) = false;
		// we do not have to do anything for the desired_bug_positions
		// and the tmp_bug_positions
	}
}


void SimulatorForResolvingConflicts::prepare() {
	Database *db = agent->getDatabase();

	bug_grid              = (TVectorData<TVectorData<IntegerData>* > *) db->getDataItem("bug_grid");
	bug_positions         = (TVectorData<TVectorData<IntegerData>* > *) db->getDataItem("bug_positions");
	desired_bug_positions = (TVectorData<TVectorData<IntegerData>* > *) db->getDataItem("desired_bug_positions");
	desired_bug_grid      =	(TVectorData<TVectorData<TVectorData<IntegerData>* >* >*) db->getDataItem("desired_bug_grid");

	// reserve memory for the tmp_bug_grid and tmp_pug_positions
	this->gridsize = ((IntegerData *) db->getDataItem("grid_size"))->getInteger();
	this->num_bugs = ((IntegerData *) db->getDataItem("number_bugs"))->getInteger();

	tmp_bug_grid = new TVectorData<TVectorData<int>* > ("noname", "notype");
	tmp_bug_grid->reserveSize(gridsize);
	for (int i=0; i<gridsize; i++) {
		tmp_bug_grid->at(i) = new TVectorData<int> ("noname", "notype");
		tmp_bug_grid->at(i)->reserveSize(gridsize);
	}

	tmp_bug_positions = new TVectorData<TVectorData<int>* > ("noname", "notype");
	tmp_bug_positions->reserveSize(num_bugs);
	for (int i=0; i<num_bugs; i++) {
		tmp_bug_positions->at(i) = new TVectorData<int> ("noname", "notype");
		tmp_bug_positions->at(i)->reserveSize(2);
	}

	tmp_bug_moved = new TVectorData<int> ("noname", "notype");
	tmp_bug_moved->reserveSize(num_bugs);
	for (int i=0; i<num_bugs; i++)
		tmp_bug_moved->at(i) = false;
}

void SimulatorForResolvingConflicts::setAgent(Agent *ag) {
	this->agent = ag;
	this->prepare();
}

void SimulatorForResolvingConflicts::setTimeStep(double t) {
	this->dt = t;
}

// nothing to be done
void SimulatorForResolvingConflicts::synchroniseWithDatabase() { }

// method for testing the simulator
void SimulatorForResolvingConflicts::test() {
	cerr << "# entering testing method of SimulatorForResolvingConflicts" << endl;

	int grid_size = 5;
	int num_bugs  = 4;

	this->num_bugs = num_bugs;
	this->gridsize = grid_size;

	// reserve memory for the grids
	bug_grid = new TVectorData<TVectorData<IntegerData>* > ("nn","nt");
	bug_grid->reserveSize(grid_size);
	tmp_bug_grid = new TVectorData<TVectorData<int>* > ("nn", "nt");
	tmp_bug_grid->reserveSize(grid_size);
	desired_bug_grid = new TVectorData<TVectorData<TVectorData<IntegerData>* >* > ("nn", "nt");
	desired_bug_grid->reserveSize(grid_size);
	for (int i=0; i<grid_size; i++) {
		bug_grid->at(i) = new TVectorData<IntegerData> ("nt", "nn");
		bug_grid->at(i)->reserveSize(grid_size);
		tmp_bug_grid->at(i) = new TVectorData<int> ("nt", "nn");
		tmp_bug_grid->at(i)->reserveSize(grid_size);
		desired_bug_grid->at(i) = new TVectorData<TVectorData<IntegerData>* >("nn", "nt");
		desired_bug_grid->at(i)->reserveSize(grid_size);
		for (int j=0; j<grid_size; j++) {
			bug_grid->at(i)->at(j).setInteger(0);
			tmp_bug_grid->at(i)->at(j) = 0;
			desired_bug_grid->at(i)->at(j) = new TVectorData<IntegerData> ("mm","mm");
		}
	}

	// reserve memory for bug positions
	bug_positions = new TVectorData<TVectorData<IntegerData>* >("NN","nt");
	bug_positions->reserveSize(num_bugs);
	tmp_bug_positions = new TVectorData<TVectorData<int>* > ("NN","nt");
	tmp_bug_positions->reserveSize(num_bugs);
	desired_bug_positions = new TVectorData<TVectorData<IntegerData>* > ("NN","nt");
	desired_bug_positions->reserveSize(num_bugs);
	for (int i=0; i<num_bugs; i++) {
		bug_positions->at(i) =  new TVectorData<IntegerData> ("","");
		bug_positions->at(i)->reserveSize(2);
		tmp_bug_positions->at(i) =  new TVectorData<int> ("","");
		tmp_bug_positions->at(i)->reserveSize(2);
		desired_bug_positions->at(i) =  new TVectorData<IntegerData> ("","");
		desired_bug_positions->at(i)->reserveSize(2);
	}

	// reserve memory for bug moved
	tmp_bug_moved = new TVectorData<int> ("","");
	tmp_bug_moved->reserveSize(num_bugs);
	for (int i=0; i<num_bugs; i++)
		tmp_bug_moved->at(i) = false;

	// set up configuration
	// bug 1
	bug_grid->at(1)->at(1).setInteger(1);
	bug_positions->at(0)->at(0).setInteger(1);
	bug_positions->at(0)->at(1).setInteger(1);


	// bug 2
	bug_grid->at(2)->at(2).setInteger(2);
	bug_positions->at(1)->at(0).setInteger(2);
	bug_positions->at(1)->at(1).setInteger(2);


	// bug 3
	bug_grid->at(2)->at(3).setInteger(3);
	bug_positions->at(2)->at(0).setInteger(2);
	bug_positions->at(2)->at(1).setInteger(3);


	// bug 4
	bug_grid->at(0)->at(2).setInteger(4);
	bug_positions->at(3)->at(0).setInteger(0);
	bug_positions->at(3)->at(1).setInteger(2);


	for (int i=0; i<num_bugs; i++)
		cerr << "bug " << i+1 << " sits at position " << bug_positions->at(i)->at(0).getInteger()
		<< " " << bug_positions->at(i)->at(1).getInteger() << endl;

	// bug 1
	desired_bug_grid->at(1)->at(2)->addElementToEnd(IntegerData("",1));
	desired_bug_positions->at(0)->at(0).setInteger(1);
	desired_bug_positions->at(0)->at(1).setInteger(2);


	// bug 2
	desired_bug_grid->at(1)->at(2)->addElementToEnd(IntegerData("",2));
	desired_bug_positions->at(1)->at(0).setInteger(1);
	desired_bug_positions->at(1)->at(1).setInteger(2);

	// bug 3
	desired_bug_grid->at(2)->at(2)->addElementToEnd(IntegerData("",3));
	desired_bug_positions->at(2)->at(0).setInteger(2);
	desired_bug_positions->at(2)->at(1).setInteger(2);

	// bug 4
	desired_bug_grid->at(1)->at(2)->addElementToEnd(IntegerData("",4));
	desired_bug_positions->at(3)->at(0).setInteger(1);
	desired_bug_positions->at(3)->at(1).setInteger(2);

	cerr << "moving ... ";
	this->step(1);
	cerr << " ... done" << endl;
	for (int i=0; i<num_bugs; i++)
		cerr << "bug " << i+1 << " sits at position " << bug_positions->at(i)->at(0).getInteger()
		<< " " << bug_positions->at(i)->at(1).getInteger() << endl;


	exit(1);
}

// nothing to be done
void SimulatorForResolvingConflicts::initialise() { }
