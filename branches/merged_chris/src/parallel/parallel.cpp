#include "parallel.hh"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <set>

#include "../util/util.hh"
#include "../agent/data/primitive/primitiveData.hh"

using namespace std;
using namespace Hive;

#ifdef BHIVEMPI
#include <mpi.h>
#endif


int rank = 0;
int size = 1;

Hive::ParallelMessage parallel_out_message;
Hive::ParallelMessage parallel_in_message;

#ifdef BHIVEMPI
MPI_Request reqs[2];
MPI_Status stats[2];
#endif

int Hive::mpisize() {
	return size;
}

int Hive::mpirank() {
	return rank;
}

void parallel_test_4();

void parallel_test()
{
    cout << "parallel_test" << endl;
    parallel_test_4();
}

void Hive::parallel_initialize(int argc, char *argv[], int& inrank, int& insize) {
#ifdef BHIVEMPI
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    // cerr <<" hello mpi.  i am "<< rank << " of " << size << endl;
#endif

    inrank = rank;
    insize = size;

#ifdef PARATEST
    parallel_test();
    parallel_finalise();
    exit(0);
#endif
}

void Hive::parallel_finalize() {
#ifdef BHIVEMPI
	MPI_Finalize();
#endif
//	cerr << "done finalizing" << endl;
}

void Hive::parallel_abort(int code, string message) {
#ifdef BHIVEMPI
    if (mpirank() == 0) {
        cerr << message << endl;
        MPI_Abort(MPI_COMM_WORLD, code);
    } else {
        while (1) { }
    }
#else
    exit(code);
#endif
}

void Hive::parallel_barrier() {
#ifdef BHIVEMPI
	MPI_Barrier(MPI_COMM_WORLD);
#endif
}

void Hive::parallel_send(int dest, char* data, int datalen, int tag) {
    Hive::ParallelMessage* msg = &parallel_out_message;
    msg->src = rank;
    msg->tag = tag;
    msg->len = datalen;
    if (datalen > 0 && data != 0)
        memcpy(msg->data, data, datalen);
    int actlen = sizeof(msg->src) + sizeof(msg->tag) + sizeof(msg->len) + datalen;
#ifdef BHIVEMPI
    MPI_Send(msg, actlen, MPI_CHAR, dest, TAG_MSG, MPI_COMM_WORLD);
#endif
}


void Hive::parallel_recv(int source, char*& data, int& datalen, int tag)
{
    Hive::ParallelMessage* msg = &parallel_in_message;
#ifdef BHIVEMPI
    MPI_Status status;
    // MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // if (status.MPI_TAG == TAG_DATA) {
        // int id = status.MPI_SOURCE;
        // msg.src = id;
        // msg.tag = rpt_data;
        // MPI_Recv(raw_buffers[id], incoming_sizes[id], MPI_CHAR, MPI_ANY_SOURCE, TAG_DATA, MPI_COMM_WORLD, &status);
    // } else {

    MPI_Recv(msg, sizeof(*msg), MPI_CHAR, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &status);

    data = new char[msg->len];

    memcpy(data, msg->data, msg->len);
    //   data = msg->data;
    datalen = msg->len;
    // }
    // printf("[%d] parallel_recv from [%d]: datalen = %d\n", mpirank(), source, datalen);
    // cerr << "[" << mpirank() << "] " << parallel_recv: datalen = " << datalen << endl;
#endif
}

void Hive::parallel_isend(int dest, char* data, int datalen, int tag) {
    Hive::ParallelMessage* msg = &parallel_out_message;
    msg->src = rank;
    msg->tag = tag;
    msg->len = datalen;
    if (datalen > 0 && data != 0)
        memcpy(msg->data, data, datalen);
    int actlen = sizeof(msg->src) + sizeof(msg->tag) + sizeof(msg->len) + datalen;
#ifdef BHIVEMPI
    MPI_Isend(msg, actlen, MPI_CHAR, dest, TAG_MSG, MPI_COMM_WORLD, &reqs[0]);
#endif
}


