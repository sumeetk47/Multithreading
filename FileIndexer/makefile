CC=gcc
CXX=g++  -std=c++11
RM=rm -f
CPPFLAGS=-g
LDLIBS=-lboost_filesystem -lboost_system -pthread
all: fileindexer
fileindexer: $(OBJS) 
	$(CXX) $(LDFLAGS) -o ssfi Main.cpp FileIndexer.cpp FileIndexer.h $(LDLIBS)
clean:
	$(RM) $(OBJS)
Main.o: 
	Main.cpp
FileIndexer.o:
	FileIndexer.cpp
distclean: 
	clean $(RM) ssfi

