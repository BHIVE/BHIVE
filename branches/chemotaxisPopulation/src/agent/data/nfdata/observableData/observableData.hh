/*
 * observableData.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef OBSERVABLEDATA_HH_
#define OBSERVABLEDATA_HH_




#include "../../data.hh"
#include "observable.hh"

using namespace Hive;

namespace NF
{

	class Observable;

	/**
	* @brief A data object to store NFsim MoleculeType objects
	* @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	* @author Michael Sneddon
	*/
	class NF_ObservableData : public Data {
		public:
			/// default-constructor
			NF_ObservableData(Observable *ob, string observableName);

			~NF_ObservableData();

			/// virtual method for print the content of a data item on sterr.
			void printContent(ostream &out);


			Observable * getObservable() const;
			string getObservableName() const;
			int getObservableValue() const;

		private:
			Observable *obs;
	};

}

#endif /* OBSERVABLEDATA_HH_ */
