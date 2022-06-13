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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <signal.h>
#include <filesystem>


#include<dirent.h>
#include <queue>
#include <pthread.h>
#define perror2(s, e) fprintf(stderr, "%s: %s\n", s, strerror(e))

using namespace std;


void* com_thread(void*);
void* worker_thread(void*);
void sigchld_handler(int);

//mutex
pthread_mutex_t lock;


int sock;
queue<pair<string,int>> queue_run;
int queue_size;
int block_size;
int thread_pool_size;

int main(int argc, char *argv[])
{
    if (argc != 9)
    {
        perror("wrong arguments in dataserver");
        exit(EXIT_FAILURE);
    }

    lock = PTHREAD_MUTEX_INITIALIZER;

    int port;
    port = atoi(argv[2]);

    //παιρνω τα ορισματα
    thread_pool_size = atoi(argv[4]);


    queue_size = atoi(argv[6]);

    block_size = atoi(argv[8]);

    

    cout << "Server’s parameters are:" << endl;
    cout << "port: " << port << endl;
    cout << "thread_pool_size: " << thread_pool_size << endl;
    cout << "queue_size: " << queue_size << endl;
    cout << "Block_size: " << block_size << endl;
    cout << "Server was successfully initialized..." << endl;

    //δημιουργω το σοκετ
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct sockaddr *clientptr = (struct sockaddr *)&client;
    struct hostent *rem;

    //σημα για ctrl + c
    signal(SIGINT, sigchld_handler);
    
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    const int opt = 1;
    int err1;     
    // για να χρησιμοποιω το ιδιο port
    if((err1 = setsockopt(sock, SOCK_STREAM, SO_REUSEADDR, &opt, sizeof(opt))) == -1)
    {         
        perror("sock error");         
        exit(EXIT_FAILURE);     
    }

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* Listen for connections */
    if (listen(sock, 5) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    cout << "Listening for connections to port " << port << endl;


    //τα threads για workers τοσα οσα το thread_pool_size apo input
    pthread_t thread_table[thread_pool_size]; 
    
    //workers
    for(int i = 0; i < thread_pool_size; i++)
    {   
        int err;
        if (err = pthread_create(&thread_table[i], NULL, worker_thread, (void *) &sock))
        {
            perror2("pthread_create", err);
            exit(1);
        }
    }


    //socket
    int newsock;
    while (1)
    {
        clientlen = sizeof(client);

        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }


        //threads          
        pthread_t thr;
        int err;

        /*New Thread */
        if (err = pthread_create(&thr, NULL, com_thread, (void *) &newsock))
        {
            perror2("pthread_create", err);
            exit(1);
        }

    }

}

/* Wait for all dead child processes */
void sigchld_handler(int s)
{
    close(sock);
}

//Διαβαζω τα μονοπατια
void ReadDirectory( string dire, int newsocket)
{   

    DIR *directory;   // creating pointer of type dirent
    struct dirent *x; // pointer represent directory stream

    bool result = false; // declaring string variable and assign it to false.

    char* dir = new char[1000];


    for( int i = 0; i < dire.size(); i++)
    {
        dir[i] = dire.at(i);
    }
    dir[dire.size()] = '\0';

    //Ειναι αναδρομικη συναρτηση
    //αν ειμαι σε φακελο τοτε βλεπω τι εχει και καλω ξανα την συναρτηση
    if ((directory = opendir(dir)) != NULL)
    { 



        for(x=readdir(directory); x!=NULL; x=readdir(directory))
        {
            string name = x->d_name;
            if( name.rfind(".",0) == 0)
            {
                continue;
            }

            ReadDirectory( dire + "/" + x->d_name, newsocket);
        }

        closedir(directory); // close directory....
    }
    else
    {   
        cout <<"file edooo " << dire << endl;
        //αλλιως εισαγω τα μονοπατια στην ουρα αν χωρανε
        while (queue_run.size() > queue_size);
        
        pair<string,int> name_;
        name_.first = dire;
        name_.second = newsocket;
        queue_run.push( name_);
    }
}

