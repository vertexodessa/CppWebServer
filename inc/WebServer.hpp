/*
 * WebServer.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#ifndef WEBSERVER_HPP_
#define WEBSERVER_HPP_

/**
 * Web Server application.
 *
 */

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>      // Needed for the socket functions
#include <syslog.h>
#include <strings.h>

using namespace std;

const static int MAX_THREADS=64;
const static char* DAEMON_NAME = "MYWEBSERVER";

// portability define
#define SOCKET int

class WebServer{
private:
	//disallow copying
	WebServer(WebServer&);

	bool daemonMode;
	int openConnCount;
	int mPort;
	vector <int> mThreads;

	void acceptConnection(SOCKET sock);
protected:
	void mLog(const char* text, int level =LOG_INFO);
	void mLog(string s){mLog(s.c_str());}; // overload

public:
	WebServer(int port);
	virtual ~WebServer();

	void init(int daemonMode);
	void run(void);
	virtual void onIncomingConnection(SOCKET sock){};

};



#endif /* WEBSERVER_HPP_ */