void Hive::parallel_irecv(int source, int tag)
{
    Hive::ParallelMessage* msg = &parallel_in_message;
#ifdef BHIVEMPI
    MPI_Irecv(msg, sizeof(*msg), MPI_CHAR, MPI_ANY_SOURCE, TAG_MSG, MPI_COMM_WORLD, &reqs[1]);
#endif

}

void Hive::parallel_waitall(char*& data, int& datalen) 
{
    Hive::ParallelMessage* msg = &parallel_in_message;
#ifdef BHIVEMPI
    MPI_Waitall(2, reqs, stats);
    data = new char[msg->len];
    memcpy(data, msg->data, msg->len);
    datalen = msg->len;
#endif
}

void Hive::parallel_file_input(string filename, vector <string> &result,int input_node) {
	string concatenatedfile;
	int length;
	char* buffer;
	if (input_node == rank) {
		ifstream Input(filename.c_str());
		while (!Input.eof()) {
			string Line;
			getline( Input, Line );
			result.push_back(Line);
			if (concatenatedfile.length() > 0) {
				concatenatedfile.append("\n");
			}
			concatenatedfile.append(Line);
		}
		Input.close();
		buffer = new char[concatenatedfile.length()+1];
		strcpy(buffer,concatenatedfile.data());
		length = concatenatedfile.length()+1;
	}
#ifdef BHIVEMPI
	MPI_Bcast(&length, 1, MPI::INT, input_node, MPI::COMM_WORLD);
	if (rank != input_node) {
		buffer = new char[length];
	}
	MPI_Bcast(buffer,length, MPI::CHAR, input_node, MPI::COMM_WORLD);
	if (rank != input_node) {
		concatenatedfile.assign(buffer);
		Util::StringToStrings(concatenatedfile,"\n",result);
	}
	delete [] buffer;
#endif
}

void Hive::parallel_send_recv(vector<Message*> outbox,vector<Message*>& inbox,vector<int> neighbors) {
	//Adding outbox messages to map structure
	map<int,vector<Message*> > cpuMessageMap;
	// cerr << "in sendrecv. " << Hive::mpirank() << " " << outbox.size() << endl;
	for (int i = 0; i < int(outbox.size()); i++) {
		int dest = outbox[i]->getDestinationNode();
		cpuMessageMap[dest].push_back(outbox[i]);
	}

	//Sorting the neighbors by processor ID
	sort(neighbors.begin(), neighbors.end());

	//Exchanging messages with neighbors in order of processor ID
    // printf("[%d] has %d neighbors\n", mpirank(), neighbors.size());
    
	for (int i = 0; i < int(neighbors.size()); i++) {
		//Send structures
		int sendLength = 0;
		char* sendBuffer = NULL;
		if (cpuMessageMap[neighbors[i]].size() > 0) {
			parallel_pack_messages(cpuMessageMap[neighbors[i]], sendLength, sendBuffer);
		}
		//Receive structures
		int recvLength = 0;
		char* recvBuffer = NULL;

		//Sending and receiving messages
        //Synchronous
		// if (neighbors[i] < mpirank()) {
			// parallel_send(neighbors[i], sendBuffer, sendLength);
			// parallel_recv(neighbors[i], recvBuffer, recvLength);
		// } else if (neighbors[i] > mpirank()) {
			// parallel_recv(neighbors[i], recvBuffer, recvLength);
			// parallel_send(neighbors[i], sendBuffer, sendLength);
		// }
        //Asynchronous
		if (neighbors[i] < mpirank()) {
			parallel_isend(neighbors[i], sendBuffer, sendLength);
			parallel_irecv(neighbors[i]);
		} else if (neighbors[i] > mpirank()) {
			parallel_irecv(neighbors[i]);
			parallel_isend(neighbors[i], sendBuffer, sendLength);
		}
        parallel_waitall(recvBuffer, recvLength);

        //Common for sync/async
		delete [] sendBuffer;

		//Translating the received message into message objects
		if (recvLength > 0 && recvBuffer != NULL) {
			parallel_unpack_messages(inbox, recvLength, recvBuffer);
		}
	}
    parallel_barrier();
    // cout << "parallel_send_recv reached MPI_Barrier" << endl;
}


