/**
 * simpleCell.hh
 *
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef SIMPLECELL_HH_
#define SIMPLECELL_HH_

#define GRID "grid"
#define BACTERIADATABASE "bactCell"

#include <iostream>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <list>
#include <map>

#include "../../communicator/communicator.hh"
#include "../../hive.hh"
#include "../../agent/data/data.hh"
#include "../../util/util.hh"
#include "../../util/sparselib++/compcol_double.h"
#include "../../util/sparselib++/comprow_double.h"
#include "../../util/sparselib++/coord_double.h"
#include "../../util/sparselib++/ilupre_double.h"
#include "../../util/sparselib++/gmres.h"
#include "../../util/sparselib++/mvm.h"
#include "../../util/sparselib++/mvv.h"
#include "../../util/sparselib++/vecdefs.h"
#include "../../agent/agentFactory/agent_factory.hh"
#include "../../simulators/simulator.hh"
#include "composers/SC_composers.hh"
#include "composers/SC_parallelComposers.hh"
#include "../../parallel/parallel.hh"
#include "../../parallel/parallel.hh"
#include "universe/universe.hh"
#include "universe/universeFluxMessageClasses.h"
#include "cartesianGrid/worldFluxMessageClasses.h"
#include "cartesianGrid/worldCellTransferMessageClasses.h"
#include "cartesianGrid/cartesianGrid.hh"
#include "bactCell/bactCell.hh"
#include "../../agent/data/primitive/primitiveData.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {
	void runSimpleCell(int argc, char *argv[], int rank, int size);
}

#endif /* SIMPLECELL_HH_ */
