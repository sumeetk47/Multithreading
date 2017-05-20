CC=gcc
CXX=g++  -std=c++11
RM=rm -f
CPPFLAGS=-g
LDLIBS=-lboost_filesystem -lboost_system -pthread
all: fileindexer
fileindexer: $(OBJS) 
	$(CXX) $(LDFLAGS) -o tool Main.cpp $(LDLIBS)
clean:
	$(RM) $(OBJS)
Main.o: 
	Main.cpp
distclean: 
	clean $(RM) tool