//communication thread
void* com_thread(void* news)
{
    //buf
    char buf[1000];
    int newsocket = *(int*) news;
    //διαβαζω το ονομα του αρχειου και επεξεργασια
    if( read( newsocket, buf, 1000) < 0)
    {
        perror("error read com_thread");
        exit(EXIT_FAILURE);
    }
    cout << "From server, the directory:   " << buf << endl;
    //καλω την συναρτηση ReadDirectory που βρισκει τα μονοπατια για τα αρχεια 
    //μεσω των υπαρχων καταλογων
    //και τα βαζει στην ουρα
    ReadDirectory( buf, newsocket);


    pthread_exit(NULL);
}

//write in client tha pathname, block size and file size
//write the data from file
void Write( pair<string,int> inp)
{   

    //filename
    string name = inp.first;
    
    //socket
    int sock = inp.second;
    char* buf = new char[1000];
    for(int i=0; i < name.size(); i++)
    {
        buf[i] = name.at(i);
    }


    //το block_char ειναι για να μεταρεεψουμε το block size σε string 
    //μετα το pathname εχω "//" που μετα απο αυτο εχω πρωτα το block size
    //και μετα εχω το file size
    string block_char;
    block_char = to_string( block_size);
    int num = name.size() + block_char.size() + 2;
    buf[name.size()] = '/';
    buf[name.size() + 1] = '/';

    //create block size
    int j = 0;
    for(int i = name.size() + 2; i < num; i++)
    {   
        buf[i] = block_char.at(j);
        j++;
    }

    
    //to file char που ναι το file size σε string
    string file_char;
    int file_size = 0;
    //βρισκω το file size
    struct stat stat_buf;
    int rc = stat(name.c_str(), &stat_buf);
    if(rc == 0)
    {   
        file_size = stat_buf.st_size;
    }


    file_char = to_string( file_size);

    buf[num] = '/';
    int prev = num + 1;
    num  += file_char.size() + 1;
    j = 0;
    for(int i = prev; i < num; i++)
    {
        buf[i] = file_char.at(j);
        j++;
    }
    buf[num] = '/';
    buf[num + 1] = '\0';



    //write filename, blocksize and filesize
    //γραφω στο client να διαβασει το ονομα αρχειο μεσω path
    //μαζι με block_size and file_size
    if(write(sock, buf, 1000) < 0)
    {
        perror("error in write worker");
        exit(EXIT_FAILURE);
    }


    //write the data from file
    //το times εχει να κανει ποσες φορες θα γραψουμε ωστε να διαβασει ο client
    //for data
    int times = 1;
    if( file_size == 0)
    {
        times = 0;
    } 
    else
    {
        if( file_size >= block_size)
        {
            times = file_size/block_size;
            int checkn = file_size - times*block_size;
            if( checkn >0)
            {
                times++;
            }
        }

    }

    //bytes για καθε φορα που θα διαβαζουμε και θα γραφουμε
    int bytes = block_size;
    if( file_size < block_size)
    {
        bytes = file_size;
    }

    ifstream myFile (name, ios::in | ios::binary);

    while(times>0)
    {

        //read from file
        char buffer[bytes];
        myFile.read(buffer, bytes);
        

        //write in socket
        if( write(sock, buffer, bytes) < 0)
        {
            perror("error data in sock");
            exit(EXIT_FAILURE);
            
        }

        //ενημερωνω το τωρινο filesize που απομενει
        //και τα επομενα bytes
        file_size -= bytes;
        if( file_size > block_size)
        {
            bytes = block_size;
        }
        else
        {
            bytes = file_size;
        }
        times--;
    }

}

//Worker Thread
void* worker_thread(void* sock)
{
    while(1)
    {
    
        //αν η ουρα δεν ειναι αδεια
        while(!queue_run.empty())
        {
            
            if( queue_run.size() == 0)
                continue;

            //κλειδωνω
            pthread_mutex_lock(&lock);
            
            if( queue_run.size() == 0)
            {
                pthread_mutex_unlock(&lock);
                continue;
            }

            //παιρνω τα στοιχεια του πανω στοιχειου
            //αποθηκευω εκει το pathname & socket
            pair<string,int> name = queue_run.front();

            queue_run.pop();

            //γραφω το αρχειο αυτο           
            Write(name);
            //ξεκλειδωνω 
            pthread_mutex_unlock(&lock);
        }
    }

    pthread_exit(NULL);
}




