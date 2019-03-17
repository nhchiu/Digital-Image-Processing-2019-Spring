CXX		:= g++
CPPFLAGS	:= -std=c++11 -Wall -Wextra -O $(shell pkg-config --cflags --libs opencv4)

ifeq ($(OS),Windows_NT)
EXECUTABLES	:= raw2jpeg.exe raw2hist.exe
else
EXECUTABLES	:= raw2jpeg raw2hist
endif

all : $(EXECUTABLES)

.PHONY: clean
clean:
	$(RM) $(EXECUTABLES)

%.exe: %.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@

%: %.cpp
	$(CXX) $(CPPFLAGS) $^ -o $@
