#ifndef _FI_H
#define _FI_H

	#include <iostream>
	#include <thread>
	#include <string>
	#include <map>
	#include <queue>
	#include <vector>
	#include <mutex>
	#include <boost/filesystem.hpp>
	#include <boost/foreach.hpp>
	#include <stdlib.h>
	#include <unistd.h>
	#include <ctype.h>
	#include <stdio.h>
	#include <cstdlib>
	#include <fstream>
	#include <boost/algorithm/string/classification.hpp>
	#include <boost/algorithm/string/split.hpp>
	#include <algorithm>
	#include <functional>
	#include <sys/stat.h>

	void listFilesRecursively(const char *dir, const char* ext);
	void wordCount();
	void printTop10();
	static inline void trim(std::string &s);
#endif