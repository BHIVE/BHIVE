/*
 *@file tdata.hh
 *
 * this file contains the interface to the template data classes
 *
 * @date Dec, 14, 2009 last edited: Dec 14, 2009 by garrit
 *
 * @author gjentsch
 */

#ifndef TDATA_HH_
#define TDATA_HH_

#include <string>
#include <vector>

#include "../data.hh"

using namespace std;

namespace Hive {
	/**
	 *  @brief template vector data class. we do not want to reimplemt a vector each time we need one with a different
	 *  template arguement
	 *
	 * @todo still a lot. this is just a preliminary implementation of a vector class. in the end one would like this
	 * class to have the full functionality of the std:vector class
	 *
	 *  @date Dec, 14th 2009 last edited: Dec 14th, 2009 by garrit
	 *
	 *  @author garrit
	 */
    template <class T> class TVectorData : public Data {
    public:
    	TVectorData(string name, string type):Data(name,type){ currentsize = 0; tvector.resize(0);};
    	TVectorData():Data("mr. x", "tvectordata"){ currentsize = 0; tvector.resize(0); };
    	~TVectorData() { tvector.clear(); };

    	/// returns element at index
    	T& at(int index) { return tvector[index]; };

    	/// prints content (needed from data)
    	void printContent(ostream &out) {
    		out<<"[DATA] type=\""<<datatype<<"\" name=\""<<dataName<<"\" size="<<tvector.size() << endl;
    	};

    	/// adds element to end
    	void addElementToEnd(T ele) { tvector.push_back(ele); currentsize += 1; }

    	/// removes element from end and reduces size of tvector
    	T removeElementFromEnd() { tvector.pop_back(); currentsize -= 1;};

    	/// clears the tvector such that its size is equal to zero after the call
    	void removeElements() { tvector.clear(); }

    	/// reserves size of tvector
    	void reserveSize(unsigned int num_ele) { this->tvector.resize(num_ele); currentsize = num_ele; };

    	/// set entry
    	void setEntry(int index, T value) { tvector[index] = value; };

    	/// return size of the vector
    	int size() { return currentsize; };

    	/// [] operator
    	T& operator[](int index) { return tvector[index]; };


    	void copyContentsInto(Data *destination) {
    		TVectorData <T> *dest = ((TVectorData <T> *)destination);
    		dest->tvector.clear();
    		dest->currentsize = 0;
    		for(unsigned int k=0; k<tvector.size(); k++)
    			dest->addElementToEnd(tvector.at(k));
    	}


    protected:
    	int currentsize;
    	vector<T> tvector;

    };

    /**
     * @brief partial specialisation of the tvectordata class for pointers. this way the container destroys the
     * objects that are stored by the vectordata.
     */

    template<class T> class TVectorData<T*> : public Data {
       public:
       	TVectorData(string name, string type):Data(name,type){};
       	TVectorData():Data("mr. x", "tvectordata"){};
       	~TVectorData() { for (unsigned int i=0; i<tvector.size(); i++) {
       		delete tvector[i]; tvector[i] = NULL;
       	}};

       	/// returns element at index
       	T*& at(int index) { return tvector[index]; };

       	/// prints content (needed from data)
       	void printContent(ostream &out) { };

       	/// adds element to end
       	void addElementToEnd(T* ele) { tvector.push_back(ele); currentsize += 1; }

       	/// removes element from end and reduces size of tvector
       	T* removeElementFromEnd() { tvector.pop_back(); currentsize -= 1;};

       	/// clears the tvector such that its size is equal to zero after the call
       	void removeElements() { tvector.clear(); }

       	/// reserves size of tvector
       	void reserveSize(unsigned int num_ele) { this->tvector.resize(num_ele); currentsize = num_ele; };

       	/// set entry
       	void setEntry(int index, T* value) { tvector[index] = value; };

       	/// return size of the vector
       	int size() { return currentsize; };

       	/// [] operator
       	T*& operator[](int index) { return tvector[index]; };

       protected:
       	int currentsize;
       	vector<T*> tvector;

       };



}

#endif /* TDATA_HH_ */
