/*
*       Program:        client_base.c
*       Author:         Paul Girard Ph.D., DIM, UQAC
*		modified by:	Li Kunhao 
*       Date:           tjut Sept 2016
*		Date modified:	****************
*       Objective:      Show how to use a tcp socket
*                       on a client program running on
*                       a Unix server ; this program transmits
*                       a message to a server and receives
*                       an acknowledge each time.  A delay of
*                       one second is introduced between messages.
*
*       Options to compile on Solaris:          
*       ===>    gcc client_base.c -lsocket -lnsl -o client_base
*       Options to compile on Linux:          
*       ===>    gcc client_base.c -lnsl -o client_base
*
*       Execution: 3 parameters: program_name server_name server port_number
*       =========>  client_base server_machine port_number
*               ex. client_base dimensxcn1.uqac.ca 5---
*
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
#include <time.h>      
#include <stdio.h>         

/*
*       The following functions are used in this program
*
*               socket, gethostbyname, connect, 
*               read, write, close.
*/


main(argc,argv)
        int argc;
        char *argv[];                   /* argv[0] pointer to program name
                                           argv[1] pointer to server name
                                           argv[2] pointer to tcp port number*/
{
        int sock2, msgsock;             /* socket descriptor */
        struct sockaddr_in server2;     /* structure used to assign a name
                                   to a socket sccording internet format*/
        size_t length;                  /* #octets in structure sockadr_in */
        struct hostent *hp,*gethostbyname(); /* structure to associate the
                                        server name and its address */
        struct hostent hpstruct; /* structure to access the server address*/
        char buf[81];         /* message buffer*/ 
        int rval;               /* status code for read */
        int n, i;               /* temporary */
        FILE *fptr;
        char loca_filename[20];
        char remote_filename[20];
        char com[3];
/*          clock    */
        struct timespec time1, time2, temp; /* time_t tv_sec; seconds
                                                long tv_nsec; nanoseconds */
        double delay;
        int nline = 0;
        int noctets = 0;
/*
*       1. Validation of all (3) parameters read on the command line
*/
        if (argc != 3)
        {
          printf("Please call the program ==> client_base server_name port_number\n");
          return 0;
        }
/*
*       2. Socket creation parameters:          
*               - AF_INET is the internet format 
*               - SOCK_STREAM specifies a TCP type socket
*               - 0 specifies to use the default protocol.
*/
        sock2 = socket(AF_INET, SOCK_STREAM, 0);
        if (sock2 < 0) 
        {
                perror("Error in creating a TCP socket");
                return 0;
        }
/*
*       3. Get a pointer to the network server address structure.
*          The server name is taken from the command line (argv[1]):
*               ex.  "sunens.uqac.ca"
*/
        server2.sin_family = AF_INET;   /* internet format*/
        hp = gethostbyname(argv[1]);    /* pointer to the network structure
                                        of the server listening on port
                                        argv[1] */
        if (hp == 0)
        {
                fprintf(stderr, "%s: unknown host ===>", argv[1]);
                return 0;
        }
        hpstruct = *hp; /*  save  hostent data */

/*
*       4. Fill in the remote address part
*/
        server2.sin_family = hpstruct.h_addrtype;
        server2.sin_port = htons(atoi(argv[2]));  /* char port # ==>integer port #
                                                     ====> network 16 bits format */
        server2.sin_addr = * ((struct in_addr *) hpstruct.h_addr);

/*      5. Try a connection with the server
*/
         
        if (connect(sock2, (struct sockaddr *)&server2, sizeof(server2)) < 0)
        {
                perror("Error at connect time");
                return 0;
        }

/*
*       6. Transmit 10 messages to the server and 
*       receive an ackknowledge from the server each time.
*/
        printf("put loca_filename remote_filename \n");

        scanf("%s %s %s",com,loca_filename ,remote_filename);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1); /* read the timer1 nanosec*/
        
        if((fptr=fopen(loca_filename,"r")) == NULL )
        {
                perror("error opening this file");
                return 0;
        }
        else
        {
                
                printf("Client get file ,send filename to server \n ");
               // write(sock2,remote_filename, strlen(remote_filename));
              
                if(write(sock2, remote_filename,strlen(remote_filename)) < 0)
                        perror("Error in the transmission of message");
                else 
                { 

                        /*  read a message from the server */
                        
                        if ((rval = read(sock2,buf,1024) )< 0)
                                perror("Error in reading a message\n");
                        if (rval == 0)
                                printf("End of transmission\n");
                        else //if(buf[0] == 'A')
                        {

                               buf[rval]='\0'; 
                                printf("=test for ack ==>%s\n", buf);
                               
                                while(!feof(fptr))
                                {
                                // length2=fread(buf,1,1024,fptr); 
                                        fgets(buf,81,fptr);
                                        write(sock2,buf,strlen(buf));
                                        nline++;
                                        noctets=noctets+strlen(buf);
                                       // if(feof(fptr)) 
                                       // { 
                                       //         printf("end \n"); 
                                        //        break;
                                       // }
                                        // else
                                        // {
                                        //         write(sock2,buf,strlen(buf));
                                        //         //send(sock2, buf, length2, 0);
                                        // } 
                                }
                                
                        }
                }
        }
        
        
        //... ==> file transfer where lines sent and octets sent are saved        
        //if(read(sock2,,1024))
        //int length2 = 0; 
                // loop until eof
            
         
        
        // while((length2 = fread(buf, sizeof(char), 1024, fptr)) > 0) 
        // { 
        //         if(send(sock2, buf, length2, 0) < 0) 
        //         { 
        //         printf("Send File:%s Failed./n", loca_filename); 
        //         break; 
        //         } 
                
        // }
        

     // close the file 
        
        
       

               // ...
       

        /* Statistics */
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2); /* read the timer2 nanosec*/
         if ((time2.tv_nsec-time1.tv_nsec)<0)
         {
                temp.tv_sec = time2.tv_sec-time1.tv_sec-1;
                temp.tv_nsec = 1000000000+time2.tv_nsec-time1.tv_nsec;
        } else
        {
                temp.tv_sec = time2.tv_sec-time1.tv_sec;
                temp.tv_nsec = time2.tv_nsec-time1.tv_nsec; 
        }
        delay = (double)((temp.tv_sec * 1000000) + (temp.tv_nsec/1000));
        delay = (double)delay/1000;
        printf("File %s had %d lines and a total of %d octets\n",loca_filename, nline, noctets); 
        printf("Transfer rate : %9.3f MO/sec \t Transfer delay:%9.3f msec\n",(double)((noctets*0.9765)/(1024*delay)),delay);
        printf("File:%s Transfer Successful! \n",loca_filename); 
       
        printf("End of program\n");
        fclose(fptr);  
        close(sock2);
      //  puts("End of client request");
        return 0;;        /* end of client program */
}
