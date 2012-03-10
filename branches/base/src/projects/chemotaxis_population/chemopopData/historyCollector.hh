/*
 * historyCollector.hh
 *
 *  Created on: Dec 14, 2011
 *      Author: msneddon
 */

#ifndef HISTORYCOLLECTOR_HH_
#define HISTORYCOLLECTOR_HH_


#include "../../../core/agent/data/data.hh"
#include <iostream>
#include <fstream>
#include <queue>


using namespace std;

namespace Hive
{

	class HistoryAggregator;


	/*
	 * This class is simply a data structure that acts as a stack
	 * with a fixed size, and when you place a new element on top of the stack, it removes
	 * the bottom element of the stack if the size is exceeded.  Then, at any time,
	 * you have the functionality of printing out the stack.  This class is used as a
	 * history collector which allows you to place double valued data at each time point
	 * on the stack, which is remembered for the length of the stack, which can then
	 * be retrieved at any time for output.  Thus, if some event happens (e.g. a tumble)
	 * you can retrieve exactly what the values were up to the given point.
	 *
	 */
	class HistoryCollector : public Data
	{
		public:

			// this is a friend class so that HistoryAggregator can operate efficiently on this object
			friend class HistoryAggregator;

			/*
			 * Constructor which indicates how many elements can be placed on the stack
			 */
			HistoryCollector(string name, int length):Data(name, "history_collector") {
				this->history=new double [length];
				this->memoryLength = length;
				this->currentTime = 0;
				this->hasCompleteHistoryFlag = false;
				for(int i=0; i<memoryLength; i++) {
					history[i]=0;
				}
			}

			/// destructor
			~HistoryCollector() {
				delete [] history;
			};
			void printContent(ostream &out) {
				out<<"HISTORY COLLECTOR ("<<this->dataName<<") ============= \n";
				this->retellHistoryInReverseCronologicalOrder(out);
			};
			void packDataForMPI(char*& buffer,int& size) { cerr<<"\n\nI do not know how to pack HistoryCollector yet"<<endl; exit(1);};
			void unpackDataForMPI(char* buffer,int size) { cerr<<"\n\nI do not know how to unpack HistoryCollector yet"<<endl; exit(1);};

			/*
			 * Call this function to add another element (of value) to the top of the stack
			 */
			void makeHistory(double value) {
				history[currentTime]=value;
				currentTime++;
				if(currentTime>memoryLength) {
					currentTime = 0; hasCompleteHistoryFlag = true;
				}
			};

			/**
			 * returns true if the stack is full (and a complete history exists), false otherwise
			 */
			bool hasCompleteHistory() const { return hasCompleteHistoryFlag; };

			/**
			 * output the history of this cell in cronological (bottom of the stack -> top of the stack)
			 */
			void retellHistoryInCronologicalOrder(ostream &out) {
				out<<"here"<<currentTime<<" "<<memoryLength<<endl;
				for(int i=currentTime; i<memoryLength; i++) {
					out<<history[i]<<" ";   }
				for (int i=0; i<currentTime; i++) {
					out<<history[i]<<" ";  }
				out<<endl;
			};

			/**
			 * output history of this cell in reverse cronological order (top of stack -> bottom)
			 */
			void retellHistoryInReverseCronologicalOrder(ostream &out) {
				// loop back in time until we get to zero
				for(int i=currentTime-1; i>=0; i--) {
					out<<history[i]<<" ";  }
				// loop back in time until we get to zero
				for(int i=memoryLength-1; i>=currentTime; i--) {
					out<<history[i]<<" ";  }
				out<<endl;
			};

			/**
			 * returns how many elements of history can be remembered
			 */
			int getMemoryLength() const { return memoryLength; };

		protected:
			double *history;
			int memoryLength;
			int currentTime;
			bool hasCompleteHistoryFlag;

	};


	// compare class which allows the priority_queue in HistoryAggregator to
	// sort times from earliest to latest (default double compare returns highest values first)
	class CompareDoubleReverse {
		public:
			bool operator ()(double &d1, double &d2) {
				return d1>d2;
			};
	};

	/**
	 * This class takes a running average and running standard deviation of a particular HistoryCollector.  Whenever
	 * an event is triggered, you can call the aggregateNow function, and the current stack will be added to this
	 * aggregator.  There is also a start and end time so that events can be selectively aggregated.
	 */
	class HistoryAggregator : public Data
	{
		public:
			HistoryAggregator(string name, HistoryCollector *hc, double startTime, double endTime):Data(name, "history_aggregator") {
				this->hc = hc;
				this->Mk=new double [this->hc->getMemoryLength()];
				for(int j=0; j<hc->memoryLength; j++)  Mk[j]=0;
				this->Qk=new double [this->hc->getMemoryLength()];
				for(int j=0; j<hc->memoryLength; j++)  Qk[j]=0;
				this->k_records = 0;
				this->Mk_minus_one = 0;
				this->Qk_minus_one = 0;
				this->startTime = startTime;
				this->endTime = endTime;
			}

