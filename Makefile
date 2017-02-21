CP=g++
LIBXMLFLAGS=`xml2-config --cflags`
LIBXMLLIB=`xml2-config --libs | sed -e 's@-lz@@g'`
CPFLAGS=-std=c++11 -pthread -pedantic -Wall -O3 -g
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: pds-scanner

pds-scanner: $(OBJ_FILES)
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

%.o:%.cpp
	$(CP) $(CPFLAGS) -c $< -o $@ -I/usr/local/include $(LIBXMLFLAGS) $(LIBXMLLIB) -L/usr/local/lib64 -lm

clean:
	$(RM) pds-scanner $(OBJ_FILES) *.h.gch
