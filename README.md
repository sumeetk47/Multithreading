# FileIndexer

This project implements super simple Fileindexer. It displays top ten words with highest counts. This is a multithreaded code which spaws a single master thread and multiple consumer threads. Master thread parses directories to search text files and passes the system path of the text files found to the consumer threads using a queue. While doing this, consumer threads pick the file paths from the queue and processes that file to find word counts. This word counts are stored in global map by having key as the word and value as the count. After all the threads are done, this map is firstly sorted in descending order on the basis of values and top 10 values and respective words are printed. Word matching is case insensitive. Makefile is included with project for smooth compilation. This code has been tested with Linux Ubuntu.

Usage:
This is the way executable file needs to be spawned for smooth working:
./ssfi -t 3 ./path

Where -t parameter represents number of consumer threads to be run and './path' is the directory path.

Help functionality is also provided. Just type in './ssfi -h' to get the help.

Test cases :
This code has been tested for different test scenarios and also handles different error conditions. It checks the existence of directory and if any text files are present or not. Moreover, it also checks for error conditions in passing arguments. It separates the words on the basis of punctuations. So, any punctuational characters are treated as separators.  
