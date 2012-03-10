/*
 * chemotaxisnfsimactions.hh
 *
 *  Created on: Oct 22, 2009
 *      Author: jentsch
 */

#ifndef CHEMOTAXISNFSIMACTIONS_HH_
#define CHEMOTAXISNFSIMACTIONS_HH_

#include "../../agent/action/arnie.hh"
#include "../../agent/data/primitive/primitiveData.hh"

using namespace Hive;
using namespace std;

namespace Chemotaxis {

	class UpdateLigandConcentration : public Action {
		public:
			UpdateLigandConcentration();
			UpdateLigandConcentration(int id);

			~UpdateLigandConcentration() {};

			void fire (Data *d);
	};
}

#endif /* CHEMOTAXISNFSIMACTIONS_HH_ */
