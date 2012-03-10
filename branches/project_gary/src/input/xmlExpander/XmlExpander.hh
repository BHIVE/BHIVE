/**
 * @file xmlExpander.hh
 *
 * XmlExpander reads an xml file and expands several statements into
 * larger xml expressions. This facilitates parameter scans and setting
 * up large simulations using small, readable xml files.
 *
 * @author Scott Devoid 
 *
 * @date June 10, 2010 
 *
 */

#ifndef XMLEXPANDER_HH_
#define XMLEXPANDER_HH_

#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <iostream>
#include "../../util/rand/rand.hh"
#include "../../agent/agent.hh"
#include "../../agent/agentFactory/agent_factory.hh"
#include "../../util/xml/TinyXML/tinyxml.h"

using namespace std;

namespace Hive {
	/**
	 * @brief xmlExpander class
	 *
     * 
     *
	 */
   
   struct varState{
        int depth;
        string value;
		double max;		// for intervals
		double step_size;  // for intervals
    };

	class XmlExpander {
      public:
		/// Constructor
		XmlExpander();
		/// destructor
		virtual ~XmlExpander();
        /// main function
        void expandFile(string inputFilename, string outputFilename);
        // configure xml keywords
		void config(string tagName, string tagValue) { this->config_[tagName] = tagValue; };
		string config(string tagName); 
        
      protected:
        TiXmlDocument* generateNodeLevelDocument(TiXmlDocument* inputDocument);
        vector<TiXmlNode*> expandNode(TiXmlNode* inputNode);
        TiXmlNode* interpretNode(TiXmlNode* inputNode);
        string getVariable(string varName);
        void setVariable(string varName, string varValue);
		void setVariable(string varName, string varValue, double step_size, double max);
        list<string> parseDistributionCommand(string distribution);
        void popVariableState();
        void pushVariableState();
        int currDepth;
        map<string, string> config_; 
        map<string, stack<varState*> > variables_;
	};
}
#endif /* XMLEXPANDER_HH_ */
