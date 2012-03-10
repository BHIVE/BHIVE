/*
 * SC_messageGenerators.hh
 *
 *  Created on: Feb 15, 2010
 *      Author: msneddon
 */

#ifndef SC_MESSAGEGENERATORS_HH_
#define SC_MESSAGEGENERATORS_HH_



#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell
{

		class WorldNotifyChemicalConcentrationToCellMessageGenerator : public MessageGenerator {
			public:
				WorldNotifyChemicalConcentrationToCellMessageGenerator(Agent *ag);

				~WorldNotifyChemicalConcentrationToCellMessageGenerator();

				void placeMessage(int destID);

			private:
				Database *db;
		};
}

#endif /* SC_MESSAGEGENERATORS_HH_ */
