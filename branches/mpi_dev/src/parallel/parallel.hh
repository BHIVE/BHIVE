/*
 * parallel.hh
 *
 *  Created on: Feb 23, 2010
 *      Author: fang-fang, chris, michael, garrit
 */

#ifndef PARALLEL_HH_
#define PARALLEL_HH_

#include <cstring>
#include <string>
#include <vector>

#include "../agent/message/message.hh"


/**
 *  this wraps up the mpi code / calls. functions declared in here
 *  are used by the parallel implementations of the communicator,
 *  composer etc.
 */

using namespace std;

namespace Hive {

int mpisize();

int mpirank();

void parallel_initialise(int argc, char *argv[],int& inrank, int& insize);

void parallel_finalise();

void parallel_file_input(string filename, vector <string> &result, int input_node = 0);

void parallel_send(int dest, char* data, int datalen, int tag = 0);


void parallel_recv(int source, char*& data, int& datalen, int tag = 0);

void parallel_pack_messages(vector<Message*> outbox, int& totalSize, char*& completeMessage);

void parallel_unpack_messages(vector<Message*>& inbox,int recvLength,char* recvBuffer);

void parallel_send_recv(vector<Message*> outbox,vector<Message*>& inbox,vector<int> neighbors);


const int PAR_MSG_SIZE = 1 << 20;
const int TAG_MSG      = 99;
const int TAG_DATA    = 99;

struct ParallelMessage
{
    int src;
    int tag;
    int len;
    char data[PAR_MSG_SIZE];
};

}
#endif /* PARALLEL_HH_ */
