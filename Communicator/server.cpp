#include <map>
#include <stdio.h>
#include <sys/types.h>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <exception>
#include <iostream>
#include <signal.h>
#include <arpa/inet.h>

#ifndef PORTNO_H
#define PORTNO_H (4097)
#endif

static std::map<size_t, int> FileDescriptors;
static size_t id = 0;
static int serverFd = 0;

void run(int fd, size_t handle) {
	char inp;
	while(true) {
		int ret = recv(fd, &inp, sizeof(char), MSG_NOSIGNAL);
		if(ret != sizeof(inp)) {
			int err = errno;
			if(err == EAGAIN || err == EINTR) {
				continue;
			} else {
				fprintf(stderr, "ERROR: Unable to recv data :'(\n");
				break;
			}
		}


		for(std::map<size_t, int>::const_iterator it = FileDescriptors.begin();
		    it != FileDescriptors.end(); ++it) {
			ret = send(it->second, &inp, sizeof(inp), MSG_NOSIGNAL);
			if(ret != sizeof(inp)) {
				int err = errno;
				if(err == EAGAIN || err == EINTR) {
					--it;
					continue;
				} else {
					fprintf(stderr, "Client with (ID, FD), (%d, %d), exited!", (int) it->first, it->second);
				}
			}
		}
	}
}

void sig(int sig) {
	for(std::map<size_t, int>::const_iterator it = FileDescriptors.begin(); it != FileDescriptors.end(); ++it) {
		shutdown(it->second, SHUT_RDWR);
		close(it->second);
	}
	shutdown(serverFd, SHUT_RDWR);
	close(serverFd);
}

void thd(int fd) {
	try {
		FileDescriptors[id] = fd;
		run(fd, id++);
	} catch(std::exception& e) {
		fprintf(stderr, "ERROR: Exception thrown in thread:\n\t[%s]\n", e.what());
	}

	shutdown(fd, SHUT_RDWR);
	close(fd);
}

int main(int argc, char** argv) {
	signal(SIGINT, sig);
	signal(SIGTERM, sig);
	struct sockaddr_in clientAddress;
	struct sockaddr_in serverAddress;

	memset(&clientAddress, 0, sizeof(clientAddress));
	memset(&serverAddress, 0, sizeof(serverAddress));

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverFd < 0) {
		fprintf(stderr, "ERROR: Failed to create server socket :'(\n");
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Debug: %d\n", PORTNO_H);

	serverAddress.sin_port = htons(PORTNO_H);
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	if(bind(serverFd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
		int err = errno;
		fprintf(stderr, "ERROR: Failed to bind to port [ERRNO: %d] :'(\n", errno);
		exit(EXIT_FAILURE);
	}

	listen(serverFd, 10);

	int cliLen = sizeof(clientAddress);
	while(true) {
		int clientFd = accept(
		    serverFd,
		    (struct sockaddr*) &clientAddress,
		    (socklen_t*) &cliLen
		);

		char remoteIp[INET_ADDRSTRLEN];

		inet_ntop(
		    AF_INET,
		    &((struct sockaddr_in*) &clientAddress)->sin_addr.s_addr,
		    remoteIp,
		    INET_ADDRSTRLEN
		);

		if(clientFd < 0) {
			fprintf(stderr, "Could not Establish Connection Successfully :'(\n");
		}

		fprintf(stdout, "Connection Established to: {%s}\n", remoteIp);

		std::thread(thd, clientFd).detach();
	}
}
