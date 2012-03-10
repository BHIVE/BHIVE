/**
 *@file dummymessagegenerator.hh
 *
 *this file contains the interface to the dummymessagegenerator
 *
 *@author garrit jentsch
 *
 *@date  Oct 15, 2009, last edited 10-15-2009
 *
 */

#ifndef DUMMYMESSAGEGENERATOR_HH_
#define DUMMYMESSAGEGENERATOR_HH_

#include "../../agent/messageGenerator/messagegenerator.hh"
#include "../../agent/data/primitive/primitiveData.hh"

using namespace std;
using namespace Hive;

namespace Dummy
{

	/**
	 * @brief this class implements the messagegenerator
	 *
	 * @todo
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 15, 2009 last edited 10-15-2009
	 */


	class DummyMessageGenerator : public  MessageGenerator {
	public:
		//DummyMessageGenerator();
		DummyMessageGenerator(Agent*, bool isParentMessage);
		~DummyMessageGenerator();

		void placeMessage(int destID);
	};
}


#endif // DUMMYMESSAGEGENERATOR_HH_
