/**
 * @file xmlExpander.hh
 *
 * This is a simple parsing interface for parsing xml into a simulation setup. 
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
	 * @brief xmlParser class
	 *
     * 
     *
	 */
   
   struct varState{
        int depth;
        string value;
    };

	class XmlExpander {
      public:
		/// Constructor
		XmlExpander();
		/// destructor
		virtual ~XmlExpander();
        /// main function
        void expandFile(const char* inputFilename, const char* outputFilename);
        void config(string tagName, string tagValue) { this->config_[tagName] = tagValue; };
        string config(string tagName); 
        
      protected:
        void generateNodeLevelDocument(TiXmlDocument inputDocument, TiXmlDocument outputDocument);
        TiXmlNode* expandNode(TiXmlNode* inputNode);
        TiXmlNode* interpretNode(TiXmlNode* inputNode);
        string getVariable(string varName);
        void setVariable(string varName, string varValue);
        list<string> parseDistributionCommand(string distribution);
        void popVariableState();
        void pushVariableState();
        int currDepth;
        map<string, string> config_; 
        map<string, stack<varState*> > variables_; 
	};
}
#endif /* XMLEXPANDER_HH_ */