void Hive::parallel_pack_messages(vector<Message*> outbox, int& totalSize, char*& completeMessage) {
    totalSize = 0;
    vector<char*> messageBlurb;
    vector<int> messageSize;
  //  cerr << "outboxsize" << outbox.size() << " " << Hive::mpirank() << endl;
    for (int i=0; i < (int) outbox.size(); i++) {
        char* temp = NULL;
        int size;
        Data* messageData = NULL;
        outbox[i]->getArgument(messageData);
        if (messageData == NULL) {
            size = 0;
        } else {
            messageData->packDataForMPI(temp,size);
        }
        char* newMessage = new char[size + sizeof(int) * 3];
        ((int*)newMessage)[0] = outbox[i]->getDestinationID();
        ((int*)newMessage)[1] = outbox[i]->getActionId();
        ((int*)newMessage)[2] = size;
        memcpy(newMessage + sizeof(int) * 3, temp, size);
        // for (int j = sizeof(int)*3; j < size + sizeof(int) * 3; j++) {
            // newMessage[j] = temp[j];
        // }
        totalSize += size + 3 * sizeof(int);
        messageBlurb.push_back(newMessage);
        // messageSize.push_back(totalSize);
        messageSize.push_back(size + sizeof(int) * 3);
        if (temp != NULL) {
            delete[] temp;
        }
    }
    completeMessage = new char[totalSize];
    int current = 0;
    for (int i = 0; i < (int) messageBlurb.size(); i++) {
        for (int j = 0; j < messageSize[i]; j++) {
            completeMessage[current] = messageBlurb[i][j];
            current++;
        }
        delete[] messageBlurb[i];
    }
//    cerr << "hier " << totalSize << " " << Hive::mpirank() << endl;
}

void Hive::parallel_unpack_messages(vector<Message*>& inbox,int recvLength,char* recvBuffer) {
//	cerr << "entering parallel unpack of messages" << endl;
	char* currentBuffer = recvBuffer;
	int currentLocation = 0;
	while (currentLocation < recvLength) {
		Message* newMessage = new Message();
		newMessage->setDestination(((int*)currentBuffer)[0], mpirank());
		newMessage->setAction(((int*)currentBuffer)[1]);

		int size = ((int*)currentBuffer)[2];
		char* messageBuffer = new char[size];
		for (int j = 0; j < size; j++) {
			messageBuffer[j] = currentBuffer[sizeof(int) * 3 + j];
		}
		//Creating the buffer data object to store the message data
		RawBufferData* messageData = new RawBufferData("mpi message", messageBuffer, size);
		newMessage->setArgument(messageData);
		inbox.push_back(newMessage);
		currentBuffer   += size + 3 * sizeof(int);
		currentLocation += size + 3 * sizeof(int);
	}
	delete[] recvBuffer;
}



void parallel_test_1()
{
    // test pack & unpack of a single double/int data
    cout << "parallel_test_double" << endl;

    vector<Message*> outbox;
    int sendSize;
    char* sendMessage;

    for (int i = 1; i <= 9; ++i) {
        Message* m = new Message();
        m->setDestination(i, i * 10);
        m->setAction(i * 100);
        // m->setArgument(new IntegerData("IntegerData", i * 1000));
        m->setArgument(new DoubleData("DoubleData", i + 0.01 * i));
        outbox.push_back(m);
    }

    parallel_pack_messages(outbox, sendSize, sendMessage);

    vector<Message*> inbox;
    // int recvSize;
    // char* recvMessage;
    // parallel_unpack_messages(inbox, recvSize, recvMessage);

    parallel_unpack_messages(inbox, sendSize, sendMessage);

    for (unsigned int i = 0; i < inbox.size(); ++i) {
        Message* m = inbox[i];
        int destinationId = m->getDestinationID();
        int actionId = m->getActionId();
        RawBufferData* data;
        m->getArgument((Data*&)data);

        cout << "destinationId = " << destinationId << endl;
        cout << "actionId = " << actionId << endl;
        cout << "data->getSize() = " << data->getSize() << endl;
        // cout << "*(int*)(data->getBuffer()) = " << *(int*)(data->getBuffer()) << endl;
        cout << "*(double*)(data->getBuffer()) = " << *(double*)(data->getBuffer()) << endl;
        data->printContent(cout);
        cout << endl;
    }

}

