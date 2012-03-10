/**
 * simpleCell.hh
 *  Created on: Jan 29, 2010
 *      Author: msneddon
 */

#ifndef SIMPLECELL_HH_
#define SIMPLECELL_HH_

//Include for the hive, which is need for all BHIVE projects
#include "../../hive.hh"

//Definitions utilized in components specific to simple cell project
#define GRID "grid"
#define BACTERIADATABASE "bactCell"

//Includes for utilities used by simple cell project objects
#include "../../util/util.hh"
#include "../../util/sparselib++/compcol_double.h"
#include "../../util/sparselib++/comprow_double.h"
#include "../../util/sparselib++/coord_double.h"
#include "../../util/sparselib++/ilupre_double.h"
#include "../../util/sparselib++/gmres.h"
#include "../../util/sparselib++/mvm.h"
#include "../../util/sparselib++/mvv.h"
#include "../../util/sparselib++/vecdefs.h"

//Includes for all simple cell specific objects 
#include "composers/SC_composers.hh"
#include "composers/SC_parallelComposers.hh"
#include "universe/universe.hh"
#include "universe/universeFluxMessageClasses.h"
#include "cartesianGrid/worldFluxMessageClasses.h"
#include "cartesianGrid/worldCellTransferMessageClasses.h"
#include "cartesianGrid/cartesianGrid.hh"
#include "bactCell/bactCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {
	void runSimpleCell(int argc, char *argv[], int rank, int size);
}

#endif /* SIMPLECELL_HH_ */
