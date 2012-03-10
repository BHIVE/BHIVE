/*
 * SC_actions.hh
 *
 *  Created on: Feb 15, 2010
 *      Author: msneddon
 */

#ifndef SC_ACTIONS_HH_
#define SC_ACTIONS_HH_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {



	class UpdateLigandConcentration : public Action {
		public:
			UpdateLigandConcentration();
			UpdateLigandConcentration(int id);

			~UpdateLigandConcentration() {};

			void fire (Data *d);
	};





}







#endif /* SC_ACTIONS_HH_ */
