/*
 * @file message.hh
 *
 * this file contains the interface to the message class.
 *
 *@author Garrit Jentsch
 *
 *
 *@date Oct 13th, 2009 last edited: 10-14-2009 by garrit and michael
 *
 */

#ifndef MESSAGE_HH_
#define MESSAGE_HH_

#include <vector>

#include "../data/data.hh"
#include "../agent.hh"

using namespace std;


namespace Hive
{

	/**
	 * @brief message for agent communication.
	 *
	 * messages are exchanged by agents to communicate with one another. with the help of a message agents tell
	 * each other which action they should take.
	 *
	 * @see action
	 *
	 * @todo everything. how do we specify the parameters ?!
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 13th, 2009 last edited: 10-14-2009 by Garrit and Michael
	 */

	class Message {
	public:
		/// Constructor
		Message();

		/// destructor
		~Message();

		/**
		 * returns actionid of the action to be taken by the destination agent
		 * @return actionid
		 */
		int getActionId();

		int getDestinationID();

		int getSourceID();

		int getDestinationNode();

		/**
		 * set action which has to be communicated
		 * @param a action to be broadcasted.
		 */
		void setAction(int a);

		void setDestination(int id, int node);
		void setDestination(int id);

		void setSourceID(int id, int node=0);

		int getDestinationNode() const;

		/**
		 * adds an arbitrary data item to the parameter list
		 * @param d data item
		 */
		void setArgument(Data& d);

		/**
		 * adds a pointer to an arbitrary data item to the parameter list
		 * @param *d pointer to data item
		 */
		void setArgument(Data *d);

		/**
		 * return data item at a certain position
		 * @param *d return parameter
		 * @param i position
		 */
		void getArgument(Data *&d, int i=0);



	private:
		/// identifyer of the action that has to be taken
		int action_to_be_taken;

		/// time at which action in message should be executed
		double time_stamp;

		/// vector of dataitems storing arguments
		vector<Data* > parameter;

		/// agentID of the destination to which this message is send
		int destinationID;

		int sourceID;
		int sourceNode;

		/// nodeID of the destination
		int destinationNode;
	};
}

#endif /* MESSAGE_HH_ */
