//============================================================================
// Name        : ParallelCommunicationTest.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <queue>
#include <boost/thread.hpp>
#include "TCPSocket.h"

using namespace std;

int req = 0;

void fazRequisicoes(TCPSocket *socket, const char *cmd, float s) {
	printf("iniciou a thread fazRequisicoes\n");
	while(1) {
		//printf("E\n");
		socket->send(cmd, 1);
		req++;
		sleep(s);
	}

}


void recebeRequisicoes(TCPSocket *socket) {
	printf("iniciou a thread recebeRequisicoes\n");
	int tm = 5000;
	char buffer[tm];
	while(1) {

		socket->recv(buffer, tm);
		printf("%s\n", buffer);
		printf("%d\n", req);
		sleep(.5);

	}
}

int main() {

	TCPSocket socket;
	socket.connect("169.254.0.10", 10001);

	boost::thread requisicoes1(fazRequisicoes, &socket, "8", .2);
	boost::thread requisicoes2(fazRequisicoes, &socket, "5", .2);
	boost::thread requisicoes3(fazRequisicoes, &socket, "I", 1);
	boost::thread leitura(recebeRequisicoes, &socket);

	while(1);

	printf("finalizou a thread principal\n");

	return 0;
}
