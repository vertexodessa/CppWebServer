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
#include "WebServer.hpp"

class HTTPServer : public WebServer{
private:
	//disallow copy and assign
	HTTPServer(HTTPServer&){};

public:
	HTTPServer(int port)
			:WebServer(port){};
	virtual ~HTTPServer(){};

	void errorReport(SOCKET sock, string code, string title, string mesg);
	MyString guessContentType(MyString path);
	void sendFile(SOCKET sock, ifstream file);

//CALLBACKS
	void onUrlRequested(MyString url, SOCKET sock);
	virtual void onIncomingConnection(SOCKET sock);
};






#endif /* HTTPSERVER_HPP_ */
