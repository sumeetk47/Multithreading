#include <iostream>
#include <thread>
#include <string>
#include <map>
#include <queue>
#include <mutex>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <cstdlib>
#include <fstream>


std::mutex m,n,queuelock,resultlock;

std::queue<std::string> exchange;
std::map<std::string,int> result_set;



int finished = false;
int finished2 = false;



void ListFilesRecursively(const char *dir, const char* ext)
{    
    boost::filesystem::recursive_directory_iterator rdi(dir);  
    boost::filesystem::recursive_directory_iterator end_rdi;
 
    std::string ext_str0(ext);
    std::string string_pointer;   
    for (; rdi != end_rdi; rdi++)
    {
        //rdi++;
         
        if (ext_str0.compare((*rdi).path().extension().string()) == 0)
        {
            //std::cout << (*rdi).path().string() << std::endl;
            string_pointer = (*rdi).path().string();
            queuelock.lock();
            exchange.push(string_pointer);  
            queuelock.unlock();         
        }
    }
    n.lock();
	finished2 = true;
	n.unlock();
}

void WordCount(int tid)
{
	while(true)
	{
		std::string i;
		std::string iterate;
		while(true)
		{
			queuelock.lock();
			if(exchange.size() > 0)
			{
				i = exchange.front();
				exchange.pop();
				queuelock.unlock();
				std::cout << "Got file : " << i << " with id = " << tid << std::endl;
				std::ifstream b_file (i);
				while(b_file>> iterate)
				{
				resultlock.lock();
				++result_set[iterate];
				resultlock.unlock();
				}
			}
			else
			{
				queuelock.unlock();
				break;
			}
		}
		n.lock();
		if(finished2)
		break;
		n.unlock();
	}
	n.unlock();

}

int main(int argc, char* argv[]) {
	int c;
	char* dir;
	int nthreads;
	while ((c = getopt (argc, argv, "t:")) != -1)
    switch (c)
      {
      case 't':
      try{
        nthreads = std::stoi(optarg);
        std::cout << nthreads;
    }
    catch(std::exception const & e)
    {
    	std::cout <<"Please enter integer:"
    	 << std::endl;
    	exit(0);
    }
        break;
      }
      if(argv[3] != NULL)
      {
      dir = argv[3];
  }
  else
  {
  	std::cout <<"Please enter directory:";
  	exit(0);
  }

      std::cout << nthreads << dir << std::endl;



	std::thread t[nthreads + 1];
	

 


   
int i = 1;


t[0] = std::thread(ListFilesRecursively, dir, ".txt");	
for(i=1; i< (nthreads + 1); i++)
{
t[i] = std::thread(WordCount, i);
}
t[0].join();




for(i=1; i< (nthreads + 1); i++)
{
t[i].join();
}


 std::map<std::string, int>::iterator it;
for (it = result_set.begin(); it != result_set.end(); it++)
{
    std::cout << it->first << ' ' << it->second << '\n';
}




}