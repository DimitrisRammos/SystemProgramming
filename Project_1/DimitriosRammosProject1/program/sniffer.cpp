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



#define MSIZE 100


using namespace std;

int main( int argc, char* argv[])
{   

////////////////
//
//pipe
////////////////

    int p[2];
    if( pipe(p) == -1)
    {
        perror("pipe call");
        exit(1);
    }

    



/////////////
///
///inotify
///
/////////////

    //καλω μια διεργασια
    //αν ειναι το παιδι -> καλω τον  listener
    //αν ειναι ο πατερας -> ειναι ο manager 
    pid_t pid_2;
    if( ( pid_2 = fork()) == 0)
    {   
        //αλλαζω την εξοδο να εκτυπωνει στο pipe που εχει δημιοργηθει
        dup2(p[1],1);

        char* args[] = {"inotifywait", "-m", "-q", "--format", "%f", "-e", "create", "-e","moved_to", "./Directory",NULL};
        
        //me execvp αλλαζω το σωμα του 'child'
        if( execvp( args[0], args) < 0 )
        {
            perror("Failed exevp");
            exit( EXIT_FAILURE);

        }
        

        exit(1);
    }
    else if( pid_2 < 0)
    {
        perror("Failed Fork");
        exit(EXIT_FAILURE);
    }

    //Αν ειναι ο πατερας συνεχιζω παρακατω



    //Manager
    list< pid_t> l;
    int i = 1;
    while (1)
    {


        //Δημιουργω εναν named-pipe 
        //ftiaxno ena pathname
        //με ονομα p1,....,pn
        char buf[100] = {};
        int rsize = read(p[0],buf, MSIZE);
        

        char path[100] = "p";
        char num[10];
        
        sprintf(num, "%d", i);
            
        int k = 1;
        for(int j = 0; j < 10; j++)
        {
            path[k] = num[j];
            k++;
        }
        
        //create mkfifo
        if(  mkfifo(path, 0666) == -1)
        {
            if( errno != EEXIST)
            {
                perror("Failed Path");
                exit(EXIT_FAILURE);
            }
        }

    

        pid_t pid_1;

        //δημιουργω για καθε worker ενα named pipe
        //αρα για καθε αρχειο ενα worker
        if( (pid_1 = fork()) == 0)
        {   

            char* args[] = {(char*)"./Worker", path, NULL};
            if( execvp( args[0],args) < 0)
            {
                perror("Failed EXECL");
                exit(EXIT_FAILURE);       
            }

            exit(1);

        }
        else if (pid_1 > 0)
        {
            
            //ο manager ανοιγει το αρχειο και γραφει το 
            //buf που διαβαζει απο το listener
            //to buf apo listener einai ena onoma arxeiou
            //ayto to onoma grafo sto named-pipe pou exei dimioyrgithei
            //to grafo kai o worker to diavazei
            int p1 = open(path, O_WRONLY);

            char name[100] = {};
            int f = 0;
            for( int j = 0; j < 100; j++)
            {
                if( buf[j]!= ' ' && buf[j] != '\n' && buf[j] != '\0')
                {
                    name[j] =  buf[j];
                }
                else
                {
                    name[j] = '\0';
                    f = j;
                    break;
                }
            }
            
            if( write( p1, name, f) == 1)
            {
                perror("Failed");
                exit(EXIT_FAILURE);
            }

            l.push_back(pid_1);

            
        }
        
        else if( pid_1 < 0)
        {

            perror("Failed fork");
            exit(EXIT_FAILURE);
        }
        i++;
    }
    


    list< pid_t>::iterator itr;
    for( itr = l.begin(); itr != l.end(); ++itr)
    {
        pid_t p;
        p = *itr;
        if( waitpid( p, NULL,0) < 0)
        {
            perror("Waitpid failed");
        }

    }


    if( waitpid( pid_2, NULL,0) < 0)
    {
        perror("Waitpid failed");
    }



    exit(0);

}