#include "LUT.h"
#include <iostream>
#include <sstream>

std::string LUT::ReadLine(std::ifstream& in, char lineSeperator) {
	const char Marker = '#';
	std::string Line("");
	while (Line.size() == 0 || Line[0] == Marker)
	{
		if (in.eof())
		{
			status = prematureend;
			break;
		}
		std::getline(in, Line, lineSeperator);
		if (in.fail()) {
			status = readerror;
			break;
		}
	}

	return Line;
}

std::vector<float> LUT::Parse(const std::string& text_) {
	int i = 3;
	tablerow yeet(i);
	std::istringstream line(text_);
	for (int c = 0; c < i; c++) {
		line >> yeet[c];
		if (line.fail()) {
			status = parsefail;
			break;
		}
		return yeet;
	}
}

LUT::State LUT::SaveFile(std::ofstream& out) {
	if (status != ok) {
		return status;
	}
	const char space = ' ';
	const char quoteyboi = '"';
	if (title.size() > 0) {
		out << "TITLE" << space << quoteyboi << title << quoteyboi << std::endl;
	}
	out << "created by LUT.cpp" << std::endl;
	out << "DOMAIN_MIN" << space << Min[0] << space << Min[1] << space << Min[2] << std::endl;
	out << "DOMAIN_MAX" << space << Max[0] << space << Max[1] << space << Max[2] << std::endl;
	if (lut1d.size() > 0) {
		int i = lut1d.size();
		out << "LUT_1D_SIZE" << space << i << std::endl;
		for (int c = 0; c < i && out.good(); c++) {
			out << lut1d[c][0] << space << lut1d[c][1] << space << lut1d[c][2] << std::endl;
		}
	}
	else
	{
		int i = lut3d.size();
		out << "LUT_3D_SIZE" << space << i << std::endl;
		for (int Blue = 0; Blue < i && out.good(); Blue++) {
			for (int Green = 0; Green < i && out.good(); Green++) {
				for (int Red = 0; Red < i && out.good(); Red++) {
					out << lut3d[Red][Green][Blue][0] << space << lut3d[Red][Green][Blue][1] << space << lut3d[Red][Green][Blue][2] << std::endl;
				}
			}
		}
	}
	out.flush();
	return (out.good() ? ok : writeerror);
}

LUT::State LUT::LoadFile(std::ifstream& in) {
	status = ok;
	title.clear();
	Min = tablerow(3, 0.0);
	Max = tablerow(3, 1.0);
	lut1d.clear();
	lut3d.clear();

	const char newL = '\n';
	char seperator = newL;

	const char returncharacter = '\r';
	for (int c = 0; c < 255; c++) {
		char co = in.get();
		if (co == newL) {
			break;
		}
		if (c > 250) {
			status = lineerror;
			break;
		}
	}
	in.seekg(0);
	in.clear();

	int i = 0, Title = 0, Size = 0, Mini = 0, Maxi = 0;

	while (status == ok) {
		long linePos = in.tellg();
		std::string text_ = ReadLine(in, seperator);
		if (status != ok) {
			break;
		}
		std::istringstream line(text_);
		std::string key;
		line >> key;
		if ("+" < key && key < ":") {
			in.seekg(linePos);
			break;
		}
		else if (key == "TITLE" && Title++ == 0) {
			const char quote = '"';
			char start;
			line >> start;
			if (start != quote) {
				status = titlemissingquote;
				break;
			}
			std::getline(line, title, quote);
		}
		else if (key == "DOMAIN_MIN" && Mini++ == 0) {
			line >> Min[0] >> Min[1] >> Min[2];
		}
		else if (key == "DOMAIN_MAX" && Maxi++ == 0) {
			line >> Max[0] >> Max[1] >> Max[2];
		}
		else if (key == "LUT_1D_SIZE" && Size++ == 0) {
			line >> i;
			if (i < 2 || i > 65536) {
				status = sizenotinrange;
				break;
			}
			lut1d = table1d(i, tablerow(3));
		}
		else if (key == "LUT_3D_SIZE" && Size++ == 0) {
			line >> i;
			if (i < 2 || i > 256) {
				status = sizenotinrange;
				break;
			}
			lut3d = table3d(i, table2d(i, table1d(i, tablerow(3))));
		}
		else
		{
			status = unknownkeyword;
			break;
		}
		if (line.fail()) {
			status = readerror;
			break;
		}
	}
	if (status == ok && Size == 0) {
		status = sizenotinrange;
	}
	if (status == ok && (Min[0] >= Max[0] || Min[1] >= Max[1] || Min[2] >= Max[2])) {
		status = domainreversed;
	}
	if (lut1d.size() > 0) {
		i = lut1d.size();
		for (int Blue = 0; Blue < i && status == ok; Blue++) {
			for (int Green = 0; Green < i && status == ok; Green++) {
				for (int Red = 0; Red < i && status == ok; Red++) {
					lut3d[Red][Green][Blue] = Parse(ReadLine(in, seperator));
				}
			}
		}
	}
	return status;
}