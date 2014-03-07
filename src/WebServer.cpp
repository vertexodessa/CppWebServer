

/**
 * Web Server application.
 *
 */

#include "../inc/WebServer.hpp"

volatile int WebServer::openConnCount =0;
pthread_mutex_t WebServer::mutex = PTHREAD_MUTEX_INITIALIZER;


void* threadFunc(void* data){
	WebServer* ws = (WebServer*) data;
	#if defined(FULLDEBUG) || defined(DEBUG)
	ws->mLog("In thread");
	#endif
	ws->onIncomingConnection(ws->newsockfd);
	return data;
}


void WebServer::mLog(const char* text, int level) {

	lock();
	string temp(text);
	temp += (daemonMode == 1)? " 1" : " 0";

	if(daemonMode == 1){
		syslog(level, temp.c_str());
	}else{
		cout << temp.c_str() << endl;
	}
	unlock();

}

WebServer::~WebServer(){

}

WebServer::WebServer(int port){
	this->mPort = port;
}

void WebServer::run(void){
    //SOCKET sockfd; // The socket descriptors
    unsigned int clilen;
    struct sockaddr_in serv_addr, cli_addr;
	#if defined(FULLDEBUG) || defined(DEBUG)
    mLog("Creating socket");
	#endif
    /* First call to socket() function */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		mLog("ERROR opening socket", LOG_PERROR);
		exit(1);
	}

	#if defined(FULLDEBUG) || defined(DEBUG)
	mLog("Filling info");
	#endif

	/* Initialize socket structure */
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(this->mPort);

	#if defined(FULLDEBUG) || defined(DEBUG)
	mLog("Binding socket");
	#endif

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0){
         mLog("ERROR on binding", LOG_PERROR);
         exit(1);
    }

    /* Now start listening for the clients, here process will
	 * go in sleep mode and will wait for the incoming connection
	 */
	#if defined(FULLDEBUG) || defined(DEBUG)
    mLog("Starting listening");
	#endif

    listen(sockfd,5);
	clilen = sizeof(cli_addr);


	while(true){
		#if defined(FULLDEBUG) || defined(DEBUG)
		mLog("Accept connection started");
		#endif

		/* Accept actual connection from the client */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		#if defined(FULLDEBUG) || defined(DEBUG)
        mLog("Socket accepted");
		#endif
        if (newsockfd < 0){
            mLog("ERROR on accept", LOG_PERROR);
            exit(1);
        }


        //if thread_count < MAX_THREADS , create thread with onIncomingConnection(newsockfd);
        pthread_t thread1;
		#if defined(FULLDEBUG) || defined(DEBUG)
        mLog("Creating thread");
		#endif
        if(openConnCount < MAX_THREADS){
        	lock();
        	openConnCount++;
        	unlock();
			if( pthread_create(&thread1, NULL, threadFunc, (void*) this) < 0 ){
			   mLog("could not create thread", LOG_PERROR);
			   exit(1);
			}
        } else {
        	lock();
        	openConnCount--;
        	unlock();
        }
	} /* end while */

}

void WebServer::init(int daemonMode) {
	mLog("Initialising web server");

	this->daemonMode = daemonMode;

	// fork if daemon == 1
	if(daemonMode == 1){
	    //Set our Logging Mask and open the mLog
	    setlogmask(LOG_UPTO(LOG_NOTICE));
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
	    //close(STDIN_FILENO);
	    //close(STDOUT_FILENO);
	    //close(STDERR_FILENO);

	}
}


