/*
 * grkode.cpp
 *
 *  Created on: Dec 14, 2009
 *      Author: jentsch
 */

#include "grkode.hh"
#include <iomanip>

using namespace Hive;

GRKOde::GRKOde() { i = 0; j = 0; num_entries = 0;}


GRKOde::~GRKOde() {
	delete this->stoch;
}


void GRKOde::setAgent(Agent *ag) {
	this->ag              = ag;
	this->stoch_matrix    = (TVectorData<TVectorData<int> >* ) this->ag->getDatabase()->getDataItem("stoich_matrix");
	this->rate_ptrs       = (TVectorData<GRate* > *) this->ag->getDatabase()->getDataItem("rate_vector");
	this->num_reactions   = this->rate_ptrs->size();
	this->conc            = (TVectorData<double> *) this->ag->getDatabase()->getDataItem("concentrations");
	this->num_species     = this->conc->size();
	this->time            = (DoubleData *) this->ag->getDatabase()->getDataItem("time");
	this->propagate_until = (DoubleData *) this->ag->getDatabase()->getDataItem("local_propagate_until");

	this->rate.resize(this->num_reactions);
	this->input.resize(this->num_species);
	this->output.resize(this->num_species);
	this->conc_0.resize(this->num_species);
	this->conc_1.resize(this->num_species);
	this->conc_2.resize(this->num_species);
	this->conc_3.resize(this->num_species);
	this->conc_4.resize(this->num_species);

	// set up local stochiometry matrix

	for (i=0; i<num_species; i++) {
		for (j=0; j<num_reactions; j++)
			if ((*stoch_matrix)[i][j] != 0) {
				num_entries += 1;
				col_index.push_back(j);
				row_index.push_back(i);
				values.push_back((*stoch_matrix)[i][j]);
			}
	}


	double v[values.size()];
	int r[row_index.size()];
	int c[col_index.size()];

	for (i=0; i<num_entries; i++) {
		v[i] = values[i];
		c[i] = col_index[i];
		r[i] = row_index[i];
	}

	this->stoch = new Coord_Mat_double(this->num_species, this->num_reactions, num_entries, &v[0], &r[0], &c[0]);



}

void GRKOde::singleStep(double t) {
	// calculate the time step over which we have to propagate
	// save a pointer here!!!!
	// can't we move this into an action?
	if (((BoolData*) ag->getDatabase()->getDataItem("event_occured"))->getBool()) {
		this->dt = this->propagate_until->getDouble() - this->time->getDouble();
	} else
		this->dt = ((DoubleData *) this->ag->getDatabase()->getDataItem("propagate_until"))->getDouble() - time->getDouble();

	// initialise y_n
	for (i=0; i<num_species; i++) {
		conc_0[i] = (*conc)[i];
		output[i] = 0.0;
	}

	// calculate k1
	this->singleSubStep(conc_0, output);
	for (i=0; i<num_species; i++) {
		conc_1[i] = dt*output[i];
		output[i] = 0.0;
	}

	// prepare input for k2 calculation
	for (i=0; i<num_species; i++)
		input[i] = conc_0[i] + 0.5*conc_1[i];
	// calculate k2
	this->singleSubStep(input, output);
	for (i=0; i<num_species; i++) {
		conc_2[i] = dt*output[i];
		output[i] = 0.0;
	}

	// prepare input for k3 calculation
	for (i=0; i<num_species; i++)
		input[i] = conc_0[i] + 0.5*conc_2[i];
	// calculate k3
	this->singleSubStep(input, output);
	for (i=0; i<num_species; i++) {
		conc_3[i] = dt*output[i];
		output[i] = 0.0;
	}

	// prepare input for k4 calculation
	for (i=0; i<num_species; i++)
		input[i] = conc_0[i] + conc_3[i];
	// calculate k4
	this->singleSubStep(input, output);
	for (i=0; i<num_species; i++)
		conc_4[i] = dt*output[i];

	for (i=0; i<num_species; i++)
		conc->setEntry(i, conc_0[i] + 1./6*conc_1[i] + 1./3*conc_2[i] + 1./3*conc_3[i] + 1./6*conc_4[i]);

	this->time->setDouble(this->time->getDouble() + dt);

	// check for updates
	// -------- ATTENTION -------- ATTENTION ------- ATTENTION ------ ATTENTION -------
	// this is bad!!!! the single step method should not know how to perform the update
	// this really belongs someweher else !!!!!!
	if (((BoolData*) ag->getDatabase()->getDataItem("event_occured"))->getBool()) {
		TVectorData<double> *remember = (TVectorData<double> *) ag->getDatabase()->getDataItem("remember");
		conc->at(5) = conc->at(5) + remember->at(1);
		// set the pstat5 numbers
		conc->at(6) = conc->at(6) - remember->at(2);
		// set the chrom_il2 numbers
		conc->at(7) = remember->at(3);
		conc->at(8) = remember->at(4);
		conc->at(9) = remember->at(5);
		// set the chrom_reca numbers
		conc->at(10) = remember->at(6);
		conc->at(11) = remember->at(7);
		// set the chrom_recb numbers
		conc->at(12) = remember->at(9);
		conc->at(13) = remember->at(10);
		((BoolData*)ag->getDatabase()->getDataItem("event_occured"))->setBool(false);
	}
}


