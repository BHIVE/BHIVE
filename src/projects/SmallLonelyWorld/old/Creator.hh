/*
 * Creator.hh
 *
 *  Created on: Oct 22, 2009
 *      Author: Johnyboy
 */


#ifndef CREATOR_HH_
#define CREATOR_HH_

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

class Creator
{
public:
	Creator();
	~Creator(){};

	//void initialize();
};

#endif /* CREATOR_HH_ */
