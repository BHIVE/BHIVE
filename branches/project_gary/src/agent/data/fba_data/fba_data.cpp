#include "fba_data.hh"
#include <stdlib.h>

using namespace Hive;

FBAData::~FBAData() {
	for (int i=0; i < int(compounds.size()); i++) {
		delete compounds[i];
	}
	for (int i=0; i < int(reactions.size()); i++) {
		delete reactions[i];
	}
}

string FBAData::GetFileLine(ifstream &Input) {
	string Buff; 
	getline( Input, Buff );
	return Buff;
}

vector<string>* FBAData::StringToStrings(string FullString, const char* Delim, bool TreatConsecutiveDelimAsOne) {
	vector<string>* NewVect = new vector<string>;
	string Buff(FullString);

	int Location;
	do {
		Location = int(Buff.find_first_of(Delim));
		if (Location != -1) {
			if (Location == 0) {
				if (!TreatConsecutiveDelimAsOne) {
					string NewString;
					NewVect->push_back(NewString);
				}
				Buff = Buff.substr(Location+1, Buff.length()-(Location+1));
			} else {
				string NewString = Buff.substr(0, Location);
				NewVect->push_back(NewString);
				Buff = Buff.substr(Location+1, Buff.length()-(Location+1));
			}
		}
	} while(Location != -1);
	
	if (Buff.length() != 0 || !TreatConsecutiveDelimAsOne) {
		NewVect->push_back(Buff);
	}
	
	return NewVect;
}

void FBAData::load(string filename) {
	ifstream input(filename.data());
	
	bool parsing_compounds = false;
	bool parsing_reactions = false;
	vector<string>* compound_headers = NULL;
	vector<string>* reaction_headers = NULL;
	while (!input.eof()) {
		string line = GetFileLine(input);
		if (line.compare("COMPOUNDS") == 0) {
			parsing_compounds = true;
			parsing_reactions = false;
			compound_headers = StringToStrings(GetFileLine(input),";",false);
		} else if (line.compare("REACTIONS") == 0) {
			parsing_reactions = true;
			parsing_compounds = false;
			reaction_headers = StringToStrings(GetFileLine(input),";",false);
		} else if (parsing_compounds) {
			vector<string>* strings = StringToStrings(line,";",false);
			Compound* newcpd = new Compound;
			for (int i=0; i < int(compound_headers->size()); i++) {
				if ((*compound_headers)[i].compare("Name") ==0) {
					compound_map[(*strings)[i]] = newcpd;
					newcpd->name = (*strings)[i];
				} else if ((*compound_headers)[i].compare("Concentrations") ==0) {
					newcpd->concentration = atof((*strings)[i].data());
				}
			}
			compounds.push_back(newcpd);
			delete strings;
		} else if (parsing_reactions) {
			vector<string>* strings = StringToStrings(line,");",true);
			Reaction* newrxn = new Reaction;
			for (int i=0; i < int(reaction_headers->size()); i++) {
				if ((*reaction_headers)[i].compare("Name") ==0) {
					reaction_map[(*strings)[i]] = newrxn;
					newrxn->name = (*strings)[i];
				} else if ((*reaction_headers)[i].compare("k1") ==0) {
					if ((*strings)[i].compare("FBA") != 0) {
						newrxn->k1 = atof((*strings)[i].data());
					} else {
						newrxn->k1 = -1;
					}
				} else if ((*reaction_headers)[i].compare("k2") ==0) {
					if ((*strings)[i].compare("FBA") != 0) {
						newrxn->k2 = atof((*strings)[i].data());
					} else {
						newrxn->k2 = -1;
					}
				} else if ((*reaction_headers)[i].compare("e") ==0) {
					if ((*strings)[i].compare("FBA") != 0) {
						newrxn->e = atof((*strings)[i].data());
					} else {
						newrxn->e = -1;
					}
				} else if ((*reaction_headers)[i].compare("Equation") ==0) {
					vector<string>* equation_parts = StringToStrings((*strings)[i],")[ +",true);
					int sign = -1;
					double coeff = 1;
					char compartment = 'c';
					for (int j=0; j < int(equation_parts->size()); j++) {
						if ((*equation_parts)[j].compare("=>") == 0) {
							sign = 1;
						} else if ((*equation_parts)[j].substr(0,1).compare("(") == 0) {
							coeff = atof((*equation_parts)[j].substr(1,(*equation_parts)[j].length()-1).data());
						} else if ((*equation_parts)[j].substr(1,1).compare("]") == 0) {
							compartment = (*equation_parts)[j][0];
							if (compartment != 'c') {
								newrxn->compartments[newrxn->compartments.size()-1] = compartment;
							}
						} else {
							string reactant = (*equation_parts)[j];
							Compound* newreactant = compound_map[reactant];
							if (newreactant == NULL) {
								cerr << "Unrecognized reactant " << reactant << " in " << newrxn->name << endl;
							} else {
								newrxn->reactants.push_back(newreactant);
								newrxn->compartments.push_back('c');
								newrxn->coefficejnts.push_back(sign*coeff);
							}
							coeff = 1;
						}
					}
					delete equation_parts;
				}
			}
			reactions.push_back(newrxn);
			delete strings;
		}
	}
	if (reaction_headers != NULL) {
		delete reaction_headers;
	}
	if (compound_headers != NULL) {
		delete compound_headers;
	}

	input.close();
}

int FBAData::getNumberOfReactions() {
	return int(reactions.size());
}

int FBAData::getNumberOfCompounds() {
	return int(reactions.size());
}

Reaction* FBAData::getReaction(int i) {
	return reactions[i];
}

Compound* FBAData::getCompound(int i) {
	return compounds[i];
}

Compound* FBAData::getCompound(string name) {
	return compound_map[name];
}
