############################
# Author: Lukas Dudichum   #
# Project: HoloBotControl  #
# Date: 1.10.2012          #
############################

CC=g++ 
CFLAGS= -W -Wall -pthread -g -ldl -I.. -lpthread

holobotcontrol: grovemotordriver.o omnidrive.o liquidcrystal_i2c.o holobotcontrol.o
	$(CC) $(CFLAGS) grovemotordriver.o holobotcontrol.o omnidrive.o liquidcrystal_i2c.o mongoose/_mongcpp.so  mongoose/_mongoose.so -o HoloBotControl

grovemotordriver.o: grovemotordriver.cpp
	$(CC) $(CFLAGS) -c grovemotordriver.cpp

omnidrive.o: omnidrive.cpp
	$(CC) $(CFLAGS) -c omnidrive.cpp

liquidcrystal_i2c.o: liquidcrystal_i2c.cpp
	$(CC) $(CFLAGS) -c liquidcrystal_i2c.cpp

clean:
	rm -rf *o HoloBotControl