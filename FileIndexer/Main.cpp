#include "FileIndexer.h"
#define EXT ".txt"

extern std::map<std::string, int> result_set;
extern struct stat sb;

int main(int argc, char* argv[])
{
	int opts, i;
	char* dir;
	int nthreads;
	while ((opts = getopt (argc, argv, "ht:")) != -1) 
	{
		switch (opts)
		{
		case 't':
			try 
			{
				nthreads = std::stoi(optarg);
			}
			catch (std::exception const & e)
			{
				std::cout << "Please enter integer value for number of threads. Try -h flag to know appropriate usage.\n"
				          << std::endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'h':
		{
			std::cout << "---Help---\n-t = number of threads\n Example use: ./ssfi -t 3 ./path\n";
			exit(0);
		}
		break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s -t nthreads dir\n",
			        argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	if (argc == 1)
	{
		std::cout << "---Help---\n-t = number of threads\n Example use: ./ssfi -t 3 ./path\n";
		exit(EXIT_FAILURE);
	}
	if (argv[3] != NULL)
	{
		dir = argv[3];
	}
	else
	{
		std::cout << "Please enter directory path. Try -h flag to know appropriate usage.\n";
		exit(EXIT_FAILURE);
	}
	if (stat(dir, &sb) == 0 && S_ISDIR(sb.st_mode))
	{
		std::thread t[nthreads + 1];
		i = 1;
		t[0] = std::thread(listFilesRecursively, dir, EXT);
		for (i = 1; i < (nthreads + 1); i++)
		{
			t[i] = std::thread(wordCount);
		}
		t[0].join();

		for (i = 1; i < (nthreads + 1); i++)
		{
			t[i].join();
		}
		printTop10();
	}
	else
	{
		std::cout << "Directory Not Found. Please use -h to know appropriate usage.\n";
		exit(EXIT_FAILURE);
	}
}