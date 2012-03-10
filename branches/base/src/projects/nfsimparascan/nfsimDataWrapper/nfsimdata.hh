/*
 * nfsimdata.hh
 *
 * this is a simple wrapper that wraps a NFsim System into a data-item.
 * with the help of this class we will be able to easily create agents that
 * use NFsim.
 *
 * Disadvantage: data item s of this type can not be send across the mpi interface.
 * consequently the methods packDataForMPI and unpackDataForMPI will throw exceptions.
 *
 *  Created on: Mar 16, 2011
 *      Author: jentsch
 */

#ifndef NFSIMDATA_HH_
#define NFSIMDATA_HH_

#include "../../../core/agent/data/data.hh"
#include "../../../core/exception/exception.hh"

#include "../nfsimSimulator/NFsim/NFcore/NFcore.hh"
#include "../nfsimSimulator/NFsim/NFinput/NFinput.hh"
#include "../nfsimSimulator/NFsim/NFreactions/NFreactions.hh"


using namespace Hive;

namespace nfsimparascan {

	class NFsimData : public Data {
	public:
		NFsimData(string name);
		~NFsimData();

		void printContent(ostream& out);

		void packDataForMPI(char*& buffer, int& size);
		void unpackDataForMPI(char* buffer, int size);

		System* getNFsimSystem();

		void setNFsimSystem(System *s);

		void copyContentsInto(Data *destination);

	protected:
		/// pointer to the NFsim system.
		System *sys;
	};

}

#endif /* NFSIMDATA_HH_ */
