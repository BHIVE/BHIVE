/*
 * parameterData.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef PARAMETERDATA_HH_
#define PARAMETERDATA_HH_


#include "../../data.hh"

using namespace Hive;

namespace NF
{
	/**
	* @brief A data object to store NFsim MoleculeType objects
	* @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	* @author Michael Sneddon
	*/
	class NF_ParamaterData : public Data {
		public:
			/// default-constructor
			NF_ParamaterData(string parameterName, double value);

			~NF_ParamaterData();

			/// virtual method for print the content of a data item on sterr.
			void printContent(ostream &out);


			double getValue() const;
			void setValue(double value);

		private:
			double value;
	};

}




#endif /* PARAMETERDATA_HH_ */
