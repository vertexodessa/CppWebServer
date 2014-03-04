/*
 * HttpServer.hpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */

#ifndef HTTPSERVER_HPP_
#define HTTPSERVER_HPP_


#include <string>
#include <algorithm>
#include <dirent.h>
#include "../inc/WebServer.hpp"
#include "../inc/MyString.hpp"

#include <iostream>
#include <fstream>
#include <sstream>


class HTTPServer : public WebServer{
private:

	int getDirFiles(string path, string* files);

public:

	HTTPServer(int port)
			:WebServer(port){};
	virtual ~HTTPServer(){};

//helpers
	void errorReport(SOCKET sock, string code, string title, string mesg);
	const string guessContentType(MyString path) const;
	int parseFile(SOCKET sock, string path, string* data) const;


//CALLBACKS
	void onUrlRequested(MyString url, SOCKET sock);
	virtual void onIncomingConnection(SOCKET sock);
};






#endif /* HTTPSERVER_HPP_ */
