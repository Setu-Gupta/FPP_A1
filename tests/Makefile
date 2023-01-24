CXX = g++
CXXFLAGS = -march=native -pedantic -Wall -Wextra -Wno-unused-parameter
DBGFLAGS = -Og -g3 -ggdb
RELFLAGS = -Ofast
LDFLAGS = -lpthread -lstamp

SRC = src/stamp.cpp
ALLSRC = $(wildcard ./*.cpp)
RELTARGETS = $(patsubst ./%.cpp,release/%,$(ALLSRC))
DBGTARGETS = $(patsubst ./%.cpp,debug/%,$(ALLSRC))

.PHONY: release
release: $(RELTARGETS)
$(RELTARGETS): $(ALLSRC)
	$(CXX) -I./../src -L$(STAMP_PATH) $(CXXFLAGS) $(RELFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: debug
debug: $(DBGTARGETS)
$(DBGTARGETS): $(ALLSRC)
	$(CXX) -I./../src -L$(STAMP_PATH) $(CXXFLAGS) $(DBGFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf $(DBGTARGETS) $(RELTARGETS)

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
