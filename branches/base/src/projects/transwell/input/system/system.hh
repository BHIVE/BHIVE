/**
 * @file system.hh
 *
 * This file contains the interface of the system class.
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13, 2009 last edited: 10-14-2009 by Garrit and Michael
 *
 */

#ifndef SYSTEM_HH_
#define SYSTEM_HH_


namespace Hive
	{

	/**
	 * @brief system class
	 *
	 * The system maintains the types of agents that can exist in the simulation. Types of objects allow
	 * for the on the fly creation of new agents.
	 *
	 * @todo everything, more conceptional work needed. will we have prototypes of agents in here?!
	 * it should be a database of types ... whatever ...
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 13th, 2009 last edited 12-15 by garrit
	 */

	class System {
	public:
		/// Constructor
		System(int type) {this->system_type = type;};

		/// destructor
		virtual ~System() {};

		/*
		 * returns the system type
		 * @return system type
		 */
		int getSystemType() const { return system_type; };

		/// system type network_free_type
		const static int network_free_type = 0;

		/// system type kinetic network
		const static int kinetic_type = 1;

	protected:
		/// system type of this system
		int system_type;

	};
}
#endif /* SYSTEM_HH_ */
