CXX=g++
MONGO_SOURCES=/Users/dane/projects
CXXFLAGS=-I$(MONGO_SOURCES) $(shell mapnik-config --cflags)
LDFLAGS=-L$(MONGO_SOURCES)/mongo $(shell mapnik-config --libs)

PLUGIN=mongo
OBJS=$(PLUGIN)_datasource.o $(PLUGIN)_featureset.o

$(PLUGIN).input:	$(OBJS)
	$(CXX) -dynamiclib $(LDFLAGS) -g -lmongoclient -undefined dynamic_lookup $(OBJS) -o $(PLUGIN).input

clean:
	rm -f $(PLUGIN).input
	rm -f $(OBJS)

$(PLUGIN)_datasource.o: $(PLUGIN)_datasource.cpp
$(PLUGIN)_featureset.o: $(PLUGIN)_featureset.cpp
