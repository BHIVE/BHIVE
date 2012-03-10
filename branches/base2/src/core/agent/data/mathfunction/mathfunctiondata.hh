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
#include "../../../util/util.hh"

#include <cmath>
#include <iostream>
#include <vector>
#include <exception>

using namespace std;

namespace Hive {
	// base class does this work ... needs to be tested ...
	class MathFunctionData : public Data {
	public:
		MathFunctionData(string dataName):Data(dataName,"MathFunctionType") { t0 = 0;};
		virtual ~MathFunctionData() {};

		// this is already geared towards our purposes ...
		virtual double getValue(vector<double>& pos, double t)=0;

		virtual void printContent(ostream& out)=0;

		void setT0(double t) { this->t0 = 0; } ;

	protected:
		double t0;
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
	 * @brief allows step response in total background concentration
	 *
	 *
	 */
	class StepResponseMathFunction : public MathFunctionData
	{
		public:
			StepResponseMathFunction(string dataName):MathFunctionData(dataName) {
				initialBackground = 0;
				this->timeOfSteps.reserve(0);
				this->concentrations.reserve(0);
			};
			virtual ~StepResponseMathFunction() {};
			void printContent(ostream& out) { out << "blupdiblupp"; };

			//Assumes that time steps are in ascending order!!
			double getValue(vector<double>& pos, double t) {

				// Not very efficient!  but that should be ok, because this is
				// to test the dose response of a cell for testing, and will not
				// (should not) really be used in larger simulations.  The problem is
				// that just because you get a time that is in the future, I'm not sure
				// if it is guaranteed that you won't see a past time.

				int k = -1;
				for(;(k+1)<(int)timeOfSteps.size();k++) {
					if(timeOfSteps.at(k+1)>t) {
						break;
					}
				}
				if(k<0) return initialBackground;

				//cerr<<endl<<"time: "<<t<<" conc: " <<concentrations.at(k)<<"  k:"<<k<<endl;
				return concentrations.at(k);
			};




			double initialize(string &input) {
				cerr<<"#  initializing step Math function"<<endl;

				Util::trim(input);
				try {
					bool readingInitBackground = true; string initConc = "";
					bool readingTimeOfStep = false; string currentTime = "";
					bool readingConc = false; string currentConc = "";
					//first four characters are from "step", so they should be ignored
					for(unsigned int i=4; i<input.length(); i++)
					{
						// skip spaces
						if( input.at(i)==' ' || input.at(i)=='\t' ) { continue; }

						if(input.at(i)=='(') {
							readingInitBackground=false; readingTimeOfStep=true; readingConc=false;
							continue;
						} else if (input.at(i)==',') {
							// done reading time of step, so save the last value
							this->timeOfSteps.push_back(Util::convertToDouble(currentTime)); currentTime = "";
							readingInitBackground=false; readingTimeOfStep=false; readingConc=true;
							continue;
						} else if (input.at(i)==')') {
							this->concentrations.push_back(Util::convertToDouble(currentConc)); currentConc = "";
							readingInitBackground=false; readingTimeOfStep=false; readingConc=false;
							continue;
						}

						if(readingInitBackground) { initConc = initConc+input.at(i); }
						else if(readingTimeOfStep) { currentTime = currentTime+input.at(i); }
						else if(readingConc) { currentConc = currentConc+input.at(i); }
					}

					this->initialBackground = Util::convertToDouble(initConc);

					if(timeOfSteps.size()!=concentrations.size()) {
						throw(HiveException("error in parsing - the number of times and steps do not match","StepResponseMathFunction::initialize(string &input)"));
					}
					for(unsigned int i=1; i<timeOfSteps.size(); i++) {
						if(timeOfSteps.at(i)<timeOfSteps.at(i-1))
							throw(HiveException("error in parsing - the time of the steps must be in ascending order","StepResponseMathFunction::initialize(string &input)"));
					}

					//debugging output
					cerr<<"#      -- initial background concentration = "<<initialBackground<<endl;
					for(unsigned int i=0; i<timeOfSteps.size(); i++) {
						cerr<<"#      -- step at time: '"<<timeOfSteps.at(i)<<"' to concentration of: '"<<concentrations.at(i)<<"'"<<endl;
					}
				   cerr << "# done initialising step math function method" << endl;
				} catch (HiveException& e) {
					 e.addTraceException("StepResponseMathFunction::initialize(string &input)");
					 throw e;
				}
			}

			void setInitialConcentration(double initBack) { initialBackground = initBack; };
			double setSteps(vector <double> &times, vector <double> &concs) {
				if(times.size()!=concs.size()) {
					throw(HiveException("times and concs vectors must be the same size.",
							"StepResponseMathFunction::setSteps(vector <double> &times, vector <double> &concs)"));
				}
				for(unsigned int k=0; k<times.size(); k++) {
					timeOfSteps.push_back(times.at(k));
					concentrations.push_back(concentrations.at(k));
				}
			}


		private:

			double initialBackground;
			vector <double> timeOfSteps;
			vector <double> concentrations;


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
			double fraction = initial/(4.*M_PI*diff*(t-t0));
			double rsquared = (pos[0]-x0)*(pos[0]-x0)+(pos[1]-y0)*(pos[1]-y0)+(pos[2]-z0)*(pos[2]-z0);
			if (t>t0)
				return fraction*exp(-rsquared/(4.*diff*(t-t0)));
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

		double getValue(double x, double t) { return slope*(sin(omega*(t-t0))*x+halfconc); };

		double getValue(vector<double>& pos, double t) { return getValue(pos[0], t); };

		void setHalfConcentration(double h) { this->halfconc = h; };

		void setOmega(double omega) { this->omega = omega;};

		void setSlope(double s) { this->slope = slope; } ;

	private:
		double halfconc;
		double omega;
		double slope;
	};








}

#endif /* MATHFUNCTIONDATA_HH_ */
