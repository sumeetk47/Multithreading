	#include "FileIndexer.h"
	#define TOP 10

	std::mutex finishlock,queuelock,resultlock;
	std::condition_variable cv;
	std::queue<std::string> exchange;
	std::map<std::string,int> result_set;
	struct stat sb;
	int finished = false;

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
	    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
	            std::not1(std::ptr_fun<int, int>(std::isspace))));
	    s.erase(std::find_if(s.rbegin(), s.rend(),
	            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}
	void ListFilesRecursively(const char *dir, const char* ext)
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

	void WordCount(int tid)
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
					std::cout << "Got file : " << filename << " with id = " << tid << std::endl;
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

	void printTop10(){
		std::vector<std::pair<std::string, int>> pairs;
	for (auto it = result_set.begin(); it != result_set.end(); ++it)
	    pairs.push_back(*it);

	sort(pairs.begin(), pairs.end(), [=](std::pair<std::string, int>& a, std::pair<std::string, int>& b)
	{
	    return a.second > b.second;
	}
	);

	  for (size_t i = 0; i < TOP; ++i) {
	    std::cout << pairs[i].first << "	" << pairs[i].second << "\n";
	  }
	}
