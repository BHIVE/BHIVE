/*
 * grate.hh  stands for garrit's rate ...dumb name but the first that came to my egocentric mind.
 *
 *  Created on: Dec 14, 2009
 *      Author: jentsch
 */

#ifndef GRATE_HH_
#define GRATE_HH_

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "../data.hh"
#include "../template/tdata.hh"
#include "../../../util/rand/rand.hh"


using namespace std;

namespace Hive {
	/**
	 * @brief simple class for storing rates for garrit's runge kutta ode integrator. this is a tentative class. it
	 * is only capable of storing and calculating the rates of mass action reactions. we store both the left and right
	 * hand sides of the reactions. the hope would be that this rate can be used both for deterministic as well as
	 * stochastic simulations.
	 *
	 * @todo add stochastic capabilities / error checking! default constructor missing!
	 *
	 * @author garrit jentsch
	 *
	 * @date December, 14th 2009 last edited 12-17-09 by garrit
	 */
	class GRate : public Data {
	public:
		GRate(string name, string type);
		virtual ~GRate();

		virtual void printContent(ostream& out);

		void addEductSpecies(int number, int species);

		void addProductSpecies(int number, int species);

		void addRate(double);

		/// method for stochastic simulations calculating the propensity of a reaction
		double calcPropensity(vector<double>& );

		/// method for stochastic simulations calculating the propensity of a reaction
		double calcPropensity(vector<int>&);

		/// method for stochastic simulations calculating the propensity of a reaction
		///  where the reaction rate has to be rescaled by the volume
		double calcPropensity(vector<double>&, double volume);

		/// method for stochastic simulations calculating the propensity of a reaction
		/// where the reaction rate has to be rescaled by the volume
		double calcPropensity(vector<int>&, double volume);

		/// method for stochastic simulations calculating the propensity of a reaction
		/// where the reaction rate has to be rescaled by the volume and where the particle
		/// numbers are given by a pointer to a TVectorData object
		double calcPropensity(TVectorData<int>*, double volume);

		/// method for deterministic simulations calculating the rate of a reaction
		double calcRate(vector<double>&);

		void copy(GRate*);

		/// executes reaction and manipulates given particle_numbers
		void execute(TVectorData<int>*);

		int getEductSpecies(int index);

		int getProductSpecies(int index);

		int getEductStoichiometry(int index);

		int getProductStoichiometry(int index);

		int includedAsProduct(int species_index);

		int includedAsEduct(int species_index);

	protected:
		vector<int> indices_of_involved_educt_species;

		vector<int> indices_of_involved_product_species;

		vector<int> stoichiometry;

		// reaction rate
		double rate;
	};
}

#endif /* GRATE_HH_ */
