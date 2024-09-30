CXXFLAGS = -lncurses -std=c++20 -w
CXX = g++

OBJ = linkedList.o Text.o

all: test

%.o  : %.cpp %.h
	g++ -c $< $(CXXFLAGS) -o $@


%.o  : %.cpp
	g++ -c $^ $(CXXFLAGS) -o $@

test: $(OBJ) test.cpp
	$(CXX) $^ $(CXXFLAGS) -g -D DEBUG -o $@

run: test
	./test a.txt 2>err

release: $(OBJ) test.cpp
	$(CXX) $^ $(CXXFLAGS) -o tedit



clean_txt:
	rm *.txt 

clean:
	- rm *.o
	- rm test
	- rm *.txt

gen_txt:
	- rm a.txt
	touch a.txt
	@i=1; while [ $$i -le 50 ]; do \
		echo "Iteration $$i" >> a.txt; \
		i=$$((i + 1)); \
	done