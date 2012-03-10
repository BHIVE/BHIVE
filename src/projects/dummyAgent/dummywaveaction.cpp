/*
 *@file  dummywaveaction.cpp
 *
 *implements the dummywaveaction

 *@author garrit jentsch
 *@date Oct 14, 2009 last edited 10-15-09 by garrit
 *
 */

#include "dummywaveaction.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include <stdlib.h>

using namespace std;
using namespace Hive;
using namespace Dummy;


DummyWaveAction::DummyWaveAction() {}

DummyWaveAction::DummyWaveAction(int id) : Action(id) {}

void DummyWaveAction::fire(Data *dat) {
	int d;
	// test for integer data
	if (dat->getType() != "IntegerType") {
		cerr << "you should contact thierry emonet at yale university. something is going wrong with your simulation."
		     << endl;
		cerr << "sorry for this but i really have to abort now." << endl;
		exit(1);
	}
	else
		d = ((IntegerData *) dat)->getInteger();
	for (int i=0; i<d; i++)
		cout << "wave" << endl;

	IntegerData *id = (IntegerData *) agent->getDatabase()->getDataItem("waveNumber");
	id->setInteger(id->getInteger()+d);

}
