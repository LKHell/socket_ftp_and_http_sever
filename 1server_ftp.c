/*
*       Program:        server_base.c
*       Author:         Paul Girard Ph.D., DIM, UQAC
*		Modified by:	Li Kunhao 
*       Date:           TUT, Sept 2016
*		date modified:	*********************
*
*       Objective:      Show how to use a tcp socket 
*                       on a server program running on
*                       Sun/Solaris ; after the connection,
*                       this program receives a client message
*                       and sends back an acknowledge each time.
*
*       Options to compile and link on Solaris:
*       ===>    gcc server_base.c -lsocket -o server_base
*       Options to compile and link on Linux:
*       ===>    gcc server_base.c -o server_base
*
*       Execution:  server_base    no_port 
*                    (ex. server_base 5001)
*
*       Files to include
*/

#include <errno.h>
/* #include <strings.h> */
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h> 
#include <stdio.h>

/*
*       The following functions are used in this program
*
*               socket, bind, getsockname, listen, 
*               accept, read, write, close.
*/

#define TRUE 1

#define ACK "ACK"


main(argc,argv)
        int argc;
        char *argv[];           /* argv[0] pointer to program name
                                   argv[1] pointer to tcp port number*/
{
        int sock1, msgsock;     /* socket descriptors */                

		struct sockaddr_in server1;  /* structure used to assign a name
                                   to a socket sccording internet format*/
/*      size_t length;      for Solaris: #octets in the structure sockadr_in */ 
        int addrlen, length;    /* Linux: #octets in the structure sockadr_in */ 
        char buf[81];         /* buffer for transmission */
        int rval;               /* status code for read */
        int n;                  /* temporary */
        FILE *fptr;
        char filename[20];
        int soctets=0;
      
/*
*       1. Validation of the 2 parameters read on the command line
*/
        if (argc != 2)
        {
            printf("Call the program this way : server_base port_number\n");
            return 1;
        }
/*

/*
*       2. Socket creation parameters:          
*               - AF_INET is the internet format 
*               - SOCK_STREAM specifies a TCP type socket
*               - 0 specifies to use the default protocol.
*/
        sock1 = socket(AF_INET, SOCK_STREAM, 0);
        if (sock1 < 0) 
        {
                perror("Error in creating a TCP socket ");
                return 1;
        } 

/*
*       3. Specify the local part of the address :
*               1)the local port number in network format and
*               2)the local IP address.  INADDR_ANY is used on a server
*               because many ip addresses may be used on the same machine.
*
*       Client connections will be redirected to other temporary sockets.
*       Use your own port number.
*/
        server1.sin_family = AF_INET;           /* internet format       */
        server1.sin_addr.s_addr = INADDR_ANY;   /* under-specified address */
        server1.sin_port = htons(atoi(argv[1])); /* char port # ==>integer port #
                                                ====> network 16 bits format */
        if (bind(sock1, (struct sockaddr *) &server1, sizeof(server1)) < 0)
        {
                perror(" The function bind did not work properly");
                return 1;
        }

/*
*       4. Simple validation: find the name associated to this socket
*               and print its port number
*/
        length = sizeof(server1);
        if (getsockname(sock1, (struct sockaddr *)&server1, &length) < 0 )
        {
                perror("Error in getsockname()");
                return 1;
        }
        printf("The actual TCP port is #%d\n", ntohs(server1.sin_port));


/*
*       5. Fix the maximum number of clients waiting connection and 
        leave the server in passive mode
*/      
        listen(sock1, 5);
        puts("server ready for connection");

/*
*       6. Waiting for a client TCP connection
*
*       The second and third parameters are useless.
*       A new temporary socket (msgsock) is created 
*       for each client connection (msgsock).
*/
        do 
        {
/* Solaris
         msgsock = accept(sock1, (struct sockaddr *) 0, (size_t *) 0);
*/ 
	        msgsock = accept(sock1, (struct sockaddr *) 0, NULL);
	        puts("\n Client connection");
         if (msgsock == -1 ) perror("accept");
         else do 
        {
/*       7. Client message is read in a 1K octets buffer. rval returns
*           the number of octets received.
*/
              rval = read(msgsock, filename, 1024);
              
              if (rval <= 0)
              {
                printf("EOF , file  update \n");
                                  
                close(msgsock);
              }            
              else /* Printing of message and transmission of the acknowledge */
              {
                        filename[rval]='\0';
                        if((fptr=fopen(filename,"w")) == NULL)
                        {
                                perror("error opening this file");
                                perror("Error in transmitting the ACK");
                        }
                        else
                        {
                                soctets=0;
                                printf("=file name==>%s \n",filename);
                                if (write(msgsock,ACK,strlen(ACK)) < 0)
                                {
                                        perror("Error in transmitting the ACK");    
                                }
                                else {
                                do
                                {
                                        bzero(buf ,strlen(buf));
                                         //most important thing!
                                        rval = read(msgsock, buf, 81);
                                        if (rval <= 0)
                                        {
                                                printf("End of client file transfer \n");
                                                printf("File %s  a total of %d octets\n",filename,soctets); 
                                                close(msgsock);
                                        }
                                        else
                                        {
                                                buf[rval]='\0'; 
                                               // printf("rval is %d \n",rval);
                                                fputs(buf,fptr);
                                                soctets=soctets+strlen(buf);
                                               // printf("=====>%s\n",buf);
                                        }
                                }while(rval >0);
                                fclose(fptr);
                                }     
                        }
                    
                       
             //   buf[rval]='\0'; /* insert the end of character string*/
             //   printf("=====>%s\n", buf);
                       
              }
         }  while(rval > 0);   /* read until EOF data available */
         
         
        } while(TRUE);          /* my server never stops */

/*      This program will never end; so the close() will never be done.
*       If you cancel this program (kill) the O/S will close this socket automatically.
*       We would need a special command to stop this service
*/
        close(sock1);
        return 0;
}       /* end of server program  */
