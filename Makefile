CP=g++
LIBXML=`xml2-config --cflags`
CPFLAGS=-std=c++11 -pthread -pedantic -Wall -O3 -g $(LIBXML)
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: pds-scanner

pds-scanner: $(OBJ_FILES)
	$(CP) $(CPFLAGS) $^ -o $@ -lm

%.o:%.cpp
	$(CP) $(CPFLAGS) -c $< -o $@ -I/usr/local/include -L/usr/local/lib64 -lm

clean:
	$(RM) pds-scanner $(OBJ_FILES) *.h.gch