			/// destructor
			~HistoryAggregator() {
				delete [] Mk;
				delete [] Qk;
			};

			void printContent(ostream &out) {
				out<<"HISTORY AGGREGATOR ("<<this->dataName<<", has k="<<k_records<<" recordings) ============= \n";
				this->printMean(out);
				this->printStdDev(out);
			};
			void packDataForMPI(char*& buffer,int& size) { cerr<<"\n\nI do not know how to pack HistoryCollector yet"<<endl; exit(1);};
			void unpackDataForMPI(char* buffer,int size) { cerr<<"\n\nI do not know how to unpack HistoryCollector yet"<<endl; exit(1);};

			void aggregateNow(double currentTime) {
				if(currentTime>=startTime && currentTime<=endTime && hc->hasCompleteHistory()) {
					if(k_records>0) {
						int j=0; k_records++;
						for(int i=hc->currentTime; i<hc->memoryLength; i++,j++) {
							  Mk_minus_one = Mk[j];  Qk_minus_one = Qk[j];
							  Mk[j] = Mk_minus_one + (hc->history[i]-Mk_minus_one) / (k_records);
							  Qk[j] = Qk_minus_one + ((k_records-1) * (hc->history[i]-Mk_minus_one) * (hc->history[i]-Mk_minus_one)) / (k_records);
						}
						for (int i=0; i<hc->currentTime; i++,j++) {
							  Mk_minus_one = Mk[j];  Qk_minus_one = Qk[j];
							  Mk[j] = Mk_minus_one + (hc->history[i]-Mk_minus_one) / (k_records);
							  Qk[j] = Qk_minus_one + ((k_records-1) * (hc->history[i]-Mk_minus_one) * (hc->history[i]-Mk_minus_one)) / (k_records);
						}

					} else {
						int j=0; k_records++;
						for(int i=hc->currentTime; i<hc->memoryLength; i++,j++) {
							Mk[j] = hc->history[i];
							Qk[j] = 0;
						}
						for (int i=0; i<hc->currentTime; i++,j++) {
							Mk_minus_one = Mk[j];  Qk_minus_one = Qk[j];
							Mk[j] = Mk_minus_one + (hc->history[i]-Mk_minus_one) / (k_records);
							Qk[j] = Qk_minus_one + ((k_records-1) * (hc->history[i]-Mk_minus_one) * (hc->history[i]-Mk_minus_one)) / (k_records);
						}
					}
				}
			};


			void checkScheduledAggregations(double currentTime) {
				if(aggregationTimes.empty()) return;
				while(aggregationTimes.top()<=currentTime) {
					aggregateNow(aggregationTimes.top());
					aggregationTimes.pop();
					if(aggregationTimes.empty()) return;
				}
			};

			void scheduleAggregation(double aggTime) {
				aggregationTimes.push(aggTime);
			};

			void printMean(ostream &out) {
				for(int j=0; j<hc->memoryLength; j++) {
					out<<Mk[j]<<" ";
				} out<<endl;
			};
			void printStdDev(ostream &out) {
				for(int j=0; j<hc->memoryLength; j++) {
					out<<Qk[j]/(k_records-1)<<" ";
				} out<<endl;
			};
			void outputMean(ofstream *out, bool isBinary) {
				if(isBinary) {
					double d;
					for(int j=0; j<hc->memoryLength; j++) {
						d = Mk[j];
						out->write(reinterpret_cast<char *> (&d), sizeof(double));
					}
				} else {
					for(int j=0; j<hc->memoryLength; j++) {
						*out<<Mk[j]<<" ";
					}
				}
			};
			void outputStdDev(ofstream *out, bool isBinary) {
				if(isBinary) {
					double d;
					for(int j=0; j<hc->memoryLength; j++) {
						d = Qk[j]/(k_records-1);
						out->write(reinterpret_cast<char *> (&d), sizeof(double));
					}
				} else {
					for(int j=0; j<hc->memoryLength; j++) {
						*out<<Qk[j]/(k_records-1)<<" ";
					}
				}
			};
			int getKrecords() const { return k_records; };


		protected:
			// one pass mean and variance calculations
			double *Mk;
			double *Qk;
			double Mk_minus_one;
			double Qk_minus_one;
			unsigned int k_records; // number of records

			HistoryCollector *hc;

			double startTime;
			double endTime;


			priority_queue <double, vector<double>, CompareDoubleReverse > aggregationTimes;

	};






}









#endif /* HISTORYCOLLECTOR_HH_ */
