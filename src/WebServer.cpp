

/**
 * Web Server application.
 *
 */

#include "../inc/WebServer.hpp"

void WebServer::mLog(const char* text, int level) {

	if(daemonMode == 1){
		syslog(level, text);
	}else{
		cout << text << endl;
	}

}

WebServer::WebServer(int port) {
	this->mPort = port;
	openConnCount = 0;
}

void WebServer::run(void){
    SOCKET sockfd, newsockfd; // The socket descriptors
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;

    mLog("Creating socket");
    /* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		mLog("ERROR opening socket", LOG_PERROR);
		exit(1);
	}

	mLog("Filling info");

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(this->mPort);

	mLog("Binding socket");

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         mLog("ERROR on binding", LOG_PERROR);
         exit(1);
    }

    /* Now start listening for the clients, here process will
	 * go in sleep mode and will wait for the incoming connection
	 */

    mLog("Starting listening");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);


	while(true){


		/* Accept actual connection from the client */
        newsockfd = accept(sockfd,
                			(struct sockaddr *) &cli_addr, &clilen);
        mLog("Socket accepted");
        if (newsockfd < 0){
            mLog("ERROR on accept");
            exit(1);
        }
        /* Create child process */
        int pid = fork();
        ++openConnCount;
        if (pid < 0){
        	mLog("ERROR on fork");
        	exit(1);
        }
        if (pid == 0){
            /* This is the client process */
            close(sockfd);
            onIncomingConnection(newsockfd);
            --openConnCount;
            exit(0);
        }else{
            close(newsockfd);
        }


	} /* end while */

}

void WebServer::init(int daemonMode) {
	mLog("Initialising web server");

	this->daemonMode = daemonMode;

	// fork if daemon == 1
	if(daemonMode == 1){
	    //Set our Logging Mask and open the mLog
//	    setlogmask(LOG_UPTO(LOG_NOTICE));
	    openlog(DAEMON_NAME, LOG_CONS | LOG_NDELAY | LOG_PERROR | LOG_PID, LOG_USER);

		pid_t pid, sid;
	   //Fork the Parent Process
		mLog("Forking");
		pid = fork();

		if (pid < 0)
			exit(EXIT_FAILURE); //TODO: should throw exception instead

		//We got a good pid, Close the Parent Process
		if (pid > 0)
			exit(EXIT_SUCCESS);

		//Change File Mask
		umask(0);

		//Create a new Signature Id for our child
		sid = setsid();
		if (sid < 0)
			exit(EXIT_FAILURE);

	    //Close Standard File Descriptors
	    close(STDIN_FILENO);
	    close(STDOUT_FILENO);
	    close(STDERR_FILENO);

	}
}


