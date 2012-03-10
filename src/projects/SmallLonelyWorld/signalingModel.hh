/*
 * signalingModel.hh
 *
 *  Created on: Oct 26, 2009
 *      Author: johnsekar
 */

#include "simpleSpecies.hh"
#include "simpleRxn.hh"

#ifndef SIGNALINGMODEL_HH_
#define SIGNALINGMODEL_HH_

using namespace std;

namespace Hive{

	class SignalingModel {

	public:
		int nSpecies;
		SimpleSpecies **species;
		int nRxns;
		SimpleRxn **reaction;

		SignalingModel();
		~SignalingModel(){};
	};
}


#endif /* SIGNALINGMODEL_HH_ */
