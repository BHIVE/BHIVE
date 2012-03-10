/**
 * @file xmlParser.hh
 *
 * This is a simple parsing interface for parsing xml into a simulation setup. 
 *
 * @author Scott Devoid 
 *
 * @date April 15, 2010 
 *
 */

#ifndef XMLPARSER_HH_
#define XMLPARSER_HH_

#include <string>
#include <vector>
#include <map>
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
   class AgentFactory;
   
   struct xmlParsedNode{
        string type; 
        string value;
        map<string, string> attr;
        vector<struct xmlParsedNode *> children;
    };

	class XmlParser {
      public:
		/// Constructor
		XmlParser();
		/// destructor
		virtual ~XmlParser();
        
        /// main function
        void addTagParser(string tagName, AgentFactory* factory);
        void parseFile(const char* filename, Agent*& maestro, vector<Agent *>& orchestra);
        
        
        void setChildConfigTag(const char* tag) { this->childConfigTag = tag; }
        

      protected:
        void walkTreeToAgents(TiXmlNode* node, Agent*& maestro, vector<Agent *>& orchestra); 
        Agent* parseAgentNode(TiXmlNode* node, vector<Agent *>& orchestra);
        vector<xmlParsedNode*> getNonAgentChildren(TiXmlNode* node);
        bool isAgentNode(TiXmlNode* node);
        xmlParsedNode* toXmlParsedNode(TiXmlNode* node);
        vector<TiXmlNode*> getAgentChildren(TiXmlNode* node);
        xmlParsedNode* getChildConfig(TiXmlNode* node);
        
        map<string, string> getAttributes(TiXmlElement* element);
        map<string, AgentFactory*> tagParsers;
        const char* childConfigTag;

	};
}
#endif /* XMLPARSER_HH_ */
