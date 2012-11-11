PROG= mongoose
CFLAGS= -W -Wall -std=c99 -pedantic -O2 $(COPT) 
CXXFLAGS= -W -Wall -std=c++98 -pedantic -O2 $(COPT) 
MAC_SHARED= -flat_namespace -bundle -undefined suppress 
LINFLAGS= -ldl -pthread $(CFLAGS) 
LIB= _$(PROG).so 

linux:
	$(CXX) mongcpp.cpp $(CXXFLAGS) -shared -fPIC -fpic -o _mongcpp.so -ldl -pthread 
	$(CC) mongoose.c -ldl -shared -fPIC -fpic -o $(LIB) $(LINFLAGS) 
	$(CC) mongoose.c main.c -o $(PROG) $(LINFLAGS) 

