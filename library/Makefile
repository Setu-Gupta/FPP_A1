CXX = g++
CXXFLAGS = -march=native -pedantic -Wall -Wextra -Werror -fPIC -std=c++11 -Wno-unused-parameter
DBGFLAGS = -Og -g3 -ggdb
RELFLAGS = -Ofast
LDFLAGS = -shared -lpthread

SRC = stamp.cpp
TARGET = libstamp.so 

.PHONY: release
release: $(TARGET)
$(TARGET): $(SRC)
	$(CXX) -I. $(CXXFLAGS) $(RELFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: debug
debug: $(TARGET)
$(TARGET): $(SRC)
	$(CXX) -I. $(CXXFLAGS) $(DBGFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(TARGET)

.PHONY: help
help:
	@echo "Usage: make [TARGET] [-j[num_threads]]"
	@echo "TARGET           : Specifies what build configuration to use. Default value is release"
	@echo "j                : Number of compilation jobs to run. Default value is 1. If num_threads is not specified, run number_of_cores+1 jobs"
	@echo "TARGET:"
	@echo " release         : Builds with all the optimizations enabled"
	@echo " debug           : Builds with debug information"
	@echo " clean           : Removes all the files that were built"
	@echo " help            : Displays this message"
