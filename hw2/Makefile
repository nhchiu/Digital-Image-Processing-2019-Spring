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
EXECUTABLES	:= prob1.exe prob2.exe
else
EXECUTABLES	:= prob1 prob2
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

$(BIN)/prob1 : $(SRC)/prob1.o $(SRC)/image.o
	@echo “Problem 1”
	@echo “compiling and linking the code”
	@if [ ! -d $(BIN) ]; then \
    	 mkdir $(BIN); \
	fi
	$(LD) $(LDFLAGS) $^ -o $@
	@echo “running the program, usage: prob1 [source_image_directory] [output_directory]”
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	./$(BIN)/prob1

$(BIN)/prob2: $(SRC)/prob2.o $(SRC)/image.o
	@echo “Problem 2”
	@echo “compiling and linking the code”
	@if [ ! -d $(BIN) ]; then \
    	 mkdir $(BIN); \
	fi
	$(LD) $(LDFLAGS) $^ -o $@
	@echo “running the program, usage: prob2 [source_image_directory] [output_directory]”
	@if [ ! -d ./result ]; then \
    	 mkdir result; \
	fi
	./$(BIN)/prob2

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) $^ -c -o $@
