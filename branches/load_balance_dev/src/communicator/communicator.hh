#ifndef _COMMUNICATOR_HH_
#define _COMMUNICATOR_HH_

#include <map>

#include "../agent/agent.hh"
#include "../agent/message/message.hh"

using namespace std;

namespace Hive {

class Message;

class Communicator {
  public:
    Communicator();
    ~Communicator();

    void addAgent(Agent *a);                            // obsolete
    void sendMessage(Message* m);
    void parallelCommunicate();
    void clearQueues();
    void setupAgentMap(map<int, Agent*>* agentMap);
    void setupNeighbors(const vector<int>& neighbors);

  protected:
    void sendLocalMessage(Message* m);
    void sendRemoteMessage(Message* m);

  private:
    map<int, Agent*>* agentMap_;   // pointer to the ever-changing agentMap in PropogatorClass
    vector<int>       neighbors_;  // static list of neighboring nodes provided by universe
    vector<Message*>  outbox_;     // remote send
    vector<Message*>  inbox_;      // remote recv
};

}
#endif /* _COMMUNICATOR_HH_ */

