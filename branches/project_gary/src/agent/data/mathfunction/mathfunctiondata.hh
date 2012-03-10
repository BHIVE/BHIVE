/*
 * mathfunctiondata.hh
 *
 *  Created on: Apr 16, 2010
 *      Author: jentsch
 */

#ifndef MATHFUNCTIONDATA_HH_
#define MATHFUNCTIONDATA_HH_

#include "../data.hh"
#include "../../../exception/exception.hh"

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

namespace Hive {
	// base class does this work ... needs to be tested ...
	class MathFunctionData : public Data {
	public:
		MathFunctionData(string dataName):Data(dataName,"MathFunctionType") {};
		virtual ~MathFunctionData() {};

		virtual double getValue(vector<double>& pos, double t)=0;

		virtual void printContent(ostream& out)=0;

	};


	/**
	 * @brief one dimensional linear function
	 *
	 */
	class OneDLinearMathFunctionData : public MathFunctionData {
	public:
		OneDLinearMathFunctionData(string dataName):MathFunctionData(dataName){};
		virtual ~OneDLinearMathFunctionData() {};

		void printContent(ostream& out) { out << "blupdiblupp"; };

		double getValue(double x) {
			double ret = slope*x + intercept;
			if (ret <=0)
				return 0;
			else
				return ret;
		};

		double getValue(vector<double>& pos, double t) { return getValue(pos[0]); };

		void setSlope(double s) {this->slope = s;};

		void setYIntercept(double i) {this->intercept = i;};

	private:
		double slope;
		double intercept;
	};

	/**
	 * @brief one dimensional exponential function
	 *
	 */
	class OneDExponentialMathFunctionData : public MathFunctionData {
	public:
		OneDExponentialMathFunctionData(string dataName):MathFunctionData(dataName) {};
		virtual ~OneDExponentialMathFunctionData() {};

		void printContent(ostream& out) { out << "blubbeldiblup" << endl; };

		double getValue(double x) { return offset*exp(parameter*x); };

		double getValue(vector<double>& pos, double t) { return getValue(pos[0]); };

		void setOffset(double o) { this->offset = o; };

		void setParameter(double p) { this->parameter = p; };

	private:
		double offset;
		double parameter;
	};

	/**
	 * @brief for modelling point sources named after carl friedrich gauss
	 *
	 */
	class CarlFriedrichMathFunctionData : public MathFunctionData {
	public:
		CarlFriedrichMathFunctionData(string dataName):MathFunctionData(dataName) { x0=0; y0=0; z0=0;};
		virtual ~CarlFriedrichMathFunctionData() { };

		void printContent(ostream& out) { out << "blubbeldiblubbeldiblup "  << endl; };

		double getValue(vector<double>& pos, double t) {
			double fraction = initial/(4.*M_PI*diff*t);
			double rsquared = (pos[0]-x0)*(pos[0]-x0)+(pos[1]-y0)*(pos[1]-y0)+(pos[2]-z0)*(pos[2]-z0);

			return fraction*exp(-rsquared/(4.*diff*t));
		};

		void setDiff(double d) { this->diff = d; };

		void setInitialNumber(double i) { this->initial = i; };

		void setPosition(double x, double y, double z) { this->x0=x; this->y0=y; this->z0=z; };

	private:
		double diff;
		double initial;
		double x0, y0, z0;
	};




	class GaussGlockeMathFunctionData : public MathFunctionData {
	public:
		GaussGlockeMathFunctionData(string dataName):MathFunctionData(dataName) {x0=0; y0=0; z0=0; };
		virtual ~GaussGlockeMathFunctionData() { } ;
		void printContent(ostream& out) { out << "och noe, darauf habe ich nun wirklich keine lust" << endl; } ;
		double getValue(vector<double>& pos, double t) {
			double rsquared = (pos[0]-x0)*(pos[0]-x0)+(pos[1]-y0)*(pos[1]-y0)+(pos[2]-z0)*(pos[2]-z0);
			return a*exp(-rsquared/(2*sigma*sigma));
		}
		void setSigma(double s) { sigma = s;};
		void setAmplitude(double a) { this->a = a; };
		void setPosition(double x, double y, double z) { this->x0 = x, this->y0 = y; this->z0 = z;};
	private:
		double x0, y0, z0;
		double a, sigma;
	};

