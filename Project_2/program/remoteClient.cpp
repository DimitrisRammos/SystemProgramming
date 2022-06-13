#include <iostream>

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <string.h>


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h> 


#include <sys/stat.h>
#include <string>

#include <bits/stdc++.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

void openFile(char*);


//Client
int main( int argc, char* argv[])
{
    if(argc != 7)
    {
        perror("wrong arguments in remoteclient");
        exit(EXIT_FAILURE);
    }


    //παιρνω τα ορισματα
    int server_ip;
    server_ip = atoi(argv[2]);

    int server_port;
    server_port = atoi(argv[4]);
    


    cout << "Client’s parameters are:" << endl;
    cout << "serverIP: " << server_ip << endl;
    cout << "port: " << server_port << endl;
    cout << "directory: " << argv[6] << endl;


    //ανοιγω το σοκετ
    char buf[1000];
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
    int sock;

    strcpy(buf, argv[6]);

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* Find server address */
    if ((rem = gethostbyname(argv[2])) == NULL)
    {
        herror("gethostbyname"); 
        exit(1);
    }


    server.sin_family = AF_INET;
    /* Internet domain */

    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(server_port);
    
    /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connecting to %s port %d\n", argv[1], server_port);

    //write the directory
    if (write(sock, buf, 1000) < 0)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }


    bool name_file = true;
    char filename[1000];
    string Filename;
    ofstream Myfile;
    int size_file = 0;
    int max_file_size;
    int block_size;
    int bytes = 1000;
    int times = 0;
    //ατερμον loop
    while(1)
    {

        if(read( sock, buf, bytes) < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }

        //το name_file δηλωνει αν διαβαζουμε pathname τωρα η οχι
        if(name_file == true)
        {
            // το buf tora einai tis morfis:
            //Server/folder/p1.txt//512/10/
            //..
            //filename = Server/folder/p1.txt
            //block_size = 512
            //file_size = 10
            name_file = false;
            int find_it = -1;
            for(int i = 0; i <  1000; i++)
            {
                if(buf[i] == '/' && buf[i+1] == '/')
                {
                    find_it = i;
                    break;
                }
            }

            //for filename
            for(int i = 0; i < find_it; i++)
            {
                filename[i] = buf[i];
            }
            filename[find_it] = '\0';

            //for block size and file size
            int first = -1;
            int second = -1;
            int third = -1;
            for(int i = (find_it+1); i < 1000; i++)
            {   
                if(buf[i] == '/')
                {
                    if(first == -1)
                    {
                        first = i;
                    }
                    else if( second == -1)
                    {
                        second = i;
                    }
                    else
                    {
                        third = i;
                        break;
                    }
                }
            }


            string block_char;
            
            //for block_size
            for( int i = (first + 1); i < second; i++)
            {
                block_char.push_back(buf[i]);
            }
            block_size = stoi(block_char);



            string file_char;
            //for file_size
            for( int i = (second + 1); i < third; i++)
            {   
                file_char.push_back(buf[i]);
            }
            max_file_size = stoi( file_char);


            Filename = "out/";
            for( int i = 0; i < find_it; i++)
            {
                Filename.push_back(filename[i]);
            }

            openFile( filename);
            size_file = 0;
            
            //βρισκω τα bytes kai to times
            bytes = block_size;
            if( max_file_size <= block_size)
            {
                bytes = max_file_size;
            }
            
        
            //how loops for take data from file
            if( max_file_size >= block_size)
            {
                times = max_file_size/block_size;
                int checkn = max_file_size - times*block_size;
                if( checkn >0)
                {
                    times++;
                }

            }
            else
            {   
                times = 1;
                if( max_file_size == 0)
                {
                    times = 0;
                }
            }
            
            //delete other files with this pathname
            remove(Filename.c_str());

            //open file
            Myfile.open(Filename);
        }
        else
        {
            //Αν ειναι τα δεδομενα 
            Myfile << buf;
            

            //change bytes
            if( max_file_size <= block_size)
            {
                max_file_size = 0;
            }
            else
            {
                max_file_size -= block_size; 
            }

            bytes = block_size;
            if( max_file_size <= block_size)
            {
                bytes = max_file_size;
            }
            times--;
        }


        //την επομενη φορα που θα διαβασει ο client
        //θα διαβασει pathname
        if(times == 0)
        {
            Myfile.close();
            Filename.clear();

            for(int i = 0; i < 1000; i ++)
            {
                filename[i] = '\0';
            }
            
            name_file = true;
            size_file = 0;
            block_size = 0;
            bytes = 1000;
        }


    }

    close(sock);   /* Close socket and exit */


}

//openFile
void openFile( char* path)
{
    string table[100];
    int size = 0;
    int check;

    char* ptr;
    ptr = strtok( path, "/");

    while (ptr != NULL)  
    {  
        table[size] = ptr;
        size++;
        ptr = strtok (NULL, "/");      
    }

    string start = "out/";

    //ελεγχω η δημιουργω του φακελους 
    char* f = new char[100];
    for(int i = 0; i < size - 1; i++)
    {
        char* buf = new char[100];
        string name = table[i];

        start = start + name + "/";
        check = mkdir( start.c_str(), 0777);
    }  
    
}