//?置socket?非阻塞!然后connet!?原socket再select!再connect
int connect_socket_timeout(int sockfd,char *dest_host, int port, int timeout)
{

	struct sockaddr_in address;

	struct in_addr inaddr;

	struct hostent *host;

	int  err, noblock=1 , connect_ok=0, begin_time=time(NULL);

	log_debug("connect_socket to %s:%d\n",dest_host,port);

	if (inet_aton(dest_host, &inaddr))
	{	//log_debug("inet_aton ok now gethostbyaddr %s\n",dest_host);

		memcpy(&address.sin_addr, &inaddr, sizeof(address.sin_addr));
	}

	else
	{
		log_debug("inet_aton fail now gethostbyname %s \n",dest_host);

		host = gethostbyname(dest_host);

		if (!host) {
		/* We can't find an IP number */
		log_error("error looking up host  %s : %d\n",dest_host,errno);
		return -1;
		}

		memcpy(&address.sin_addr, host->h_addr_list[0], sizeof(address.sin_addr));

	}

	address.sin_family = AF_INET;

	address.sin_port = htons(port);

	/* Take the first IP address associated with this hostname */

	ioctl(sockfd,FIONBIO,&noblock);

	if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0)
	{
		err = errno;
		if (err != EINPROGRESS)
		{
			log_error("connect = %d connecting to host %s\n", err,dest_host);
		}

		else
		{
			while (1) /* is noblocking connect, check it until ok or timeout */
			{
				connect(sockfd, (struct sockaddr *) &address, sizeof(address));
				err = errno;
				switch (err)
				{
					case EISCONN:   /* connect ok */
						connect_ok = 1;
						break;

					case EALREADY:  /* is connecting, need to check again */
						usleep(50000);
					    break;
				}
				if (connect_ok==1)
				{
					break;
				}

				if (connect_ok==-1)
				{
					log_notice ("connect failed try time =%d \n", (time(NULL) - begin_time) );
					break;
				}
				if ( (timeout>0) && ((time(NULL) - begin_time)>timeout) )
				{
     			  　 log_notice("connect failed, timeout %d seconds\n", (time(NULL) - begin_time));
				     break;
				}
			}
		 }
	  }
	　　else        /* Connect successful immediately        */
	  　{
		   connect_ok = 1;
	　　}

	  　　/** end of try connect */

	  　　return ((connect_ok==1)?sockfd:-1);

}
