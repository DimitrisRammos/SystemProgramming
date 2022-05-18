#include <list>
#include <string>
#include <fstream>
#include <map>
#include <iterator>

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
#include <string.h>

using namespace std;

void readFile( string filename);
void parser( string line);
void writeFile( string file);


//map
//το κλειδι -> ειναι το λινκ
//το value -> ειναι ποσες φορες εμφανιζεται
map< string, int> map_with_link;
int main( int argc, char* argv[])
{   

    //περνουμε το file des
     
    int fd = open( argv[1], O_RDONLY);
    
    //διαβαζουμε το ονομα του αρχειου που πρεπει να το ανοιξουμε
    char buf[100] = {};
    int rsize = read(fd, buf, 100);
    

    //ελεγχουμε για λαθη στην αντιγραφη
    char name[100] = {};
    for( int j = 0; j < 100; j++)
    {
        if( buf[j]!= ' ' && buf[j] != '\n' && buf[j] != '\0')
        {
            name[j] =  buf[j];
        }
        else
        {
            name[j] = '\0';
            break;
        }
    }


    //καλουμε τις παρακατω συναρτησεις
    readFile( name);
    writeFile( name);
    exit(1);

}


void writeFile( string file)
{

    //grafo sto arxeio me kataliksi '.out'
    file.push_back('.');
    file.push_back('o');
    file.push_back('u');
    file.push_back('t');
    ofstream outfile (file);


    //grafo to map sto arxeio
    map< string, int>::iterator itr;
    for( itr = map_with_link.begin(); itr != map_with_link.end(); ++itr)
    {
        outfile << itr->first << " " << itr->second << endl;
    }
    
    
}

//Διαβαζουμε το αρχειο
void readFile( string filename)
{
    ifstream file_in;
    string dir = "Directory/";
    dir += filename;
    file_in.open( dir);

    //διαβαζω αν string
    string line;     
    while ( file_in >> line)
    {
        //καλω τον parser na ελενξεθ το λινκ αν ειναι σωστο
        parser(line);

    }
    
}


//ο παρσερ
void parser( string line)
{
    
    char ch;
    int size = line.length();

    //αν ειναι λινκ θα αρχιζει ετσι οπως την συμβολοσειρα key
    string key = "http://";
    string find = "";
    if(size > 7)
    {
        for(int i = 0; i < 7; i++)
        {
            ch = line.at(i);
            find.push_back(ch);
        }
    }
    

    if( key != find)
    {
        return;
    }
    
    //edo kratame to link
    string link = "";
    //take the link
    
    string start = "";
    
    int num = 7;
    for( int i = 7; i <11;i++)
    {
        ch = line.at(i);
        start.push_back(ch);
    }

    //an yparxei www.
    if(start == "www.")
    {
        num = 11;
    }

    char ch_end = '/';
    for(int i = num; i < size; i++)
    {
        ch = line.at(i);
        if( ch_end == ch)
        {
            break;
        }

        link.push_back(ch);
    }

    //chech the map
    auto search = map_with_link.find(link);
    if( search != map_with_link.end())
    {
        
        int num = search->second + 1;
        map_with_link[link] = num;
    }    
    else
    {
        map_with_link[link] = 1;
    }

}