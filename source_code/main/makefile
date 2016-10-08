CC	= g++
CPPFLAGS= -Wno-deprecated -O2 -c
LDFLAGS	= -O2
SOURCES	=  BCGDLink.cpp
OBJECTS	= $(SOURCES:.cpp=.o)
EXECUTABLE=BCGDEmbed

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	-rm -f *.o
