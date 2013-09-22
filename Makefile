PLUGIN = mongodb.input
CXX = g++

CXXFLAGS = $(shell mapnik-config --cflags) -fPIC
LDFLAGS = -shared $(shell mapnik-config --libs) -lmongoclient -lboost_thread-mt -lboost_filesystem -lboost_system

SOURCES := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))

all: $(PLUGIN)

$(PLUGIN): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@

clean:
	rm -f $(PLUGIN) $(OBJS)
