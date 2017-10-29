CC=g++
CFLAGS=-Wall -std=c++11
LD=g++
TARGETS=rorschach

all:	$(TARGETS)

rorschach:	rorschach.o map.o event_handler.o event_detector.o read_rules.o
	@echo "Linking $@..."
	@$(LD) -o $@ $^

%.o: %.cpp monitor.h
	@echo "Compiling $@..."
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:	
	@echo Cleaning...
	@rm -f $(TARGETS) *.o
