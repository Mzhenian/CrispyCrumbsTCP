#overview
SRCS := WebServer/main.cpp
OUT := server.out
CPPFLAGS := -std=gnu++20 -pthread -g -fdiagnostics-color=always

all: main
.PHONY: all clean test

#trivial
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

#the build tree
main: $(OBJS)
	@g++ $(CPPFLAGS) -o $(OUT) $^

%.o : %.cpp
	@g++ $(CPPFLAGS) -c $< -o $@

test: 
	make
	./$(OUT) config.txt

clean:
	rm -f $(OUT) $(OBJS)
