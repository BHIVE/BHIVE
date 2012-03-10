/**
 * @file XmlExpander.cpp
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

#include "XmlExpander.hh"

using namespace Hive;


XmlExpander::XmlExpander() {
	// Setting default names for loop, create and set
	this->config("loop", "loop");
	this->config("create", "create");
	this->config("set", "set");
	this->config("scan", "scan");
	this->currDepth = 0;
};

XmlExpander::~XmlExpander()
{
    for (map<string, stack<varState*> >::iterator it = this->variables_.begin(); it != this->variables_.end(); it++) {
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

void XmlExpander::expandFile(string inputFilename, string outputFilename) {
	TiXmlDocument inputDocument(inputFilename.c_str());
	bool loadSucess = inputDocument.LoadFile();
	if(!loadSucess) {
		cerr << "Error in loading XML file: " << 
            inputFilename << " into composer." << endl;
		exit(1);
	}
    TiXmlDocument* outputDocument = this->generateNodeLevelDocument(&inputDocument);
    bool saveSucess = outputDocument->SaveFile(outputFilename.c_str());
    if (!saveSucess) {
        cerr << "Error in saving XML file: " <<
            outputFilename << " in composer." << endl;
    }
	delete(outputDocument);
}

TiXmlDocument* XmlExpander::generateNodeLevelDocument(TiXmlDocument* inputDocument)
{
    vector<TiXmlNode*> outputNodes = this->expandNode((TiXmlNode*) inputDocument->RootElement());
    TiXmlNode* outputNode;
	if (outputNodes.size() > 1) {
		cerr << "Root level expandNode generated multiple children" << endl;
	} else {
		outputNode = outputNodes[0];
	}
	TiXmlDocument* outputDocument = new TiXmlDocument();
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    outputDocument->LinkEndChild(decl);
    outputDocument->LinkEndChild(outputNode);
    return outputDocument;
}

void XmlExpander::setVariable(string name, string value)
{
    varState* currState = new varState;
    currState->depth = this->currDepth;
    currState->value = value;
	currState->max = 0;
	currState->step_size = 0;
    if(this->variables_.count(name) > 0) {
        this->variables_[name].push(currState);
    } else {
        stack<varState*> stateStack;
        stateStack.push(currState);
        this->variables_[name] = stateStack;
    }
}

void XmlExpander::setVariable(string name, string value, double step_size, double max)
{
	varState* currState = new varState;
	currState->depth = this->currDepth;
	currState->value = value;
	currState->max = max;
	currState->step_size = step_size;
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
  

vector<TiXmlNode*> XmlExpander::expandNode(TiXmlNode* inputNode)
{
    vector<TiXmlNode*> outputNodes;
	bool noParentNode = true;
	this->setVariable("__loop__", "1");
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
					// normal distribution normal(mean, varience)
                    double normal = Util::RANDOM_GAUSSIAN();
                    if ((int) command.size() == 2) {
                        double mean = atof(command.front().c_str());
                        command.pop_front();
                        double variance = atof(command.front().c_str());
                        normal = mean + variance * normal;
                    } else if((int) command.size() == 0) {
						// do nothing, normal already set    
					} else {
						cerr << "(mean, variance) or () required for normal function in <scan .../" << endl;
					}
                    ostringstream strs;
                    strs << normal;
                    value = strs.str();
					this->setVariable(name, value);
                } else if (function.compare("continuous") == 0) {
					// continuous distribution continuous(min,max)
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
					this->setVariable(name, value);
				} else if (function.compare("intervalByStepSize") == 0) {
					// equally spaced intervals set by step size intervalByStepSize(step_size, start, stop)
					if ((int) command.size() == 3) {
						double step = atof(command.front().c_str());
						command.pop_front();
						double start = atof(command.front().c_str());
						command.pop_front();
						double stop = atof(command.front().c_str());
						ostringstream strs;
						strs << start;
						value = strs.str();
						this->setVariable(name, value, step, stop);						
					} else {
						cerr << "(step,start,stop) arguments required for intervalByStepSize function in <scan .../>" << endl;
					}
				} else if (function.compare("intervalByCount") == 0) {
					// N intervals equally spaced intervalByCount(count, start, stop)
					if ((int) command.size() == 3) {
						double count = atof(command.front().c_str());
						command.pop_front();
						double start = atof(command.front().c_str());
						command.pop_front();
						double stop  = atof(command.front().c_str());
						double step = (stop - start)/count;
						ostringstream strs;
						strs << start;
						value = strs.str();
						this->setVariable(name, value, step, stop); 
					} else {
						cerr << "(count,start,stop) arguments required for intervalByCount function in <scan .../>" << endl;
					}
                } else {
                    cerr << "Unknown distribution function in <scan ../> " <<
                        " Got '" << function << "'"<< endl;
                }
            } else {
                cerr << "Warning, invalid use of <scan ../>" <<
                    " Must have attributes 'distribution' and 'name'." << endl;
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
        outputNodes.push_back(this->interpretNode(inputNode));
		noParentNode = false;
    }
    // push state as we are looking at node's children now...
	this->pushVariableState();
	int loopMax = atoi(getVariable("__loop__").c_str());
    for (int i=0; i < loopMax; i++) {
        // now parse children
        for (TiXmlNode* inputChild = inputNode->FirstChild(); 
            inputChild; inputChild = inputChild->NextSibling()) {
            vector<TiXmlNode*> outputChildren = this->expandNode(inputChild); 
			for (vector<TiXmlNode*>::iterator it = outputChildren.begin();
				it < outputChildren.end(); it++) {
					if (noParentNode) {
						outputNodes.push_back(*it);
					} else {
						outputNodes[0]->LinkEndChild(*it);
					}
			}
		}
    }
    this->popVariableState();
    // done with node's children, pop state and return
    return outputNodes;
}

TiXmlNode* XmlExpander::interpretNode(TiXmlNode* inputNode) {
    TiXmlNode* outputNode;
    if (inputNode->Type() == inputNode->ELEMENT) {
        TiXmlElement* outputElement = new TiXmlElement(inputNode->Value());
        TiXmlElement* inputElement = inputNode->ToElement();
        for (TiXmlAttribute* attr = inputElement->FirstAttribute();
            attr; attr = attr->Next()) {
            string value = this->getVariable(string(attr->Value()));
            if (value.empty()) {
                value = attr->Value(); 
            }
            outputElement->SetAttribute(string(attr->Name()), value);
        }
        outputNode = dynamic_cast <TiXmlNode*>(outputElement);
	} else if (inputNode->Type() == inputNode->COMMENT) {
		outputNode = inputNode->ToComment()->Clone();
	} else if (inputNode->Type() == inputNode->TEXT) {
		string nodeText = this->getVariable(string(inputNode->Value()));
		if (nodeText.empty()) {
			nodeText = inputNode->Value();
		}
		TiXmlText* outputText = new TiXmlText(nodeText);
		outputNode = dynamic_cast <TiXmlNode*>(outputText);
	} else if (inputNode->Type() == inputNode->DECLARATION) {
		TiXmlDeclaration* inputDec = inputNode->ToDeclaration();
		TiXmlDeclaration* outputDec = new TiXmlDeclaration(inputDec->Version(),
			inputDec->Encoding(), inputDec->Standalone());
		outputNode = dynamic_cast <TiXmlNode*>(outputDec);
	} else if (inputNode->Type() == inputNode->UNKNOWN) {
		outputNode = dynamic_cast <TiXmlNode*>(inputNode->ToUnknown()->Clone());
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
	string rtv = string();
    if (this->variables_.find(varName) != variables_.end()) {
        varState* currState = this->variables_[varName].top();
        if (this->currDepth >= currState->depth) {
			if ((currState->step_size != (double)0) && (currState->max >
				atof(currState->value.c_str()) )) {
				rtv = string(currState->value); // copy of rtv string
				double newVal = atof(currState->value.c_str()) + currState->step_size;
				ostringstream strs;
                strs << newVal;
                currState->value = strs.str();
			} else {
	            rtv = currState->value;
			}
		}
	}
	return rtv;
}


void XmlExpander::popVariableState()
{
    this->currDepth--;
	vector<string> deleted;
    for (map<string, stack<varState*> >::iterator it = this->variables_.begin();
        it != this->variables_.end(); it++) {
        stack<varState*>* stack = &(*it).second; // NOT PRETTY
        while (!stack->empty() && stack->top()->depth > this->currDepth) {
            stack->pop();
        }
        if (stack->empty()) {
			deleted.push_back((*it).first);
        }
    }
	for (vector<string>::iterator it = deleted.begin();
		it != deleted.end(); it++) {
			this->variables_.erase((*it));
	}
}

void XmlExpander::pushVariableState()
{
    this->currDepth++;
}



