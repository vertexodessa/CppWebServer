/*
 * HttpServer.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */
#include "../inc/HttpServer.hpp"
#include "../inc/MyFile.hpp"

void HTTPServer::onUrlRequested(MyString req, SOCKET sock) {
	//string req = request.substr(4, request.length()-9).trim();

	if (req.find("..")!=-1 ||
		req.find("/.ht")!=-1 || req.endsWith("~")) {
		// evil hacker trying to read non-wwwhome or secret file
		errorReport(sock, "403", "Forbidden",
					"You don't have permission to access the requested URL.");
	} else {
		MyString path = req;
		MyFile f(path);
		if (f.isDirectory() && !path.endsWith("/")) {
			// redirect browser if referring to directory without final '/'
			MyString temp;
			temp = "HTTP/1.0 301 Moved Permanently\r\n";
			temp += "Location: http://";
			temp += (string) "127.0.0.1" + ":" + "8080";
			temp += req.c_str();
			temp += "/\r\n\r\n";
			write(sock, temp.c_str(), temp.length());
			mLog("301 Moved Permanently");
		} else {
			if (f.isDirectory()) {
				// if directory, implicitly add 'index.html'
				//TODO: out all files here
			} else {
				try {
					// send files
					ifstream ifile;


					MyString temp;
					temp = (string)"HTTP/1.0 200 OK\r\n";
					temp += 		"Content-Type: " + guessContentType(path) + "\r\n";
					temp += (string)"Date: " + "01.01.2014" + "\r\n";
					temp += 		"Server: FileServer 1.0\r\n\r\n";
					write(sock, temp.c_str(), temp.length());

					sendFile(sock, ifile); // send raw file
					mLog("200 OK");
				} catch (...) {
					// file not found
					errorReport(sock, "404", "Not Found",
								"The requested URL was not found on this server.");
				}//try-catch
			}//else

		}//else
	}//else
}

void HTTPServer::onIncomingConnection(SOCKET sock){
	//read data from socket
	char buffer[1024];
	int n;
	bzero(buffer, sizeof(buffer));

	n = read(sock,buffer,255);
	if (n < 0)
	{
		mLog("ERROR reading from socket");
		exit(1);
	}

	MyString data(buffer);
	//data now contains incoming data.

	string firstLine;
	firstLine = data.substr(0, data.find("\r\n"));

	//we should check if header is correct
	const string prefix("GET");
	const string postfix("HTTP/1.1");

	mLog(("Data received: " + data).c_str());

	if(firstLine.find(prefix) != 0 // doesn't start with prefix
			|| firstLine.find(postfix) != firstLine.length() - postfix.length() // doesn't end with postfix
			|| firstLine.length() < 14){ // length is small
		// header is incorrect
		mLog("Bad request: " + firstLine);
		exit(1);
	} else {
		// header is correct
		MyString req = firstLine.substr(4, firstLine.find(postfix)-4);
		req.trim();

		mLog("request is:" + req);
		mLog("first line is:" + firstLine);

		onUrlRequested(req, sock);
	}



}




