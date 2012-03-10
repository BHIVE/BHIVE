/*
 * mapintintdata.cpp
 *
 *  Created on: Jun 18, 2010
 *      Author: msneddon
 */


#include "mapintintdata.hh"

using namespace Hive;
using namespace std;



MapIntIntData::MapIntIntData(string dataName) : Data(dataName,"mapintintdata")
{
}
MapIntIntData::~MapIntIntData() {}


void MapIntIntData::insert(int key, int value)
{
	m.insert(pair<int,int>(key,value));
}


void MapIntIntData::erase(int key)
{
//	cerr<<" MapIntIntData::erase(int key), "<< this->getName()<<" key: "<<key<<" map size: "<<m.size()<<endl;
//	cerr<<"--- before:"<<endl;
//	map<int,int>::iterator i;
//	for(i=m.begin(); i!=m.end(); i++) {
//		cerr<<" contents: "<< i->first <<" -> "<<i->second<<endl;
//	}

	if(m.empty())
		throw HiveException("map is empty, cannot remove element!", "MapIntIntData::erase(int key)");
	if(m.find(key)==m.end()) {
		throw HiveException("map cannot find element!", "MapIntIntData::erase(int key)");
	}


	m.erase(m.find(key));

	//cerr<<"--- after:"<<endl;
	//for(i=m.begin(); i!=m.end(); i++) {
	//	cerr<<" contents: "<< i->first <<" -> "<<i->second<<endl;
	//}
}


int MapIntIntData::size()
{
	return m.size();
}


void MapIntIntData::set(int key, int value)
{
	m[key]=value;
}


void MapIntIntData::getPtrToMap(map<int,int> *ref)
{
	ref = &m;
}


// will return real index in vectors for agent with agentId id
int MapIntIntData::returnValueForKey(int key)
{
	return (m.find(key))->second;
}


void MapIntIntData::printContent(ostream& out) {
     cerr << "MapIntIntData::printContent(ostream& out) not implemented" << endl;
}
