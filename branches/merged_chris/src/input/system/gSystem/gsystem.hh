/*
 * gsystem.hh
 *
 *  Created on: Dec 15, 2009
 *      Author: jentsch
 */

#ifndef GSYSTEM_HH_
#define GSYSTEM_HH_

#include "../system.hh"
#include "../../../agent/data/rate/grate.hh"
#include "../../../agent/data/template/tdata.hh"

#include <iomanip>

using namespace std;

namespace Hive {

	class GSystem : public System {
	public:
		GSystem():System(1) {
			stoichiometry = new TVectorData<TVectorData<int> > ();
			rates = new TVectorData<GRate*> ();
			concentration_names = new TVectorData<string> ();};
		~GSystem() {
			cerr << "# entering gsystem destructor" << endl;
			delete stoichiometry;
			// note rates is a TVectorData<T* > object. hence it
			// destroys the rates although it has not build them.
			delete rates;
			delete concentration_names;
			cerr << "# leaving gsystem destructor" << endl;
		};

		void printContent(ostream& ) {
			cerr << "# outputting GSystem of type " << this->getSystemType() << endl;
			cerr << "# the system contains the following species: " << endl;
			for (int i=0; i<concentration_names->size(); i++)
				cerr << concentration_names->at(i) << endl;

			cerr << "# outputting the stoichiometric matrix" << endl;
			for (int i=0; i<stoichiometry->size(); i++) {
				for (int j=0; j<stoichiometry->at(i).size(); j++)
					cerr << setfill(' ') << setw(3) << stoichiometry->at(i).at(j) << "  ";
				cerr << endl;
			}

			cerr << "# outputting the rates" << endl;
			for (int i=0; i<rates->size(); i++)
				rates->at(i)->printContent(cerr);
		};


		TVectorData<string>* getPtrToConcentrationNames() { return concentration_names; };
		TVectorData<GRate*>* getPtrToRatesVector() { return rates; };
		TVectorData<TVectorData<int> >* getPtrToStoichiometryMatrix() { return stoichiometry; };
		friend class GSystemReader;

	protected:
		TVectorData<TVectorData<int> > *stoichiometry;
		TVectorData<GRate* > *rates;
		TVectorData<string> *concentration_names;

	};
}

#endif /* GSYSTEM_HH_ */
