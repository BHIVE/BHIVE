/*
 * reagon.cpp
 *
 *  Created on: Feb 24, 2010
 *      Author: msneddon
 */



#include "reagan.hh"
#include "../../parallel/parallel.hh"
#include "../../agent/data/primitive/primitiveData.hh"

using namespace Hive;


		Reagan::Reagan()
		{
			for(int i=0; i<Hive::mpisize(); i++)
				alreadyinthere.push_back(0);
		}

		void Reagan::sendMessage(Message* m)
		{
		//	cerr << "bomb iran " << m->getDestinationID()<<" "<<m->getDestinationNode() << " " << Hive::mpirank() << endl;
			if(m->getDestinationNode()==Hive::mpirank())
			{
		//		cerr<<"sending message to ohio"<<endl;
				this->localConnectionMap[m->getDestinationID()]->placeMessageOnMessageQueue(m);
			}

			else
			{
		//		cerr<<"sending message to russia"<<endl;
				this->overseasMessageQueue.push_back(m);
			}

		}

		/**
		 * composer adds an agent to the communication map
		 * @param a pointer to agent to be added to map
		 */
		void Reagan::addAgent(Agent *a)
		{
			// here the composor adds all LOCAL agents to this communicator
			localConnectionMap[a->getAgentId()] = a;

			// lookup which node the parent of this agent lives
			if(a->getParentNode()!=Hive::mpirank()) {
				if (!alreadyinthere[a->getParentNode()]) {
					alreadyinthere[a->getParentNode()] = 1;
					overseasDestinations.push_back(a->getParentNode());
				}

			}

			// lookup which nodes the children of this agent lives on
			for(unsigned int c=0; c<a->getNumOfChildren(); c++) {
				if(a->getChildAgentNode(c)!=Hive::mpirank()) {
					if (!alreadyinthere[a->getChildAgentNode(c)]) {
						alreadyinthere[a->getChildAgentNode(c)] = 1;
						overseasDestinations.push_back(a->getChildAgentNode(c));
					}
				}
			}

		}

		void Reagan::clearQueues() {
		//	for (unsigned int i=0; i<overseasMessageQueue.size(); i++)
		//		delete this->overseasMessageQueue[i];
			this->overseasMessageQueue.resize(0);

		//	for (unsigned int i=0; i<overseasReceivingQueue.size(); i++)
		//		delete this->overseasReceivingQueue[i];
			this->overseasReceivingQueue.resize(0);
		}


		/// method for sending/recieving accumulated messages to other nodes
		void Reagan::parallelCommunicate()
		{
			Hive::parallel_send_recv(overseasMessageQueue, overseasReceivingQueue, overseasDestinations);
			cerr << "ronnie on " << Hive::mpirank() << " received "
			<< overseasReceivingQueue.size() << " greeetings from russia" << endl;
			// deliver the messages to the agents.
			for (int i=0; i<overseasReceivingQueue.size(); i++) {
				Message *msg = overseasReceivingQueue[i];
				Data *d;
				msg->getArgument(d,0);
				this->localConnectionMap[msg->getDestinationID()]->placeMessageOnMessageQueue(msg);
			}
			// we have to remove the old stuff from the queues ...
//			this->overseasMessageQueue.resize(0);
//			this->overseasReceivingQueue.resize(0);
		}


