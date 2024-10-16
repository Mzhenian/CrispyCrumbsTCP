#overview
SRCS := WebServer/main.cpp recommendationEngine.cpp jsonConverter.cpp
OUT := server.out
CPPFLAGS := -std=gnu++20 -pthread -g -fdiagnostics-color=always

all: main
.PHONY: all clean test JC_test

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

JC_test:
	g++ -std=gnu++20 -pthread -g -fdiagnostics-color=always ./tests/test_jsonConverter.cpp jsonConverter.cpp -o test.out && ./test.out