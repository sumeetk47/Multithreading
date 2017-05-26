#include "FileIndexer.h"
#define TOP 10

//Locks for thread synchronization
std::mutex queuelock, resultlock;
//Queue to pass messages from service thread to worker threads
std::queue<std::string> exchange;
//Global dataset to store wordcounts
std::map<std::string, int> result_set;
//Structure to check the validity of directory path entered
struct stat sb;
//Variable which is set when master thread completes searching for text files
int finished = false;
//Condition variable to wait on to get notified by producer thread
std::condition_variable cv;

// trim from both ends (in place)
static inline void trim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

//Filter function to be passed to replace_if
bool filter(char ch )
{
	return (isalnum(ch) == 0);
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
			string_pointer = (*rdi).path().string();
			std::unique_lock<std::mutex> lock(queuelock);
			exchange.push(string_pointer);
			queuelock.unlock();
			cv.notify_one();
		}
	}
	std::unique_lock<std::mutex> lock(queuelock);
	finished = true;
	queuelock.unlock();
	cv.notify_all();
}

//Consumer threads generating word counts in the file
void wordCount()
{
	std::string filename;
	std::string iterate;
	std::map<std::string, int> local_map;
	std::vector<std::string> words;
	std::vector<std::string>::iterator it;
	while (true)
	{
		std::unique_lock<std::mutex> lock(queuelock);
		if (finished && exchange.size() == 0)
		{
			queuelock.unlock();
			break;
		}
		else
		{
			if (exchange.size() > 0)
			{
				filename = exchange.front();
				exchange.pop();
				queuelock.unlock();
				std::ifstream b_file (filename);
				while (b_file >> iterate)
				{
					replace_if(iterate.begin(), iterate.end(), filter, ' ');
					trim(iterate);
					std::transform(iterate.begin(), iterate.end(), iterate.begin(), ::tolower);
					boost::split(words, iterate, boost::is_any_of(" "), boost::token_compress_on);
					resultlock.lock();
					for (it = words.begin(); it != words.end(); it++)
					{
						++result_set[*it];
						//If local_map is used instead of global shared map
						//++local_map[*it];
					}
					resultlock.unlock();
				}
			}
			else
			{
				cv.wait(lock);
			}
		}
	}
	//Possible optimization by having consumer thread specific local map
	/*
	resultlock.lock();
	for (auto& it : local_map)
	{
		result_set[it.first] += it.second;
	}
	resultlock.unlock();
	*/
}

//Print top 10 values from the shared map
void printTop10()
{
	std::vector<std::pair<std::string, int>> pairs;
	if (result_set.empty())
	{
		std::cout << "No words found." << std::endl;
	}
	else
	{
		for (auto it = result_set.begin(); it != result_set.end(); ++it)
			pairs.push_back(*it);

		sort(pairs.begin(), pairs.end(), [ = ](std::pair<std::string, int>& a, std::pair<std::string, int>& b)
		{
			return a.second > b.second;
		}
		    );

		if (pairs.size() < 10)
		{
			std::cout << "The top " << pairs.size() << " words are:" << std::endl;
			for (size_t i = 0; i < pairs.size(); ++i)
				std::cout << pairs[i].first << "	" << pairs[i].second << "\n";
		}
		else
		{
			std::cout << "The top " << TOP << " words are:" << std::endl;
			for (size_t i = 0; i < TOP; ++i)
				std::cout << pairs[i].first << "	" << pairs[i].second << "\n";
		}

	}
}
