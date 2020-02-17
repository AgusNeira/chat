CXX = g++
CXXFLAGS = -Wall

CLIENT_HS = bufferedendpoint.h endpoint.h pollmanager.h protocol.h user.h
CLIENT_GCHS = $(pathsubstr %.h, %.h.gch, $(CLIENT_HS))

client: client.o
	$(CXX) $(CXXFLAGS) -o client client.o

client.o: client.cpp user.h.gch protocol.h.gch pollmanager.h.gch bufferedendpoint.h.gch
	$(CXX) $(CXXFLAGS) -c -o client.o client.cpp

%.h.gch: %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<
