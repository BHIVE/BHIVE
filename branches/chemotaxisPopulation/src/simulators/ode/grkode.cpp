/*
 * grkode.cpp
 *
 *  Created on: Dec 14, 2009
 *      Author: jentsch
 */

#include "grkode.hh"
#include <iomanip>

using namespace Hive;

GRKOde::GRKOde() {}


GRKOde::~GRKOde() {}


void GRKOde::setAgent(Agent *ag) {
	this->ag = ag;
	this->stoch_matrix  = (TVectorData<TVectorData<int> >* ) this->ag->getDatabase()->getDataItem("stoich_matrix");
	this->rate_ptrs     = (TVectorData<GRate* > *) this->ag->getDatabase()->getDataItem("rate_vector");
	this->num_reactions = this->rate_ptrs->size();
	this->conc          = (TVectorData<double> *) this->ag->getDatabase()->getDataItem("concentrations");
	this->num_species   = this->conc->size();
}


void GRKOde::step(double t) {
	// local variable
	int i;

	// declaration of the input output vectors
	vector<double> output(num_species,0.0);
	vector<double> input(num_species, 0.0);

	double num_steps = t/dt;

	for (int index = 0; index < num_steps; index++) {
		// initialise y_n
		vector<double> conc_0(num_species, 0.0);
		for (i=0; i<num_species; i++) {
			conc_0[i] = (*conc)[i];
			output[i] = 0.0;
		}

		// calculate k1
		this->singleSubStep(conc_0, output);
		vector<double> conc_1(num_species, 0.0);
		for (i=0; i<num_species; i++) {
			conc_1[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k2 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + 0.5*conc_1[i];
		// calculate k2
		this->singleSubStep(input, output);
		vector<double> conc_2(num_species, 0.0);
		for (i=0; i<num_species; i++) {
			conc_2[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k3 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + 0.5*conc_2[i];
		// calculate k3
		this->singleSubStep(input, output);
		vector<double> conc_3(num_species, 0.0);
		for (i=0; i<num_species; i++) {
			conc_3[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k4 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + conc_3[i];
		// calculate k4
		this->singleSubStep(input, output);
		vector<double> conc_4(num_species, 0.0);
		for (i=0; i<num_species; i++)
			conc_4[i] = dt*output[i];

		for (i=0; i<num_species; i++)
			conc->setEntry(i, conc_0[i] + 1./6*conc_1[i] + 1./3*conc_2[i] + 1./3*conc_3[i] + 1./6*conc_4[i]);
	}
}


void GRKOde::prepare() {
	// does not do anything ... there is nothing to prepare
}


void GRKOde::setTimeStep(double dt) {
	this->dt = dt;
}


void GRKOde::synchroniseWithDatabase() {
	// does not do anything
}


// runs a little test by setting up a lotka-volterra simulation
// X + Y_1 -> 2 Y_1 (10.0)
// Y_1 + Y_2 -> 2 Y_2 (0.01)
// Y_2 -> Z (10.0)
void GRKOde::testing() {
	cerr << "# entering the GRKOde::testing() routine " << endl;
	// begin by initializing the relevant data structures
	this->num_reactions = 3;
	this->num_species = 3;
	// initialize concentrations
	this->conc = new TVectorData<double> ("concentrations", "tvectordata_double");
	this->conc->reserveSize(3);
	this->conc->at(0) = 10;
	this->conc->at(1) = 100;
	this->conc->at(2) = 0.;
	// initialize reactions
	this->rate_ptrs = new TVectorData<GRate* > ("rate_ptrs", "tvectordata_grate");
	this->rate_ptrs->reserveSize(3);
	GRate *g1 = new GRate("first_reac", "grate");
	g1->addEductSpecies(1,0);
	g1->addProductSpecies(2,0);
	g1->addRate(10.0);
	this->rate_ptrs->at(0) = g1;
	GRate *g2 = new GRate("second_reac", "grate");
	g2->addEductSpecies(1,0);
	g2->addEductSpecies(1,1);
	g2->addProductSpecies(2,1);
	g2->addRate(0.01);
	this->rate_ptrs->at(1) = g2;
	GRate *g3 = new GRate("thir_reac", "grate");
	g3->addEductSpecies(1,1);
	g3->addProductSpecies(1,2);
	g3->addRate(10.0);
	this->rate_ptrs->at(2) = g3;

	// build up stoichiometry matrix
	stoch_matrix = new TVectorData<TVectorData<int> > ("stoch_matrix", "tvectordata_tvectordata_int");
	stoch_matrix->reserveSize(3);
	for (int i = 0; i<3; i++)
		stoch_matrix->at(i).reserveSize(3);

	// now set up the stoichiometric matrix.
	int species, reac, value;
	//cerr << rates.size() << endl;
	for (species = 0; species < 3; species++) {
		for (reac = 0; reac<3; reac++) {
			value = rate_ptrs->at(reac)->includedAsProduct(species) - rate_ptrs->at(reac)->includedAsEduct(species);
			// cerr << species << " "  << reac << " " << value << endl;
			stoch_matrix->at(species).setEntry(reac, value);
		}
	}

	cerr << "# outputting the stoichiometric matrix" << endl;
	for (int i=0; i<stoch_matrix->size(); i++) {
		for (int j=0; j<stoch_matrix->at(i).size(); j++)
			cerr << setfill(' ') << setw(3) << stoch_matrix->at(i).at(j) << "  ";
		cerr << endl;
	}

	//	vector<double> input; vector<double> output;
	//	for (int i=0; i<this->conc->size(); i++) {
	//		input.push_back(conc->at(i)); output.push_back(0);
	//	}
	//	singleSubStep(input,output);
	//	cerr << output[0] << " " << output[1] << " " << output[2] << endl;

	double time = 0;
	ofstream out("runge_test_output.txt");
	out << "# testing output for runge kutta integrator" << endl;
	out << time << " " << this->conc->at(0) << " " << this->conc->at(1) << endl;
	this->dt = 0.01;
	while (time < 20.0) {
		this->step(0.01);
		time+=0.01;
		out << time << " " << this->conc->at(0) << " " << this->conc->at(1) << endl;
	}

	// remove data structure
	delete this->rate_ptrs->at(0);
	delete this->rate_ptrs->at(1);
	delete this->rate_ptrs->at(2);
	delete rate_ptrs;
	delete conc;
	delete stoch_matrix;

	cerr << "# leaving the GRKOde::testing() routine " << endl;
}


// this method is not really worth any thing
void GRKOde::generateOutputForValidation() {
	cerr << "# in GRKOde::generateOutputForValidation" << endl;
	vector<double> input; vector<double> output;
	// check concentrations
	for (int i=0; i<this->conc->size(); i++) {
		input.push_back(conc->at(i)); output.push_back(0);
		cerr << conc->at(i) << " " ;
	}
	cerr << endl;
	// here we will check whether the rates are calculated correctly
	singleSubStep(input, output);
	cerr << output[0] << " " << output[1] <<" " << output[2] << endl;

	step(1.);
	for (int i=0; i<this->conc->size(); i++)
		cerr << conc->at(i) << " ";
	cerr << endl;
}


void GRKOde::initialise() {
	// does not do anything ...
}



void GRKOde::singleSubStep(vector<double>& input, vector<double>& output) {
	// this is the most inefficient way of calculating this ... one obvious improvement would be
	// to use Sparselib++  ...
	for (int i=0; i<num_species; i++)
		for (int j=0; j<num_reactions; j++)
			output[i] += (*stoch_matrix)[i][j]*this->rate_ptrs->at(j)->calcRate(input);

}


