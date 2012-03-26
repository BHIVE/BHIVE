/**
 * @file XmlExpander.cpp
 *
 * This is a simple parsing interface for parsing xml into a simulation setup. 
 *
 * @author Scott Devoid 
 *
 * @date June 10, 2010 
 *
 */

#include "XmlExpander.hh"

using namespace Hive;


XmlExpander::XmlExpander() {};
XmlExpander::~XmlExpander()
{
    for (map<string, stack<varState*> >::iterator it = this->variables_.begin();
        it != this->variables_.end(); it++) {
        stack<varState*> varStack = (*it).second;
        varState* state;
        while(! varStack.empty()) {
            state = varStack.top();     // perhaps this 
            delete state;               // is not needed?
            varStack.pop();
        }
    }
    this->variables_.clear();
    this->config_.clear();
}

void XmlExpander::expandFile(const char* inputFilename, const char* outputFilename) {
	TiXmlDocument inputDocument(inputFilename);
	bool loadSucess = inputDocument.LoadFile();
	if(!loadSucess) {
		cerr << "Error in loading XML file: " << 
            inputFilename << " into composer." << endl;
		exit(1);
	}
    TiXmlDocument outputDocument(outputFilename);
    this->generateNodeLevelDocument(inputDocument, outputDocument);
    bool saveSucess = outputDocument.SaveFile();
    if (!saveSucess) {
        cerr << "Error in saving XML file: " <<
            outputFilename << " in composer." << endl;
    }
}

void XmlExpander::generateNodeLevelDocument(TiXmlDocument inputDocument, TiXmlDocument outputDocument)
{
    TiXmlNode* inputNode = inputDocument.ToElement();
    TiXmlNode* outputNode = outputDocument.ToElement();
    outputNode = this->expandNode(inputNode);
}

void XmlExpander::setVariable(string name, string value)
{
    varState* currState = new varState;
    currState->depth = this->currDepth;
    currState->value = value;
    if(this->variables_.count(name) > 0) {
        this->variables_[name].push(currState);
    } else {
        stack<varState*> stateStack;
        stateStack.push(currState);
        this->variables_[name] = stateStack;
    }
}
  
list<string> XmlExpander::parseDistributionCommand(string distribution)
{
    list<string> output;
    char * token = strtok((char *)distribution.c_str(), "(,)");
    int counter = 0;
    while (token != NULL) {
        if (counter != 0 && !getVariable(string(token)).empty()) {
            output.push_back(getVariable(string(token)));
        } else {
            output.push_back(string((const char *) token));
        }
        token = strtok(NULL, "(,)");
    }
    return output;
}
  

