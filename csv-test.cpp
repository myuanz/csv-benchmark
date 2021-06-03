// csv-test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <celero/Celero.h>
#include <random>
#include <cassert>
#include <string>
#include <regex>

#include "vince-csv-parser.h"
#include <csv.h>
#include <rapidcsv.h>
#include <boost/iostreams/device/mapped_file.hpp>

using std::string;

// CELERO_MAIN
int main(int argc, char** argv)
{
	celero::Run(argc, argv);
	return 0;
}
std::random_device RandomDevice;
std::uniform_int_distribution<int> UniformDistribution(0, 1024);


BASELINE(CSVTEST, Baseline, 3, 3) {
	boost::iostreams::mapped_file mmap("COX3.txt", boost::iostreams::mapped_file::readonly);
	auto f = mmap.const_data();
	auto last_pos = f;

	auto l = f + mmap.size();

	uint64_t res = 0;
	uint16_t x = 0, y = 0, UMI_count = 0;

	uintmax_t m_numLines = 0;

	int current_col = 0;

	while (f && f != l) {
		switch (*f)
		{
		case '\t':
			switch (current_col)
			{
			case 0:
				res += x;
				x = 0;
				break;
			case 1:
				y = 0;
				break;
			case 2:
				UMI_count = 0;
				break;
			}
			current_col++;
			break;
		case '\n':
			current_col = 0;
			m_numLines++;
			break;
		default:
			switch (current_col)
			{
			case 0:
				x = x * 10 + (*f - '0');
				break;
			case 1:
				y = y * 10 + (*f - '0');
				break;
			case 2:
				UMI_count = UMI_count * 10 + *f - '0';
				break;
			default:
				break;
			}
			break;
		}
		f++;
	}
	assert(res == 831502993036);
}


BENCHMARK(CSVTEST, VINCE_CSV, 3, 3) {
	using namespace csv;
	CSVReader reader("COX3.txt");
	uint64_t res = 0;

	for (CSVRow& row : reader) {
		res += row[0].get<uint16_t>();
	}
	assert(res == 831502993036);
}


BENCHMARK(CSVTEST, FAST_CSV_PARSER, 3, 3) {
	io::CSVReader<
		3,
		io::trim_chars<' '>,
		io::no_quote_escape<'\t'>,
		io::ignore_overflow
	> in("COX3.txt");
	in.set_header("x", "y", "UMI_count");
	uint16_t x, y, UMI_count;
	uint64_t res = 0;

	while (in.read_row(x, y, UMI_count)) {
		res += x;
	}
	assert(res == 831502993036);

}

BENCHMARK(CSVTEST, RAPIDCSV, 3, 3) {
	rapidcsv::Document doc(
		"COX3.txt",
		rapidcsv::LabelParams(-1, -1),
		rapidcsv::SeparatorParams('\t')
	);

	auto length = doc.GetRowCount();

	uint64_t res = 0;

	for (size_t i = 0; i < length; i++) {
		res += doc.GetCell<uint16_t>(0, i);
	}

	assert(res == 831502993036);
}

