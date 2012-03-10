/*
 * Creator.cpp
 *
 *  Created on: Oct 22, 2009
 *      Author: Johnyboy
 */


#include <iostream>
#include <exception>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


#include "../../agent/agent.hh"
#include "../../agent/database/database.hh"
#include "../../agent/data/data.hh"
#include "../../agent/data/primitive/primitiveData.hh"
#include "../../exception/exception.hh"

#include "../../projects/SmallLonelyWorld/WorldFactory.hh"
#include "../../projects/SmallLonelyWorld/BacteriumFactory.hh"
#include "../../projects/SmallLonelyWorld/Creator.hh"

#include "../../hive.hh"


//#include "projects/SmallLonelyWorld/SmallLonelyWorld.hh"
using namespace std;
using namespace Hive;
using namespace SmallLonelyWorld;

Creator::Creator()
{
	//composing the world hierarchy(i havent figured out the composer yet)
	WorldFactory *WF = new WorldFactory();
	Agent *W = WF->createAgent();
	BacteriumFactory *BF = new BacteriumFactory();
	Agent *B = BF->createAgent();
	W->addChild(B->getAgentId());
	B->setParent(W->getAgentId());
	//dvorak could do better, I suppose

	//initializing the position, ligand_level,ligand and phosphorylation level
	srand(time(NULL));
	int position = rand()%3;
	int ligand_level = rand()%3;
	double phosph = 0;
	double ligand;
	switch (ligand_level) {
		case 0: ligand = 10; break;
		case 1: ligand = 1000; break;
		case 2: ligand = 100000; break;
	}
	cout << endl<<endl<<endl;
	cout << "Bacterium is in box "<<position<<endl;
	cout << "The ligand level is "<<ligand_level<<endl;
	cout << "The ligand conc is "<<ligand<<endl;
	cout << "The phosph conc is "<<phosph <<endl;

	//Initializing the Databases
	IntegerData *data1 = new IntegerData("Ligand Level",ligand_level);
	DoubleData *data2 = new DoubleData("Phosph Conc", phosph);
	W->getDatabase()->addData(data1->getName(),data1);
	W->getDatabase()->addData(data2->getName(),data2);
	DoubleData *data3 = new DoubleData("Ligand Conc",ligand);
	DoubleData *data4 = new DoubleData("Phosph Conc", phosph);
	B->getDatabase()->addData(data3->getName(),data3);
	B->getDatabase()->addData(data4->getName(),data4);
}
