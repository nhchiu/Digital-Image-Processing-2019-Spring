CXX		:= g++
LD		:= g++
CXXFLAGS	:= -std=c++11 -Wall -Wextra -O
LDFLAGS	:= -std=c++11 -Wall -Wextra -O

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib
LIBRARIES	:=

ifeq ($(OS),Windows_NT)
EXECUTABLES	:= warmup.exe prob1.exe prob2.exe
else
EXECUTABLES	:= warmup prob1 prob2
endif

CSOURCES	:= $(wildcard $(SRC)/*.c)
CPPSOURCES	:= $(wildcard $(SRC)/*.cpp)
OBJECTS	:= $(CSOURCES:.c=.o) $(CPPSOURCES:.cpp=.o)

all : $(addprefix $(BIN)/, $(EXECUTABLES))

.PHONY : clean
clean:
	$(RM) $(OBJECTS) $(addprefix $(BIN)/, $(EXECUTABLES))

run: all
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	@for file in $(EXECUTABLES); do \
		./$(BIN)/$$file; \
	done

$(BIN)/warmup : $(SRC)/warmup.o
	@echo “Warm-up”
	@echo “compiling and linking the code”
	@if [ ! -d $(BIN) ]; then \
    	 mkdir $(BIN); \
	fi
	$(LD) $(LDFLAGS) $(SRC)/warmup.o -o $(BIN)/warmup
	@echo “running the program, usage: warmup [source_image_directory] [output_directory]”
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	./$(BIN)/warmup

$(BIN)/prob1 : $(SRC)/prob1.o
	@echo “Problem 1”
	@echo “compiling and linking the code”
	@if [ ! -d $(BIN) ]; then \
    	 mkdir $(BIN); \
	fi
	$(LD) $(LDFLAGS) $(SRC)/prob1.o -o $(BIN)/prob1
	@echo “running the program, usage: prob1 [source_image_directory] [output_directory]”
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	./$(BIN)/prob1

$(BIN)/prob2: $(SRC)/prob2.o
	@echo “Problem 2”
	@echo “compiling and linking the code”
	@if [ ! -d $(BIN) ]; then \
    	 mkdir $(BIN); \
	fi
	$(LD) $(LDFLAGS) $(SRC)/prob2.o -o $(BIN)/prob2
	@echo “running the program, usage: prob2 [source_image_directory] [output_directory]”
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	./$(BIN)/prob2

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) $^ -c -o $@