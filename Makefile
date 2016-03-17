
all: TxtBinConverter.cpp
	g++ TxtBinConverter.cpp -std=c++11 -o TxtBinConverter

clean:
	rm *.exe