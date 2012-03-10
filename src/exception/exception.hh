/**
 * @file src/exception/exception.hh
 * @brief Contains the default Hive exception object
 *
 * The object HiveException is defined here and should be used throughout the
 * Hive framework and in new projects.
 *
 * @author Michael Sneddon
 * @date Started: Oct 19, 2009    Last edited: Oct 19, 2009
 *
 *
 */

#ifndef EXCEPTION_HH_
#define EXCEPTION_HH_


#include <iostream>

using namespace std;

namespace Hive
{

	/**
	 * @brief allows sending exceptions and checking the stack trace
	 *
	 * HiveException should be thrown whenever a runtime error is encountered.
	 * Whenever this exception type is created, you must give an error message
	 * and the name of the function where the error was generated (include
	 * the class name and / or namespace! as in: Util::toString()).  Whenever
	 * you catch such an exception, add the name of your method, and throw it
	 * again.  Then all exceptions will ultimately be handled in hive.cpp in
	 * the main function.  This allows you to give some level of stack trace
	 * for commonly encountered exceptions.
	 *
	 * Here is an example of how to use this class to generate an error:
	 *
	 * Hive::functionCall() {
	 *   if(gotToError)
	 *   	throw(HiveException("Random Error","Hive::functionCall()");
	 * }
	 *
	 * Then, if you think you might catch an error, add this code:
	 *
	 * Agent::funcitonCall2()
	 * {
	 *   try{
	 *     functionCall();
	 *   } catch (HiveException& e) {
	 *     e.addTraceException("Agent::funcitonCall2()");
	 *     throw e;
	 *   }
	 * }
	 *
	 *
	 * @author Michael Sneddon
	 * @date Oct 19, 2009 last edited: Oct 19, 2009
	 */
	class HiveException
	{
		public:

			/** constructor to use when an error is thrown
			 * @param message the error message, no newline characters please
			 * @param errorLocation the full function name where the error was generated
			 */
			HiveException(string message, string errorLocation);

			/** destructor that you shouldn't have to call
			 */
			~HiveException();

			/** allows you to add a trace location (function name) when caught
			 * You should use this method to add locations as you throw this
			 * error up the stack trace.  See the class comments on how to
			 * use this method.
			 * @param traceLocation the full function name where this error was caught
			 */
			void addTraceException(string traceLocation);

			/** returns the error message in traditional c++ exception style
			 * @return string the error message
			 */
			string what();

			/** get the error message string with the partial stack trace
			 * This method is called in Hive.cpp main and is really the only
			 * place where these errors should ultimately be caught
			 * @return string error message with the stack trace
			 */
			string getFullMessage() const;

		private:
			string message;
			string trace;
	};
}



#endif /* EXCEPTION_HH_ */