	/// environment consisting of two gauss peaks
	class DoubleGaussFunctionData : public MathFunctionData {
	public:
		DoubleGaussFunctionData(string dataName):MathFunctionData(dataName){};
		virtual ~DoubleGaussFunctionData() {};
		void printContent(ostream& out) { out << "noeonoe" << endl;};
		double getValue(vector<double>& pos, double t) {
			double rsquared0 = (pos[0]-x0)*(pos[0]-x0)+(pos[1]-y0)*(pos[1]-y0)+(pos[2]-z0)*(pos[2]-z0);
			double rsquared1 = (pos[0]-x1)*(pos[0]-x1)+(pos[1]-y1)*(pos[1]-y1)+(pos[2]-z1)*(pos[2]-z1);
			return a0*exp(-rsquared0/(2*sigma0*sigma0))+a1*exp(-rsquared1/(2*sigma1*sigma1));
		}
		void setSigma0(double s) { sigma0 = s;};
		void setSigma1(double s) { sigma1 = s;};
		void setAmplitude0(double a) { this->a0 = a; } ;
		void setAmplitude1(double a) { this->a1 = a; } ;
		void setPosition0(double x, double y, double z) { x0=x; y0=y; z0=z;};
		void setPosition1(double x, double y, double z) { x1=x; y1=y; z1=z;};

	private:
		double x0, y0, z0, x1, y1, z1;
		double a0, a1, sigma0, sigma1;

	};

	/**
	 * @brief we also include yuhai's sine function ...  fol valying ligand concentlations.
	 */
	class YuhaisSineFunctionData : public MathFunctionData {
	public:
		YuhaisSineFunctionData(string dataName):MathFunctionData(dataName) {};
		virtual ~YuhaisSineFunctionData() {};

		void printContent(ostream& out) { out << "blubbeldibblubbelblubbelblup" << endl; };

		double getValue(double x, double t) { return slope*(sin(omega*t)*x+halfconc); };

		double getValue(vector<double>& pos, double t) { return getValue(pos[0], t); };

		void setHalfConcentration(double h) { this->halfconc = h; };

		void setOmega(double omega) { this->omega = omega;};

		void setSlope(double s) { this->slope = slope; } ;

	private:
		double halfconc;
		double omega;
		double slope;
	};




	/**
	 * @brief for "modeling" step increases and decreases of a chemical
	 *
	 */
	class StepFunction : public MathFunctionData {
		public:
			StepFunction(string dataName):MathFunctionData(dataName) {
				currentStep = -1;
				nextTime = -1;
			};
			virtual ~StepFunction() { };

			void printContent(ostream& out) { out << "blubbeldiblubbeldiblup 2 "  << endl; };

			double getValue(vector<double>& pos, double t) {
				// return zero if we have no steps specified
				if(steps.size()==0) return 0;

				//If we are not at the next time, then return the current step
				if(t<nextTime)
					if(currentStep-1<0) return 0;
					else return steps.at(currentStep-1);

				// otherwise, we have to get to the next time
				do
				{
					// if there is no more steps, just return the last element, and
					// set next time to a large number
					if(currentStep+1>=times.size()) {
						currentStep++;
						nextTime = 9e999;
						return steps.at(steps.size()-1);
					}
					currentStep++;

				} while(t>=times.at(currentStep));

				nextTime = times.at(currentStep);
				if(currentStep-1<0) return 0;
				return steps.at(currentStep-1);
			};

			/**
			 * to set the steps in the simulation, pass two vectors:
			 *   steps has the list of steps in terms of chemical concentration,
			 *   and the corresponding vector times has a list of times that the
			 *   step should be taken at.  You should always have the first element
			 *   of "times" be 0 or less than zero, so that you can set the initial
			 *   concentration.  Otherwise, zero concentration is chosen until the
			 *   first step is reached.  The time of the steps must be in ascending order,
			 *   or else the behavior of this class is not well defined.
			 */
			void setSteps(vector <double> &steps, vector <int> & times) {

				if(steps.size()!=times.size()) {
					throw(Hive::HiveException("steps and time vector must be the same length.",
							"void StepFunction::setSteps(vector <double> &steps, vector <int> & times)"));
				}

				//copy the data over to our private vectors
				for(unsigned int i=0; i<steps.size(); i++)
					this->steps.push_back(steps.at(i));
				for(unsigned int i=0; i<times.size(); i++)
					this->times.push_back(times.at(i));
			};

		private:

			int currentStep;
			vector <double> steps;
			vector <int> times;
			double nextTime;
	};









}

#endif /* MATHFUNCTIONDATA_HH_ */