TiXmlNode* XmlExpander::expandNode(TiXmlNode* inputNode)
{
    TiXmlNode* outputNode;
    if (strcmp(inputNode->Value(), this->config(string("scan")).c_str()) == 0) {
        // do scan handling
        TiXmlElement* scanElement = inputNode->ToElement();
        if (scanElement) {
            string distribution, name, value;
            if((scanElement->QueryValueAttribute("distribution", 
                &distribution)) == TIXML_SUCCESS && 
                (scanElement->QueryValueAttribute("name", 
                &name) == TIXML_SUCCESS)) {
                // get distribution and name:
                list<string> command = this->parseDistributionCommand(distribution);
                string function = command.front();
                command.pop_front();
                if (function.compare("normal") == 0) {
                    double normal = Util::RANDOM_GAUSSIAN();
                    if ((int) command.size() == 2) {
                        double mean = atof(command.front().c_str());
                        command.pop_front();
                        double variance = atof(command.front().c_str());
                        normal = mean + variance * normal;
                    }
                    // convert double into string
                    ostringstream strs;
                    strs << normal;
                    value = strs.str();
                } else if (function.compare("continuous") == 0) {
                    if ((int) command.size() == 2) {
                       double min = atof(command.front().c_str());
                       command.pop_front();
                       double max = atof(command.front().c_str());
                       double rand = Util::RANDOM(1);
                       double result = ((max - min) * rand) + min;
                       ostringstream strs;
                       strs << result;
                       value = strs.str();
                    } else {
                        cerr << "(min,max) arguments required for continous function in <scan .../>" << endl;
                    }
                } else {
                    cerr << "Unknown distribution function in <scan ../> " <<
                        " Got '" << function << "'"<< endl;
                }
                // Finally, set the variable[name] = value
                setVariable(name, value);
            } else {
                cerr << "Warning, invalid use of <scan ../>" <<
                    " Must have attribute 'distribution'." << endl;
            }
        } else {
            cerr << "Error in parsing <scan ../> element." << endl;
        }
    } else if (strcmp(inputNode->Value(), this->config(string("set")).c_str()) == 0) {
        // do set handling
        TiXmlElement* setElement = inputNode->ToElement();
        if (setElement) {
            string name, value;
            if((setElement->QueryValueAttribute("name", &name) ==
                TIXML_SUCCESS) && (setElement->QueryValueAttribute("value",
                &value) == TIXML_SUCCESS)) {
                // now set variable
                this->setVariable(name, value);
            } else {
                cerr << "Warning, invalid use of <set .../>" <<
                    " Must have attributes 'name' and 'value'." << endl;
            }
        } else {
            cerr << "Error in parsing <set/> " << endl;         
        }
    } else if (string(inputNode->Value()).compare(this->config(string("loop"))) == 0) {
        // do loop handling
        TiXmlElement* loopElement = inputNode->ToElement();
        int count;
        if((loopElement->QueryIntAttribute("count", &count) == TIXML_SUCCESS)) {
            std::stringstream countString;
            countString << count;
            this->setVariable("__loop__", countString.str());
        } else {
            cerr << "Warning, invlaid use of <loop .../> " <<
                " Must have attribute 'count'." << endl;
        }
    } else {
        outputNode = this->interpretNode(inputNode);
    }
    // push state as we are looking at node's children now...
    this->pushVariableState();
    int loopMax = 1;
    if (! this->getVariable("__loop__").empty()) {
        // if loop was set, parse children __loop__ times
        loopMax = atoi(getVariable("__loop__").c_str());
    }
    for (int i=0; i < loopMax; i++) {
        // now parse children
        for (TiXmlNode* inputChild = inputNode->FirstChild(); 
            inputChild; inputChild = inputChild->NextSibling()) {
            TiXmlNode* outputChild = this->expandNode(inputChild); 
            if (outputChild != NULL) {
                outputNode->LinkEndChild(outputChild);
            }
        }
    }
    this->popVariableState();
    // done with node's children, pop state and return
    return outputNode;
}

TiXmlNode* XmlExpander::interpretNode(TiXmlNode* inputNode) {
    TiXmlNode* outputNode;
    if (inputNode->Type() == inputNode->ELEMENT) {
        TiXmlElement* outputElement = new TiXmlElement(inputNode->Value());
        TiXmlElement* inputElement = inputNode->ToElement();
        for (TiXmlAttribute* attr = inputElement->FirstAttribute();
            attr; attr = attr->Next()) {
            string value = this->getVariable(string(attr->Name()));
            if (value.empty()) {
                value = attr->Value(); 
            }
            outputElement->SetAttribute(string(attr->Name()), value);
        }
        outputNode = dynamic_cast <TiXmlNode*>(outputElement);
    } 
    return outputNode;
}

string XmlExpander::config(string tagName) 
{
	if (config_.find(tagName) != config_.end()) {
        return this->config_[tagName];
    } else {
        return string();
    }
}

string XmlExpander::getVariable(string varName)
{
    if (this->variables_.find(varName) != variables_.end()) {
        varState* currState = this->variables_[varName].top();
        if (this->currDepth >= currState->depth) {
            return currState->value;
        } else { 
            return string();
        }
    } else {
        return string();
    }
}


void XmlExpander::popVariableState()
{
    this->currDepth--;
    for (map<string, stack<varState*> >::iterator it = this->variables_.begin();
        it != this->variables_.end(); it++) {
        stack<varState*> stack = (*it).second;
        while (stack.top()->depth > this->currDepth) {
            stack.pop();
        }
        if (stack.empty()) {
            this->variables_.erase(it);
        }
    }
}

void XmlExpander::pushVariableState()
{
    this->currDepth++;
}



