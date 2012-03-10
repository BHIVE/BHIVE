/**
 * @file XmlParser.cpp
 *
 * This is a simple parsing interface for parsing xml into a simulation setup. 
 *
 * @author Scott Devoid 
 *
 * @date April 15, 2010 
 *
 */

#include "XmlParser.hh"
#include "../../util/xml/TinyXML/tinyxml.h"


using namespace Hive;


XmlParser::XmlParser() {};
XmlParser::~XmlParser() {};

void XmlParser::addTagParser(string tagName, AgentFactory* factory)
{
    pair<string, AgentFactory*> newTagParser = pair<string, AgentFactory*>(tagName, factory);
    this->tagParsers.insert(newTagParser);
    this->childConfigTag = "childConfig";
};

void XmlParser::parseFile(const char* filename, Agent*& maestro, 
    vector<Agent* >& orchestra)
{
	TiXmlDocument document(filename);
	bool loadSucess = document.LoadFile();
	if(!loadSucess) {
		cerr << "Error in loading XML file: " << 
            filename << " into composer." << endl;
		exit(1);
	}
    this->walkTreeToAgents((TiXmlNode*) document.RootElement(), maestro, orchestra);
}

void XmlParser::walkTreeToAgents(TiXmlNode* node, Agent*& maestro, vector<Agent*>& orchestra) {
    if (this->isAgentNode(node)) {
        Agent* a = this->parseAgentNode(node, orchestra);
        maestro = a;
    } else {
        for (TiXmlNode* child = node->FirstChild(); child; 
            child = child->NextSibling()) {
            this->walkTreeToAgents(child, maestro, orchestra);
        }
    }
}

Agent* XmlParser::parseAgentNode(TiXmlNode* node, vector<Agent*>& orchestra)
{
    // check if node exists
    if ( node == NULL ) { return (Agent*) NULL; }
    TiXmlElement* element = node->ToElement();
    if (element == NULL) { return (Agent*) NULL;}
    
    // Generate xmlParsedNode for this node and all non-Agent matching children.
    map<string, string> attr = this->getAttributes(element);
    if ( attr.count("agent") > 0 ) {
        string agentName = attr["agent"];
        if ( !(agentName.empty()) & this->tagParsers.count(agentName) > 0 ) {
            xmlParsedNode* currNode = new xmlParsedNode;
            vector<xmlParsedNode*> childNonAgents = this->getNonAgentChildren(node);
            currNode->attr = attr;
            currNode->value = "create";
            currNode->children = childNonAgents;
            // Call AgentFactory with xmlParsedNode struct...
            AgentFactory* nodeAgentFactory = this->tagParsers[agentName];
            Agent* parentAgent = nodeAgentFactory->createAgent(currNode);
            // Now get childAgents of this node 
            vector<TiXmlNode*> childAgentNodes = this->getAgentChildren(node);
            for (unsigned int i = 0; i < childAgentNodes.size(); i++) {
                Agent* childAgent = this->parseAgentNode(childAgentNodes[i], orchestra);
                xmlParsedNode* childNodeConfig = this->getChildConfig(childAgentNodes[i]);
                nodeAgentFactory->addChild(parentAgent, childAgent, childNodeConfig);
            }
            orchestra.push_back(parentAgent);
            return parentAgent;
        } else {
            cerr << "Element " << agentName << " invalid!" << endl;
            return (Agent *) NULL;
        }
    } else {
        cerr << "Node not valid agent node in parseAgentNode!" << endl;
        return NULL;
    }
};

vector<xmlParsedNode*> XmlParser::getNonAgentChildren(TiXmlNode* node)
{
    vector<xmlParsedNode*> topLevelNodes;
    for (TiXmlNode* child = node->FirstChild(); child; child = child->NextSibling()) {
        if (child != NULL && !(this->isAgentNode(child))) {
            topLevelNodes.push_back(this->toXmlParsedNode(child));
        }
    } 
    return topLevelNodes;
}


bool XmlParser::isAgentNode(TiXmlNode* node)
{
    if ( node == NULL ) { return false; }
    TiXmlElement* element = node->ToElement();
    if ( element == NULL ) { return false; }
    string value = element->Value();
    if ( !(value.empty()) && (value.compare("create") == 0)) {
        return true;
    } else { return false; }
}

xmlParsedNode* XmlParser::toXmlParsedNode(TiXmlNode* node) {
    if ( node == NULL ) { return NULL; }
    int nodeType = node->Type();
    xmlParsedNode* currNode = new xmlParsedNode;
    if (nodeType == node->ELEMENT || nodeType == node->TEXT || nodeType == node->DOCUMENT) {
        currNode->value = (string) node->Value();
        if (nodeType == node->ELEMENT) {
            currNode->type = "Element";
            TiXmlElement* currElement = node->ToElement();
            currNode->attr = this->getAttributes(currElement);
            for( TiXmlNode* child = node->FirstChild(); child; child = child->NextSibling()) {
                xmlParsedNode* childNode = this->toXmlParsedNode(child);
                currNode->children.push_back(childNode); 
            }
        } else if (nodeType == node->TEXT) {
            currNode->type = "Text";
        } else if (nodeType == node->DOCUMENT) {
            currNode->type = "Document";
        } else {
            currNode->type = "Unknown";
        }
    } else { return NULL; } 
    return currNode;
}

vector<TiXmlNode*> XmlParser::getAgentChildren(TiXmlNode* node)
{
    vector<TiXmlNode*> agentChildren;
    for (TiXmlNode* child = node->FirstChild(); child; child = child->NextSibling()) {
        string value = child->Value();
        if (child != NULL && !value.empty() &&    // if node has defined value
            this->tagParsers.count(value) > 0)  { // that matches a tag parser
            agentChildren.push_back(child);
        }
    }
    return agentChildren;
}

xmlParsedNode* XmlParser::getChildConfig(TiXmlNode* node)
{
    if ( node == NULL ) { return NULL; }
    for (TiXmlNode* child = node->FirstChild(); child; child = child->NextSibling()) {
        if (child != NULL && child->Value() != NULL  &&
           (strcmp(child->Value(), this->childConfigTag) == 0) ) {
            return this->toXmlParsedNode(child);
        }
    }    
    return NULL;
}


map<string, string> XmlParser::getAttributes(TiXmlElement* element)
{
    map<string, string> attributeMap;
    if (element == NULL) {
       cerr << "No element passed to getAttributes!" << endl;
       return attributeMap; 
    }
    TiXmlAttribute* currentAttribute = element->FirstAttribute();
    while (currentAttribute != NULL) {
        string name = (string) currentAttribute->Name(); 
        string value = (string) currentAttribute->Value(); 
        if (!name.empty() && !value.empty()) {
            attributeMap[name] = value;
        }
        currentAttribute = currentAttribute->Next(); 
    }
    return attributeMap;
};
