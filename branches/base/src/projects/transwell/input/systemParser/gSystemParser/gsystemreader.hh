/*
 * gsystemreader.hh
 *
 *  Created on: Dec 15, 2009
 *      Author: jentsch
 */

#ifndef GSYSTEMREADER_HH_
#define GSYSTEMREADER_HH_


#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>
#include <sstream>

#include "../../system/gSystem/gsystem.hh"
#include "../../system/system.hh"
#include "../inputsystemreader.hh"

using namespace std;

namespace Hive {

	// parser which reads textfile containing a simple kinetic system
	class GSystemReader : public InputSystemReader {
	public:
		GSystemReader();
		~GSystemReader();

		System* parseSystem(ifstream& in);
	private:
		GSystem *gsys;
	};

}
#endif /* GSYSTEMREADER_HH_ */
