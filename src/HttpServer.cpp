/*
 * HttpServer.cpp
 *
 *  Created on: Mar 4, 2014
 *      Author: vert
 */
#include "../inc/HttpServer.hpp"
#include "../inc/MyFile.hpp"



void HTTPServer::onUrlRequested(MyString req, SOCKET sock) {
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
				path += "/";
			}

			if (f.isDirectory()) {
				#if defined(FULLDEBUG) || defined(DEBUG)
				mLog("Is a directory: " + path);
				#endif
				// if directory, implicitly add 'index.html'
				string header;
				header =  (string)"HTTP/1.1 200 OK\r\n"
						+ "Content-Type: text/html\r\n";

				string length = "Content-Length: ";

				string html_header = "<html><body>";
				// out all files here
				string files;
				getDirFiles(path, &files);

				string html_footer = "</body></html>\r\n\r\n";
				string data = html_header + files + html_footer;


				//count content-length.
				stringstream sstm;
				sstm << data.length();
				length += sstm.str() + "\r\n\r\n";

				data = header + length + html_header + files + html_footer;

				int n = write(sock, data.c_str(), data.length()+1);
				if (n < 0){
					mLog("ERROR writing to socket");
					exit(1);
				}
#ifdef FULLDEBUG
				mLog("Wrote: " + data);
#endif

			} else {
				try {
					// send files

					MyString temp;
					temp = (string)"HTTP/1.0 200 OK\r\n";
					temp += 		"Content-Type: " + guessContentType(path) + "\r\n";


					string data;
					parseFile(sock, path, &data); // send raw file

					//count content-length.
					string length = "Content-Length: ";
					stringstream sstm;
					sstm << data.length();
					length += sstm.str() + "\r\n\r\n";

					temp += length + data;


					int n = write(sock, temp.c_str(), temp.length());
					if (n < 0){
						mLog("ERROR writing to socket");
						exit(1);
					}
					#if defined(DEBUG) || defined(FULLDEBUG)
					mLog("200 OK");
					#endif
				} catch (...) {
					// file not found
					errorReport(sock, "404", "Not Found",
								"The requested URL was not found on this server.");
				}//try-catch
			}//else

		}//else
}

void HTTPServer::errorReport(int sock, string code, string title, string mesg) {
	string temp;
	temp = ("HTTP/1.0 " + code + " " + title + "\r\n" +
		   "\r\n" +
		   "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n" +
		   "<TITLE>" + code + " " + title + "</TITLE>\r\n" +
		   "</HEAD><BODY>\r\n" +
		   "<H1>" + title + "</H1>\r\n" + mesg + "<P>\r\n" +
		   "<HR><ADDRESS>FileServer 1.0 at " +
		   "127.0.0.1" +
		   " Port " + "8080" + "</ADDRESS>\r\n" +
		   "</BODY></HTML>\r\n");
	mLog("Sending: " + temp);
	write(sock, temp.c_str(), temp.length());
	mLog(code + " " + title);
}



const string HTTPServer::guessContentType(MyString path) const {
	if (path.endsWith(".html") || path.endsWith(".htm"))
		    return "text/html";
/*		else if (path.endsWith(".txt") || path.endsWith(".java"))
		    return "text/plain";
		else if (path.endsWith(".gif"))
		    return "image/gif";
		else if (path.endsWith(".class"))
		    return "application/octet-stream";
		else if (path.endsWith(".jpg") || path.endsWith(".jpeg"))
		    return "image/jpeg";*/
		else
		    return "text/plain";
}

int HTTPServer::parseFile(int sock, const string& path, string* data) const{
	//TODO: add code here
	ifstream file;
	file.open(path.c_str(), std::ifstream::binary);

	if(!file.good())
		throw("Bad file");

	file.seekg (0, file.end);
    int length = file.tellg();
	file.seekg (0, file.beg);

	char* temp = new char[length];

    // read data as a block:
	file.read(temp,length);
	file.close();

	*data = temp;
	delete[] temp;

	return length;
}

int HTTPServer::getDirFiles(const string& path, string* files) {
    DIR *dp;
    struct dirent *dirp;
    vector<string> filesVect;
    vector<string> dirVect;

    if((dp  = opendir(path.c_str())) == NULL) {
        mLog("Error opening dir", LOG_PERROR);
        exit(1);
    }

    while ((dirp = readdir(dp)) != NULL) {
    	if(dirp->d_type == DT_DIR)
    		// is a directory
    		dirVect.push_back(string(dirp->d_name));
    	else
    		//is file or link
    		filesVect.push_back(string(dirp->d_name));
    }

    closedir(dp);

    sort(dirVect.begin(), dirVect.end());
    sort(filesVect.begin(), filesVect.end());


    for(int i=0; i<dirVect.size(); i++){ // could write for(string s : filesVect) , but this is faster
    	*files += "<a href = \"" + path + dirVect[i] + "\">" + dirVect[i] + "</a>";
    	*files += "<br>";
    }
    for(int i=0; i<filesVect.size(); i++){ // could write for(string s : filesVect) , but this is faster
        *files += "<a href = \"" + path + filesVect[i] + "\">" + filesVect[i] + "</a>";
        *files += "<br>";
    }


    return filesVect.size() + dirVect.size();
}

HTTPServer::~HTTPServer() {
}

void HTTPServer::onIncomingConnection(SOCKET sock){

	//read data from socket
	char buffer[1024];
	int n;
	bzero(buffer, sizeof(buffer));

	n = read(sock,buffer,1024);
	if (n < 0)
	{
		mLog("ERROR reading from socket", LOG_PERROR);
		exit(1);
	}
	//mLog((string)"Read from buffer: " + buffer);

	MyString data(buffer);
	//data now contains incoming data.

	string firstLine;
	firstLine = data.substr(0, data.find("\r\n"));

	//we should check if header is correct
	const string prefix("GET");
	const string postfix("HTTP/1.");
	#ifdef FULLDEBUG
	mLog(("Data received: " + data).c_str());
	#endif

	if(firstLine.find(prefix) != 0 // doesn't start with prefix
			|| firstLine.find(postfix) != firstLine.length()-1 - postfix.length() // doesn't end with postfix
			|| firstLine.length() < 14){ // length is small
		// header is incorrect
		mLog("Bad request: " + firstLine);
		exit(1);
	} else {
		// header is correct
		MyString req = firstLine.substr(4, firstLine.find(postfix)-4);
		req.trim();
		#if defined(FULLDEBUG)
		mLog("request is:" + req);
		mLog("first line is:" + firstLine);
		#endif

		onUrlRequested(req, sock);
	}

	close(sock);
	lock();
	openConnCount--;
	unlock();


}




