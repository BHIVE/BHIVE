/**
 * @file fba_data.hh
 *
 * this file contains the interfaces to the classes Data, ...
 *
 *
 * @date Oct 7, 2009  Last edited: Oct 14, 2009 by Ric C. and Chris Henry
 *
 * @author Ric C. and Chris Henry
 */

#ifndef FBA_DATA_HPP_
#define FBA_DATA_HPP_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "../../agent/data/data.hh"

using namespace std;
using namespace Hive;

/**
 * @brief individual element of the database
 *
 *  specific class to store concentration grid for a diffusion simulator
 *
 *  @todo
 *
 *  @date Oct 19, 2009 Last edited: Oct 19, 2009 by Garrit and Michael
 *
 *  @author Ric C. and Chris Henry
 */

namespace Chemotaxis {

struct Compound {
	string name;
	double concentration;
	double flux;
};

struct Reaction {
	string name;
	double k1;
	double e;
	double k2;
	vector<Compound*> reactants;
	vector<double> coefficejnts;
	vector<char> compartments;
};

class FBAData : public Data {
public:

	/**
	 * constructor
	 * @param name dataname
	 */
	FBAData(string name="fba_network", string type="fba_type"):Data(name, type) {};

	/// Destructor
	~FBAData();

	void load(string filename);
	void printContent(ostream& out) { cout<<"cannot print FBA data yet"<<endl; };

	vector<string>* StringToStrings(string FullString, const char* Delim, bool TreatConsecutiveDelimAsOne = true);
	string GetFileLine(ifstream &Input);

	int getNumberOfReactions();
	int getNumberOfCompounds();
	Reaction* getReaction(int i);
	Compound* getCompound(int i);
	Compound* getCompound(string name);
protected:
	map<string,Compound*> compound_map;
	map<string,Reaction*> reaction_map;
	vector<string> compartments;

	vector<Reaction*> reactions;
	vector<Compound*> compounds;

};

}

#endif /* DATA_HPP_ */
