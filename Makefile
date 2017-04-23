CP=g++
LIBXMLFLAGS=`xml2-config --cflags`
LIBXMLLIB=`xml2-config --libs | sed -e 's@-lz@@g'`
CPFLAGS=-std=c++11 -pthread -pedantic -Wall -Wformat=0 -Wno-unused-result -Wno-unused-variable
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(CPP_FILES:.cpp=.o)

all: CPFLAGS += -O3
all: pds-scanner pds-chooser pds-spoof pds-massspoof pds-intercept

debug: CPFLAGS += -DDEBUG -g
debug: pds-scanner pds-chooser pds-spoof pds-massspoof pds-intercept

pds-scanner: pds-scanner.o errormsg.o interface.o pdsxmlparser.o protheader.o host.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

pds-chooser: pds-chooser.o errormsg.o pdsxmlparser.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm
	
pds-spoof: pds-spoof.o massspoofer.o errormsg.o pdsxmlparser.o interface.o host.o spoofer.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

pds-massspoof: pds-massspoof.o massspoofer.o errormsg.o spoofer.o host.o interface.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

pds-intercept: pds-intercept.o intercepter.o errormsg.o spoofer.o host.o interface.o
	$(CP) $(CPFLAGS) $^ -o $@ $(LIBXMLFLAGS) $(LIBXMLLIB)  -lm

%.o:%.cpp
	$(CP) $(CPFLAGS) -c $< -o $@ -I/usr/local/include $(LIBXMLFLAGS) $(LIBXMLLIB) -L/usr/local/lib64 -lm

clean:
	$(RM) pds-scanner pds-chooser pds-spoof pds-massspoof pds-intercept $(OBJ_FILES) *.h.gch

zip:
	zip xprofa00.zip *.cpp *.h Makefile xprofa00.pdf
