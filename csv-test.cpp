// csv-test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <celero/Celero.h>
#include <random>
#include <cassert>
#include <string>
#include <regex>
#include <omp.h>
#include <unordered_set>
#include <set>

#include <folly/FBString.h>

#include "vince-csv-parser.h"
#include <csv.h>
#include <rapidcsv.h>
#include <boost/iostreams/device/mapped_file.hpp>

using std::string;


const char CSV_FILE_PATH[] = "COX3.txt";
const char CSV_FILE_WITH_CHAR_PATH[] = "V:/monkeyData/T33/T33-s.txt";


// CELERO_MAIN
int main(int argc, char** argv) {
    celero::Run(argc, argv);
    return 0;
}

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
        switch (*f) {
        case '\t':
            switch (current_col) {
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
            switch (current_col) {
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
BENCHMARK(CSVTEST, NAIVE_WITH_COMP, 3, 3) {
    const int thread_count = 16;

    omp_set_num_threads(thread_count);

    boost::iostreams::mapped_file mmap(CSV_FILE_WITH_CHAR_PATH, boost::iostreams::mapped_file::readonly);
    auto f = mmap.const_data() + sizeof("geneID\tx\ty\tUMICount");
    auto last_pos = f;

    auto l = f + mmap.size();

    std::unordered_set<::folly::fbstring> genes;
    uint64_t res = 0;
    uintmax_t m_numLines = 0;

#pragma omp parallel for
    for (int start_i = 0; start_i < thread_count; start_i++) {
        decltype(f) start = f + mmap.size() * start_i / thread_count; 
        decltype(f) end;

        if (start_i == thread_count - 1) {
            end = l;
        } else {
            end = start + mmap.size() / thread_count;
        }

        uint64_t inner_res = 0;
        uint16_t x = 0, y = 0, UMI_count = 0;
        uintmax_t inner_m_numLines = 0;

        int current_col = 0;

        std::unordered_set<::folly::fbstring> inner_genes;
        ::folly::fbstring* current_gene = new ::folly::fbstring("");

        while (start && start != end) {

            switch (*start) {
            case '\t':
                switch (current_col) {
                case 0:
                    if (!inner_genes.contains(*current_gene)) {
                        inner_genes.insert(*current_gene);
                    }
                    delete current_gene;
                    current_gene = new ::folly::fbstring("");
                    break;
                case 1:
                    res += x;
                    x = 0;
                    break;
                case 2:
                    y = 0;
                    break;
                case 3:
                    UMI_count = 0;
                    break;
                }
                current_col++;
                break;
            case '\n':
                current_col = 0;
                inner_m_numLines++;
                break;
            default:
                switch (current_col) {
                case 0:
                    current_gene->push_back(*start);
                    break;
                case 1:
                    x = x * 10 + (*start - '0');
                    break;
                case 2:
                    y = y * 10 + (*start - '0');
                    break;
                case 3:
                    UMI_count = UMI_count * 10 + *start - '0';
                    break;
                default:
                    break;
                }
                break;
            }
            start++;
        }
#pragma omp critical
        {
            genes.insert(inner_genes.begin(), inner_genes.end());
        }
    }
}
BENCHMARK(CSVTEST, NAIVE_WITH_CHAR, 3, 3) {
    boost::iostreams::mapped_file mmap(CSV_FILE_WITH_CHAR_PATH, boost::iostreams::mapped_file::readonly);
    auto f = mmap.const_data() + sizeof("geneID\tx\ty\tUMICount");
    auto last_pos = f;

    auto l = f + mmap.size();
    std::unordered_set<::folly::fbstring> genes;

    uint64_t res = 0;
    uint16_t x = 0, y = 0, UMI_count = 0;

    uintmax_t m_numLines = 0;

    int current_col = 0;

    ::folly::fbstring* current_gene = new ::folly::fbstring("");

    while (f && f != l) {

        switch (*f) {
        case '\t':
            switch (current_col) {
            case 0:
                if (!genes.contains(*current_gene)) {
                    genes.insert(*current_gene);
                }
                delete current_gene;
                current_gene = new ::folly::fbstring("");
                break;
            case 1:
                res += x;
                x = 0;
                break;
            case 2:
                y = 0;
                break;
            case 3:
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
            switch (current_col) {
            case 0:
                current_gene->push_back(*f);
                break;
            case 1:
                x = x * 10 + (*f - '0');
                break;
            case 2:
                y = y * 10 + (*f - '0');
                break;
            case 3:
                UMI_count = UMI_count * 10 + *f - '0';
                break;
            default:
                break;
            }
            break;
        }
        f++;
    }
}


BENCHMARK(CSVTEST, NAIVE_WITH_OMP, 5, 5) {
    const int thread_count = 16;

    omp_set_num_threads(thread_count);
    boost::iostreams::mapped_file mmap("COX3.txt", boost::iostreams::mapped_file::readonly);
    auto f = mmap.const_data();
    auto last_pos = f;

    auto l = f + mmap.size();
    uint64_t res = 0;
    uintmax_t m_numLines = 0;

#pragma omp parallel for
    for (int start_i = 0; start_i < thread_count; start_i++) {
        decltype(f) start = f + (mmap.size() / thread_count) * start_i;
        decltype(f) end;

        if (start_i == thread_count - 1) {
            end = l;
        } else {
            end = f + (mmap.size() / thread_count) * (start_i + 1);
        }
        uint16_t x = 0, y = 0, UMI_count = 0;
        uint64_t inner_res = 0;
        uintmax_t inner_m_numLines = 0;


        int current_col = 0;

        while (start && start != end) {
            switch (*start) {
            case '\t':
                switch (current_col) {
                case 0:
                    inner_res += x;
                    x = 0;
                    break;
                case 1:
                    y = 0;
                    break;
                case 2:
                    UMI_count = 0;
                    break;
                default:
                    break;
                }
                current_col++;
                break;
            case '\n':
                current_col = 0;
                inner_m_numLines++;
                break;
            default:
                switch (current_col) {
                case 0:
                    x = x * 10 + (*start - '0');
                    break;
                case 1:
                    y = y * 10 + (*start - '0');
                    break;
                case 2:
                    UMI_count = UMI_count * 10 + *start - '0';
                    break;
                default:
                    break;
                }
                break;
            }
            start++;
        }
#pragma omp critical 
        {
            res += inner_res;
            m_numLines += inner_m_numLines;
        }
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

