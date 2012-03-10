/*
 * worldModel.hh
 *
 *  Created on: Oct 23, 2009
 *      Author: Len
 */

#ifndef WORLDMODEL_HH_
#define WORLDMODEL_HH_

using namespace std;

namespace Hive{

	class WorldModel {

	public:
		int nSpecies;
		SimpleSpecies **species;
		int nRxns;
		SimpleRxn **reaction;

		WorldModel();
		~WorldModel(){};
	};
}
#endif /* WORLDMODEL_HH_ */
