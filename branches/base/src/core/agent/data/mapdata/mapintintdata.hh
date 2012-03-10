/*
 * mapintintdata.hh
 *
 *  Created on: Jun 18, 2010
 *      Author: jentsch
 */

#ifndef MAPINTINTDATA_HH_
#define MAPINTINTDATA_HH_

#include <map>
#include "../data.hh"
#include <iostream>

using namespace std;


namespace Hive {

	class MapIntIntData : public Data {
		public:
			MapIntIntData(string dataName);
			~MapIntIntData();

			void insert(int key, int value);

			void erase(int key);

			void set(int key, int value);

			int size();

			void getPtrToMap(map<int,int> *ref);

			// will return real index in vectors for agent with agentId id
			int returnValueForKey(int key);

			void printContent(ostream& out);

		protected:
			map <int,int> m;

	};


}



#endif /* MAPINTINTDATA_HH_ */
