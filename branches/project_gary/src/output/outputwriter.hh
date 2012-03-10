/**
 * @file outputwriter.hh
 *
 * this file contains the interface for the outputwriter.
 *
 * @author: Garrit Jentsch
 *
 * @date Oct 13, 2009 last edited Oct 13, 2009 by Garrit
 *
 */

#ifndef OUTPUTWRITER_HH_
#define OUTPUTWRITER_HH_

#include "../hive.hh"

using namespace std;

namespace Hive {
	/**
	 * @brief abstract class outputwriter
	 * an outputwriter is responsible for storing data on disk
	 * @author Garrit Jentsch, Christopher Henry
	 * @date Oct 13, 2009 last edited: June 10, 2010
	 */
	class Database;

	struct PrintDataNode {
		map<int,PrintDataNode*> children;
		vector<double> lastPrintData;
	};

	struct ObservableSpecs {
		string name;//e.g. Concentration
		int fileIndex;//Index of the filestream where data should be printed
		int index;//
		double threshold;
		bool fractionThreshold;
		PrintDataNode* root;
		vector<string> ids;
	};
	
	//This function recursively deletes the tree formed out of PrintDataNodes
	void deletePrintDataNode(PrintDataNode* inNode);

	class OutputWriter {
	public:
		//In the constructor, you supply the maximum number of CPU that should print in any given round
		//This is how we avoid overwhelming the IO node by having every processor print every round
		OutputWriter(int maxPrintingCPU,int rank,int size);

		//In the destructor, we need to close all the open file streams and delete all the id trees and observable specs
		~OutputWriter();

		//This function adds a new observable to the output system and returns an ID for the observeble
		//This ID should be used to print data to this observable in the future
		int addObservable(string Name,string filename,vector<string> ids,double threshold,bool fractionThreshold);
		
		//This function updates the value of an observable, and if the updated value satisfies the threshold criteria, it will be added to the buffer for printing
		//The index is the id of the observablespec for the input data, the ids vector is the vector of ids associated with the data, and the data vector is the vector of data itself
		void updateObservable(int index,vector<int> ids,vector<double> data);

		//This function prints all the data currently stored in the data buffers
		void printBuffers(bool forcePrint = false);

		int getObservableIndex(string name) {return observableMap[name];};
	private:
		//This is a count that iterates each propagation cycle to track when the outputwriter should print to files
		int count;
		//This indicates how many rounds should pass before the data buffers should be printed to the files
		int printRound;
		//This structure is a buffer for the data to be printed which is used to avoid having all processors printing at once
		vector<vector<double> > dataBuffer;
		//This is a buffer of the observable specs associated with the data to be printed in the databuffer vector
		vector<ObservableSpecs*> obsBuffer;
		//This is a buffer for the ids associated with the data to be printed in the data buffer
		vector<vector<int> > idBuffer;
		//This is a buffer for the time associated with the data to be printed in the data buffer
		vector<double> timeBuffer;
		//This vector holds all the observables handled by the output writer
		vector<ObservableSpecs*> observables;
		//This is a vector of open files where all data will be printed
		vector<ofstream*> openFiles;
		vector<string> filenames;
		//This is a map of the filename to the index of the file stream in the openFiles vector
		map<string,int> fileMap;
		//This is a map of the observable name to the index of the observable in the observables vector
		map<string,int> observableMap;
	};
}

#endif /* OUTPUTWRITER_HH_ */
