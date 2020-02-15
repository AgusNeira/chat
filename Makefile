CXX = g++
CXXFLAGS = -Wall

client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

client.o: client.cpp user.h.gch protocol.h.gch pollmanager.h.gch bufferedendpoint.h.gch
	$(CXX) $(CXXFLAGS) -c -o -include client.o 
