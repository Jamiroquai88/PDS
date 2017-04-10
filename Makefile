CP=g++
LIBXMLFLAGS=`xml2-config --cflags`
LIBXMLLIB=`xml2-config --libs | sed -e 's@-lz@@g'`
CPFLAGS=-std=c++11 -pthread -pedantic -Wall
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: CPFLAGS += -O3
all: pds-scanner pds-chooser pds-spoof

debug: CPFLAGS += -DDEBUG -g
debug: pds-scanner pds-chooser pds-spoof

pds-scanner: pds-scanner.o errormsg.o interface.o pdsxmlparser.o protheader.o host.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

pds-chooser: pds-chooser.o errormsg.o pdsxmlparser.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm
	
pds-spoof: pds-spoof.o errormsg.o pdsxmlparser.o interface.o host.o spoofer.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

%.o:%.cpp
	$(CP) $(CPFLAGS) -c $< -o $@ -I/usr/local/include $(LIBXMLFLAGS) $(LIBXMLLIB) -L/usr/local/lib64 -lm

clean:
	$(RM) pds-scanner pds-chooser pds-spoof $(OBJ_FILES) *.h.gch