void parallel_test_2()
{
    // test DoubleVectorData
    cout << "parallel_test_dvd" << endl;

    vector<Message*> outbox;
    int sendSize;
    char* sendMessage;

    for (int i = 1; i <= 5; ++i) {
        Message* m = new Message();
        m->setDestination(i, i * 10);
        m->setAction(i * 100);
        int dvdSize = 10;
        DoubleVectorData* dvd = new DoubleVectorData("testDvd", 0);
        for (int j = 0; j < dvdSize; ++j) {
            dvd->addElementToEnd(i + 0.01 * j);
        }

        m->setArgument(dvd);
        outbox.push_back(m);
    }

    parallel_pack_messages(outbox, sendSize, sendMessage);

    vector<Message*> inbox;

    parallel_unpack_messages(inbox, sendSize, sendMessage);

    for (unsigned int i = 0; i < inbox.size(); ++i) {
        Message* m = inbox[i];
        int destinationId = m->getDestinationID();
        int actionId = m->getActionId();
        RawBufferData* data;
        m->getArgument((Data*&)data);

        cout << "destinationId = " << destinationId << endl;
        cout << "actionId = " << actionId << endl;
        cout << "data->getSize() = " << data->getSize() << endl;

        char* buf = data->getBuffer();

        DoubleVectorData* dvd = new DoubleVectorData("testDvd", 0);
        dvd->unpackDataForMPI(buf, data->getSize());

        dvd->printContent(cout);

        // data->printContent(cout);
        cout << endl;
    }
}

void parallel_test_3()
{
    // test simple send and recv
    cout << "parallel_test" << endl;
    if (rank == 0) {
        char outbox[100] = "send message";
        // MPI_Send(outbox, 30, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
        parallel_send(1, outbox, 80);
        cout << "outbox = " << outbox << endl;
    } else if (rank == 1) {
        // char inbox[100] = "recv message";
        char* inbox;
        int len;
        parallel_recv(0, inbox, len);
        // char inb[100] = "old message";
        // MPI_Status status;
        // MPI_Recv(inb, 30, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        // cout << "inb = " << inb << endl;
        cout << "inbox = " << inbox << endl;
        cout << "len = " << len << endl;

        delete inbox;
    }
}

void parallel_test_4()
{
    vector<int> neighbors;
    neighbors.push_back(1);
    neighbors.push_back(2);
    neighbors.push_back(3);

    // neighbors.push_back(5);
    // neighbors.push_back(7);
    // neighbors.push_back(8);

    set<int> ns;
    for (vector<int>::iterator it = neighbors.begin(); it != neighbors.end(); ++it) {
        ns.insert(*it);
    }

    vector<Message*> outbox;
    if (ns.find(mpirank()) == ns.end())
        return;

    for (int i = 0; i < mpirank(); ++i) {
        int dest = i;
        if (dest == 0) dest = 3;
        for (int j = 0; j < 5 - dest; ++j) {
            Message* m = new Message();
            m->setDestination(dest, dest);
            m->setAction(dest);
            m->setArgument(new DoubleData("DoubleData", mpirank() + 0.01 * dest + 0.0001 * (j+1)));
            // int dvdSize = 10;
            // DoubleVectorData* dvd = new DoubleVectorData("testDvd", 0);
            // for (int j = 0; j < dvdSize; ++j) {
            //     dvd->addElementToEnd(i + 0.01 * j);
            // }
            // m->setArgument(dvd);
            outbox.push_back(m);
        }
    }

    vector<Message*> inbox;

    parallel_send_recv(outbox, inbox, neighbors);

    for (unsigned int i = 0; i < inbox.size(); ++i) {
        Message* m = inbox[i];
        int destinationId = m->getDestinationID();
        int actionId = m->getActionId();
        RawBufferData* data;
        m->getArgument((Data*&)data);

        cout << "destinationId = " << destinationId << endl;
        cout << "actionId = " << actionId << endl;
        cout << "data->getSize() = " << data->getSize() << endl;
        cout << "*(double*)(data->getBuffer()) = " << *(double*)(data->getBuffer()) << endl;
        cout << endl;
    }


	cerr << "leaving parallel unpack of messages" << endl;
}
