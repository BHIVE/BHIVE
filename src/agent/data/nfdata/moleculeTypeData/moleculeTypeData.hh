/*
 * moleculeTypeData.hh
 *
 *  Created on: Oct 20, 2009
 *      Author: msneddon
 */

#ifndef MOLECULETYPEDATA_HH_
#define MOLECULETYPEDATA_HH_


#include "../../data.hh"
#include "moleculeType.hh"

using namespace Hive;

namespace NF
{

	class MoleculeType;

	/**
	* @brief A data object to store NFsim MoleculeType objects
	* @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	* @author Michael Sneddon
	*/
	class NF_MoleculeTypeData : public Data {
		public:
			/// default-constructor
			NF_MoleculeTypeData(MoleculeType *mt, string moleculeTypeName);

			~NF_MoleculeTypeData();

			/// virtual method for print the content of a data item on sterr.
			void printContent(ostream &out);


			MoleculeType * getMoleculeType() const;
			string getMoleculeTypeName() const;

		private:
			MoleculeType *mt;
	};

}






#endif /* MOLECULETYPEDATA_HH_ */
