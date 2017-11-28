
CXX = g++
CXXFLAGS = -Wall -W -s -O3
.PHONY: default run
default: run

run :
	${CXX} ${CXXFLAGS} *.cpp -o simulator

clean: 
	rm -f *.o simulator


CXX = g++
CXXFLAGS = -std=c++11 -Wall -W -s -O3
LIBRARIES = -lpthread
.PHONY: default run
default: run

run :
	${CXX} ${CXXFLAGS} ProcessAndScheduleFunctions.cpp scheduleAnalyzer.cpp ${LIBRARIES} -o simulator

clean: 
	rm -f *.o simulator
