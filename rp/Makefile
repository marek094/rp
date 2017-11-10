CXX = g++
CXXFLAGS = -Wall -std=c++1z -O2 -march=native


.PHONY: clean tt

clean:
	ls | sed -e 's/\.cpp//' | sort | uniq -d | xargs rm

tt:
	time ./tests test/data.txt