void GRKOde::step(double t) {

	singleStep(t);
	/*
	// declaration of the input output vectors
	vector<double> output(num_species,0.0);
	vector<double> input(num_species, 0.0);

	double num_steps = t/dt;

	for (int index = 0; index < num_steps; index++) {
		// initialise y_n
		for (i=0; i<num_species; i++) {
			conc_0[i] = (*conc)[i];
			output[i] = 0.0;
		}

		// calculate k1
		this->singleSubStep(conc_0, output);
		for (i=0; i<num_species; i++) {
			conc_1[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k2 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + 0.5*conc_1[i];
		// calculate k2
		this->singleSubStep(input, output);
		for (i=0; i<num_species; i++) {
			conc_2[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k3 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + 0.5*conc_2[i];
		// calculate k3
		this->singleSubStep(input, output);
		for (i=0; i<num_species; i++) {
			conc_3[i] = dt*output[i];
			output[i] = 0.0;
		}

		// prepare input for k4 calculation
		for (i=0; i<num_species; i++)
			input[i] = conc_0[i] + conc_3[i];
		// calculate k4
		this->singleSubStep(input, output);
		for (i=0; i<num_species; i++)
			conc_4[i] = dt*output[i];

		for (i=0; i<num_species; i++)
			conc->setEntry(i, conc_0[i] + 1./6*conc_1[i] + 1./3*conc_2[i] + 1./3*conc_3[i] + 1./6*conc_4[i]);
	}
	*/
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
	for (species = 0; species < 3; species++) {
		for (reac = 0; reac<3; reac++) {
			value = rate_ptrs->at(reac)->includedAsProduct(species) - rate_ptrs->at(reac)->includedAsEduct(species);
			stoch_matrix->at(species).setEntry(reac, value);
		}
	}

	cerr << "# outputting the stoichiometric matrix" << endl;
	for (int i=0; i<stoch_matrix->size(); i++) {
		for (int j=0; j<stoch_matrix->at(i).size(); j++)
			cerr << setfill(' ') << setw(3) << stoch_matrix->at(i).at(j) << "  ";
		cerr << endl;
	}

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
	cerr << "# done running the test ... now deleting" << endl;
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
 	for (i=0; i<num_reactions; i++)
 		rate[i] = this->rate_ptrs->at(i)->calcRate(input);

 	// do the calculations
 	for (i = 0; i<this->num_entries; i++)
 		output[stoch->row_ind(i)] += stoch->val(i) * rate[stoch->col_ind(i)];

//		output[this->row_index[i]] += this->values[i] * rate[this->col_index[i]];

}


