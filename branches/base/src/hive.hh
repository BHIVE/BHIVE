/**
 * @file src/hive.hh
 * @brief The main header file for hive
 *
 * This file contains the main function headers and declarations.
 *
 * @author Michael Sneddon
 *
 * @date Started: Oct 14, 2009    Last edited: Dec 22nd, 2010
 *
 * @todo Fill in the details.
 *
 */

#ifndef HIVE_HH_
#define HIVE_HH_

#include <stdlib.h>
#include <iostream>
#include <exception>
#include <string>
#include <time.h>
#include <fstream>
#include <map>
#include <string>

#include "core/agent/agent.hh"
#include "core/agent/special_agent.hh"
#include "core/agent/data/primitive/primitiveData.hh"
#include "core/exception/exception.hh"
#include "core/util/util.hh"
#include "core/util/sparselib++/compcol_double.h"
#include "core/util/sparselib++/comprow_double.h"
#include "core/util/sparselib++/coord_double.h"
#include "core/util/sparselib++/ilupre_double.h"
#include "core/util/sparselib++/gmres.h"
#include "core/util/sparselib++/mvm.h"
#include "core/util/sparselib++/mvv.h"
#include "core/registrar/registrar.hh"

/// THIS SHOULD BE MOVED INTO THE CORE.
#include "projects/project.hh"

//!  Core Hive classes and functionality.
/*!
 *  All of the Hive framework classes and methods are defined in this namespace,
 *  so you should use this namespace in all of your projects.  Be sure to define
 *  a new namespace for problem specific projects.
 *  @author Michael Sneddon
 */
namespace Hive {}


#endif /* HIVE_HH_ */
