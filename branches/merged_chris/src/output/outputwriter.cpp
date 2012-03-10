/**
 * @file outputwriter.cpp
 * this file contains the interface for the outputwriter.
 * @author: Garrit Jentsch,Chritopher Henry
 * @date Oct 13, 2009 last edited June 10, 2010 by Chris
 */

#include "outputwriter.hh"

using namespace std;
using namespace Hive;

//This function recursively deletes the tree formed out of PrintDataNodes
void Hive::deletePrintDataNode(PrintDataNode* inNode) {
	if (inNode != NULL) {
		for (map<int,PrintDataNode*>::iterator IT = inNode->children.begin(); IT != inNode->children.end(); IT++) {
			deletePrintDataNode(IT->second);
		}
		delete inNode;
	}
};

//In the constructor, you supply the maximum number of CPU that should print in any given round
//This is how we avoid overwhelming the IO node by having every processor print every round
OutputWriter::OutputWriter(int maxPrintingCPU,int rank,int size) {
	printRound = int((size-1)/maxPrintingCPU);
	if (maxPrintingCPU == 1) {
		count = rank;
	} else {
		count = rank % maxPrintingCPU;
	}
};

//In the destructor, we need to close all the open file streams and delete all the id trees and observable specs
OutputWriter::~OutputWriter() {
	//Closing all open file streams
	for (int i=0; i < int(openFiles.size()); i++) {
		openFiles[i]->close();
		delete openFiles[i];
	}
	//Deleting the id trees and the observables
	for (int i=0; i < int(observables.size()); i++) {
		deletePrintDataNode(observables[i]->root);
		delete observables[i];
	}
};

//This function adds a new observable to the output system and returns an ID for the observeble
//This ID should be used to print data to this observable in the future
int OutputWriter::addObservable(string name,string filename,vector<string> ids,double threshold,bool fractionThreshold) {
	//Checking if an observable of the same name has already been added
	if (observableMap.count(name) > 0) {
		return observableMap[name];
	}
	//Creating and initializing a new observable
	ObservableSpecs* newobs = new ObservableSpecs;
	observableMap[name] = observables.size();
	newobs->index = observables.size();
	newobs->root = new PrintDataNode;
	newobs->threshold = threshold;
	newobs->fractionThreshold = fractionThreshold;
	observables.push_back(newobs);
	newobs->name = name;
	newobs->ids = ids;
	//Checking if the input file is new, and if so, creating and opening the ofstream
	if (fileMap.count(filename) == 0) {
		fileMap[filename] = openFiles.size();
		filenames.push_back(filename);
		ofstream* newOutput = new ofstream();
		openFiles.push_back(newOutput);
	}
	//Setting the file index in the observable
	newobs->fileIndex = fileMap[filename];
	return newobs->index;
}

//This function updates the value of an observable, and if the updated value satisfies the threshold criteria, it will be added to the buffer for printing
//The index is the id of the observablespec for the input data, the ids vector is the vector of ids associated with the data, and the data vector is the vector of data itself
void OutputWriter::updateObservable(int index,vector<int> ids,vector<double> data) {
	if (int(observables.size()) > index && observables[index] != NULL) {
		//Getting the specs for the obervable out of the storage vector
		ObservableSpecs* current = observables[index];
		//Scanning through the data tree to find the last printed value for the observable
		PrintDataNode* currNode = current->root;
		for (int i=0; i < int(ids.size()); i++) {
			if (currNode->children[ids[i]] == NULL) {
				currNode->children[ids[i]] = new PrintDataNode;
			}
			currNode = currNode->children[ids[i]];
		}
		//Checking if the updated value satisfies the threshold
		bool print = false;
		if (currNode->lastPrintData.size() == 0) {
			print = true;
		} else {
			for (unsigned int i=0; i < currNode->lastPrintData.size(); i++) {
				if (!current->fractionThreshold && fabs(currNode->lastPrintData[i]-data[i]) > current->threshold) {
					print = true;
					break;
				} else if (current->fractionThreshold) {
					double denominator = currNode->lastPrintData[i];
					if (currNode->lastPrintData[i] == 0) {
						denominator = data[i];
					}
					if (denominator != 0 && fabs(currNode->lastPrintData[i]-data[i])/denominator > current->threshold) {
						print = true;
						break;
					}
				}	
			}
		}
		//If the updated value does satisfy the threshold, then the data is added to the print buffer
		if (print) {
			currNode->lastPrintData = data;
			idBuffer.push_back(ids);
			obsBuffer.push_back(current);
			dataBuffer.push_back(data);
			timeBuffer.push_back(get_propagator()->getCurrentTime());
		}
	}
};

//This function prints all the data currently stored in the data buffers
void OutputWriter::printBuffers(bool forcePrint) {
	if (count == printRound || forcePrint) {
		count = 0;
		if (dataBuffer.size() > 0) {
			//Print all data stored in the data buffer to the appropriate file
			for (unsigned int i=0; i < dataBuffer.size(); i++) {
				//Checking to make sure the file is open
				if (!openFiles[obsBuffer[i]->fileIndex]->is_open()) {
					openFiles[obsBuffer[i]->fileIndex]->open(filenames[obsBuffer[i]->fileIndex].data());
					//Printing the file header
					(*openFiles[obsBuffer[i]->fileIndex]) << "Time\t";
					for (unsigned int j=0; j < obsBuffer[i]->ids.size(); j++) {
						(*openFiles[obsBuffer[i]->fileIndex]) << obsBuffer[i]->ids[j] << "\t";;
					}
					(*openFiles[obsBuffer[i]->fileIndex]) << obsBuffer[i]->name;
					//Printing the endline but not flushing the buffer to file yet
					(*openFiles[obsBuffer[i]->fileIndex]) << "\n";
				}
				//Printing the time
				(*openFiles[obsBuffer[i]->fileIndex]) << timeBuffer[i] << "\t";
				//Printing the IDs
				for (unsigned int j=0; j < idBuffer[i].size(); j++) {
					(*openFiles[obsBuffer[i]->fileIndex]) << idBuffer[i][j] << "\t";
				}
				//Printing the data
				for (unsigned int j=0; j < dataBuffer[i].size(); j++) {
					(*openFiles[obsBuffer[i]->fileIndex]) << dataBuffer[i][j];
					if (j < (dataBuffer[i].size()-1)) {
						(*openFiles[obsBuffer[i]->fileIndex]) << "\t";
					}
				}
				//Flushing the ofstream buffer to file and printing the endline
				(*openFiles[obsBuffer[i]->fileIndex]) << endl;
			}
			//Clearing out the buffer to ensure the same data is not printed twice
			dataBuffer.clear();
			obsBuffer.clear();
			idBuffer.clear();
			timeBuffer.clear();
		}
	}
	count++;
};

