CXX = g++
CXXFLAGS = -Wall -std=c++1z -O2 -march=native


.PHONY: clean tt t

clean:
	find . | sed -e 's/\.cpp//' | sort | uniq -d | tee /dev/stderr | xargs rm

tt:
	time ./tests test/data.txt

t:
	./tests test/data.txt
