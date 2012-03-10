/*
 * grate.cpp
 *
 *  Created on: Dec 14, 2009 last edited: dec 17, 2009
 *
 *      Author: jentsch
 */

#include "grate.hh"

using namespace Hive;

GRate::GRate(string name, string type) : Data(name, type) {}


GRate::~GRate() {}

void GRate::printContent(ostream& out) {
	cerr << "# outputting rate of " << this->getName() << endl;
	for (unsigned int i = 0; i<indices_of_involved_educt_species.size(); i++) {
		cerr << stoichiometry[i] << "x" << indices_of_involved_educt_species[i];
		if (i == indices_of_involved_educt_species.size() - 1)
			cerr << " -> ";
		else
			cerr << " + ";
	}
	for (unsigned int i = 0; i<indices_of_involved_product_species.size(); i++) {
		cerr << stoichiometry[i+indices_of_involved_educt_species.size()] << "x"
		<< indices_of_involved_product_species[i];
		if (i == indices_of_involved_product_species.size() - 1)
			cerr << " " << rate << endl;
		else
			cerr << " + " ;
	}
}

double GRate::calcRate(vector<double>& input) {
	double result = this->rate;
	// do the calculations
	for (int i=0; i<indices_of_involved_educt_species.size(); i++)
		for (int j=0; j<this->stoichiometry[i]; j++)
			result *= input[indices_of_involved_educt_species[i]];
	return result;
}

void GRate::addEductSpecies(int number, int species) {
	indices_of_involved_educt_species.push_back(species);
	stoichiometry.push_back(number);
}

void GRate::addProductSpecies(int number, int species) {
	indices_of_involved_product_species.push_back(species);
	stoichiometry.push_back(number);
}

void GRate::addRate(double r) {
	this->rate = r;
}

double GRate::calcPropensity(vector<int>& input) {
	// we assume that the rates have been rescaled adequately to account for the
	// the volume of the agent in which they occur
	double result = this->rate;
	// calculate mescoscopic reaction rate
	for (int reac=0; reac<indices_of_involved_educt_species.size(); reac++) {
		for (int i=1; i<=stoichiometry[reac]; i++)
			result *= (input[indices_of_involved_educt_species[reac]] - (i-1));
		result /= stoichiometry[reac];
	}
	return result;
}

double GRate::calcPropensity(vector<double>& input) {
	// we assume that the rates have been rescaled adequately to account for the
	// the volume of the agent in which they occur
	double result = this->rate;
	int number;
	for (unsigned int reac=0; reac<indices_of_involved_educt_species.size(); reac++) {
		// first we have to convert the doubles into integer particle numbers
		number = ((int)input[indices_of_involved_educt_species[reac]]);
		if (Util::RANDOM_CLOSED() < (input[indices_of_involved_educt_species[reac]]-number))
			number += 1;
		// calculate mesoscopic reaction rate
		for (int i=1; i<=stoichiometry[reac]; i++)
			result *= (number - (i-1));
		result /= stoichiometry[reac];
	}
	return result;
}

double GRate::calcPropensity(vector<int>& input, double volume) {
	// get the result from method above
	double result = this->calcPropensity(input);
	// check whether rescaling is necessary
	if (this->indices_of_involved_educt_species.size() == 2 ||
		(this->indices_of_involved_educt_species.size() == 1 && stoichiometry[0] == 2))
		return result/volume;
	else
		return result;
}

double GRate::calcPropensity(vector<double>& input, double volume) {
	// get the result from method above
	double result = this->calcPropensity(input);
	// check for rescaling
	if (this->indices_of_involved_educt_species.size() == 2 ||
		this->indices_of_involved_educt_species.size() == 1 && stoichiometry[0] == 2)
		return result/volume;
	else
		return result;
}

double GRate::calcPropensity(TVectorData<int> *input, double volume) {
	double result = this->rate;
	int number;
	for (unsigned int reac=0; reac<indices_of_involved_educt_species.size(); reac++) {
		number = input->at(indices_of_involved_educt_species[reac]);
		for (int i=1; i<=stoichiometry[reac]; i++)
			result *= (number - (i-1));
		result /= stoichiometry[reac];
	}
	if (this->indices_of_involved_educt_species.size() == 2 ||
		this->indices_of_involved_educt_species.size() == 1 && stoichiometry[0] == 2)
		result /= volume;
	return result;
}

void GRate::copy(GRate *r) {
	this->indices_of_involved_educt_species.resize(r->indices_of_involved_educt_species.size());
	for (unsigned int i=0; i<this->indices_of_involved_educt_species.size(); i++)
		this->indices_of_involved_educt_species.at(i) = r->indices_of_involved_educt_species.at(i);

	this->indices_of_involved_product_species.resize(r->indices_of_involved_product_species.size());
	for (unsigned int i=0; i<this->indices_of_involved_product_species.size(); i++)
		this->indices_of_involved_product_species.at(i) = r->indices_of_involved_product_species.at(i);

	this->stoichiometry.resize(r->stoichiometry.size());
	for (unsigned int i=0; i<this->stoichiometry.size(); i++)
		this->stoichiometry.at(i) = r->stoichiometry.at(i);

	this->rate = r->rate;

}

void GRate::execute(TVectorData<int> *input) {
	for (unsigned int reac=0; reac<indices_of_involved_educt_species.size(); reac++)
		input->at(indices_of_involved_educt_species[reac]) -= stoichiometry[reac];
	for (unsigned int prod=0; prod<indices_of_involved_product_species.size(); prod++)
		input->at(indices_of_involved_product_species[prod]) +=
			stoichiometry[prod+indices_of_involved_educt_species.size()];
}

int GRate::getEductSpecies(int index) {
	return indices_of_involved_educt_species[index];
}

int GRate::getProductSpecies(int index) {
	return indices_of_involved_product_species[index];
}

int GRate::getEductStoichiometry(int index) {
	return stoichiometry[index];
}

int GRate::getProductStoichiometry(int index) {
	return stoichiometry[indices_of_involved_product_species.size() + index];
}

int GRate::includedAsEduct(int species_index) {
	unsigned int index;
	bool involved = false;
	for (index = 0; index<indices_of_involved_educt_species.size(); index++)
		if (indices_of_involved_educt_species[index] == species_index) {
			involved = true;
			break;
		}
	if (involved)
		return stoichiometry[index];
	else
		return 0;
}

int GRate::includedAsProduct(int species_index) {
	unsigned int index;
	bool involved = false;
	for (index = 0; index<indices_of_involved_product_species.size(); index++)
		if (indices_of_involved_product_species[index] == species_index) {
			involved = true;
			break;
		}
	if (involved)
		return stoichiometry[index+indices_of_involved_educt_species.size()];
	else
		return 0;
}
