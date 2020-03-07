#pragma once

#include <string>
#include <vector>
#include <fstream>

class LUT {

public:
	typedef std::vector<float> tablerow;
	typedef std::vector<tablerow> table1d;
	typedef std::vector<table1d> table2d;
	typedef std::vector<table2d> table3d;

	enum State {
		ok = 0,
		notinitialized = 1,
		readerror = 2,
		writeerror = 3,
		prematureend = 4,
		lineerror = 5,
		unknownkeyword = 6,
		titlemissingquote = 7,
		domainreversed = 8,
		sizenotinrange = 9,
		parsefail = 10
	};

	State status;
	std::string title;
	tablerow Min;
	tablerow Max;
	table1d lut1d;
	table3d lut3d;

	LUT() {
		status = notinitialized;
	};
	State SaveFile(std::ofstream& out);
	State LoadFile(std::ifstream& in);
private:
	std::string ReadLine(std::ifstream& infile, char lineSeperator);
	tablerow Parse(const std::string& text_);
};