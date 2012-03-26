/*
 * gsystemreader.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: jentsch
 */

#include "gsystemreader.hh"

using namespace Hive;

GSystemReader::GSystemReader() {
	gsys = new GSystem();

}

GSystemReader::~GSystemReader() {
	cerr << "# deleting gsystemreader" << endl;
	delete gsys;
//	for (unsigned int i=0; i<rates.size(); i++)
//		if (rates[i] != NULL)
//			delete rates[i];
	cerr << "# done deleting gsystemreader" << endl;
}

System* GSystemReader::parseSystem(ifstream& in) {
	// make sure that we start at the beginning
	in.seekg(0, ios_base::beg);

	// local variables
	string line ="";
	istringstream iss;

	string species_name;
	string word;
	map<string, int> species_index_lookup;
	vector<GRate* > rates;
	int species_number = 0;
	while (getline(in,line)) {
		if (line == "SPECIES") {
			cerr << "# parsing species for gsystem ...";
			while (getline(in,line) && line != "END") {
				if (line.substr(0,1)!="#" && line != "") {
					iss.clear();
					iss.str(line);
					iss >> species_name;
					species_index_lookup[species_name] = species_number++;
				}
			}
			cerr << "... done" << endl;
		}
		if (line == "REACTIONS") {
			cerr << "# parsing reactions for gsystem ...";
			while(getline(in,line) && line != "END") {
				if (line.substr(0,1)!="#" && line != "") {
					iss.clear();
					iss.str(line);
					// temporary memory
					vector<string> reactants;
					vector<string> products;
					vector<int>    stochiometry;
					vector<string> sentence;
					int num_words = 0;
					while (iss >> word) {
						sentence.push_back(word);
						num_words += 1;
					}
                    // state for deciding whether something is a reactant or product
                    int state = 0;
                    int increment = 2;
                    for (unsigned int i=0; i<sentence.size(); i+=increment) {
                            if ((i+2 < sentence.size()) &&
                                    (sentence[i+2] == "+" || sentence[i+2]== "->" || i == sentence.size()-3))
                                    increment = 3;
                            else
                                    increment = 2;
                            // no stochiometric coefficients given in file
                            if (increment == 2) {
                                    int number = 1;
                                    bool already_present = false;
                                    if (state == 0) {
                                            // reactant
                                            // first we have to check whether there is already such a reactant
                                            unsigned position = 0;
                                            for (position=0; position<reactants.size(); position++)
                                                    if (reactants[position] == sentence[i]) {
                                                            already_present = true;
                                                            break;
                                                    }
                                            if (already_present == true)
                                                    stochiometry[position] += 1;
                                            else
                                                    reactants.push_back(sentence[i]);
                                    }
                                    else
                                            // product
                                            products.push_back(sentence[i]);
                                    if (!already_present)
                                            stochiometry.push_back(number);
                                    // check for state change
                                    if (sentence[i+1]=="->")
                                            state = 1;
                            } else { // stochiometric coefficients given in file
                                    int number = atoi(sentence[i].c_str());
                            //      int s = species_index_connector[sentence[i+1]];
                                    if (state == 0)
                                            reactants.push_back(sentence[i+1]);
                                    else
                                            products.push_back(sentence[i+1]);
                                    stochiometry.push_back(number);
                                    // check for state change
                                    if (sentence[i+2]=="->")
                                            state = 1;
                            }
                    }
                    double rate = strtod(sentence[num_words-1].c_str(),NULL);
                    // now we have to set up the reaction rate (grate)
                    ostringstream reac_name;
                    reac_name << "reac_" << rates.size();
                    GRate *r = new GRate(reac_name.str(),"grate");
                    for (unsigned i=0; i<reactants.size(); i++)
                    	r->addEductSpecies(stochiometry[i],species_index_lookup[reactants[i]]);
                    for (unsigned int i=0; i<products.size(); i++)
                    	r->addProductSpecies(stochiometry[i+reactants.size()],species_index_lookup[products[i]]);
                    r->addRate(rate);
                    rates.push_back(r);
				}
			}
			cerr << "... done" << endl;
		}
	}

	// add concentration name to the gsys
	gsys->concentration_names->reserveSize(species_number);
	int hans = 0;
	map<string,int>::iterator iter;
	for (iter = species_index_lookup.begin(); iter != species_index_lookup.end(); iter++)
		gsys->concentration_names->setEntry(hans++, iter->first);

	// add the rates to the
	gsys->rates->reserveSize(rates.size());
	for (unsigned int i=0; i<rates.size(); i++)
		gsys->rates->setEntry(i, rates[i]);

	// construct the stoichiometric matrix of the gsystem ...
	gsys->stoichiometry->reserveSize(species_number);
	for (int i = 0; i<species_number; i++)
		gsys->stoichiometry->at(i).reserveSize(rates.size());

	// now set up the stoichiometric matrix.
	int species, reac, value;
	//cerr << rates.size() << endl;
	for (species = 0; species < species_number; species++) {
		for (reac = 0; reac<rates.size(); reac++) {
			value = rates[reac]->includedAsProduct(species) - rates[reac]->includedAsEduct(species);
			// cerr << species << " "  << reac << " " << value << endl;
			gsys->stoichiometry->at(species).setEntry(reac, value);
		}
	}

	this->sys = gsys;

	// finally return the system
	return gsys;
}
