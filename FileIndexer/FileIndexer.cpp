#include "FileIndexer.h"
#define TOP 10
	
//Locks for thread synchronization
std::mutex finishlock,queuelock,resultlock;
//Queue to pass messages from service thread to worker threads
std::queue<std::string> exchange;
//Global dataset to store wordcounts
std::map<std::string,int> result_set;
//Structure to check the validity of directory path entered
struct stat sb;
//Variable which is set when master thread completes searching for text files
int finished = false;

// trim from both ends (in place)
static inline void trim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	            std::not1(std::ptr_fun<int, int>(std::isspace))));
	    s.erase(std::find_if(s.rbegin(), s.rend(),
	            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

//Search for .txt files and pass the path onto consumer threads
void listFilesRecursively(const char *dir, const char* ext)
{    
	    boost::filesystem::recursive_directory_iterator rdi(dir);  
	    boost::filesystem::recursive_directory_iterator end_rdi;
	 	
	    std::string extension(ext);
	    std::string string_pointer;   
	    for (; rdi != end_rdi; rdi++)
	    {
	        if (extension.compare((*rdi).path().extension().string()) == 0)
	        {
	            //std::cout << (*rdi).path().string() << std::endl;
	            string_pointer = (*rdi).path().string();
	            queuelock.lock();
	            exchange.push(string_pointer);  
	            queuelock.unlock();       
	        }
	    }
	    finishlock.lock();
		finished = true;
		finishlock.unlock();
}

//Consumer threads generating word counts in the file
void wordCount()
{
		std::string filename;
		std::string iterate;
		std::size_t length;
		std::vector<std::string> words;
		std::vector<std::string>::iterator it;
		
		while(true)
		{
			while(true)
			{
				queuelock.lock();
				if(exchange.size() > 0)
				{

					filename = exchange.front();
					exchange.pop();
					queuelock.unlock();
					std::ifstream b_file (filename);
					while(b_file>> iterate)
					{
						length = iterate.size();
						for (std::size_t i=0; i<length; i++)
							{
	    						if (std::ispunct(iterate[i]))
	    							{
	        							iterate[i] = ' ';
	    							}
							}
							trim(iterate);
							std::transform(iterate.begin(), iterate.end(), iterate.begin(), ::tolower);
						boost::split(words, iterate, boost::is_any_of(" "), boost::token_compress_on);
						resultlock.lock();
						for (it = words.begin(); it != words.end(); it++)
							{
								if(*it != " ")
	    						++result_set[*it];
							}
						resultlock.unlock();
					}
				}
				else
				{
					queuelock.unlock();
					break;
				}
			}
			finishlock.lock();
			if(finished)
			break;
			finishlock.unlock();
		}
		finishlock.unlock();

}

//Print top 10 values from the shared map
void printTop10(){
		std::vector<std::pair<std::string, int>> pairs;
	for (auto it = result_set.begin(); it != result_set.end(); ++it)
	    pairs.push_back(*it);

	sort(pairs.begin(), pairs.end(), [=](std::pair<std::string, int>& a, std::pair<std::string, int>& b)
	{
	    return a.second > b.second;
	}
	);

	if(result_set.empty())
	{
		std::cout << "No text files found." << std::endl;
	}
	else{
	std::cout << "The top 10 words are:" << std::endl;
	if(pairs.size() < 10)
		for (size_t i = 0; i < pairs.size(); ++i) {
	    std::cout << pairs[i].first << "	" << pairs[i].second << "\n";
	}
	else
	for (size_t i = 0; i < TOP; ++i) {
	    std::cout << pairs[i].first << "	" << pairs[i].second << "\n";
	}
	}
}
