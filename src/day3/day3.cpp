#include "assert.h"
#include <charconv>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>

auto parse_file(const char *filename) -> std::string
{
    std::ifstream input_file { filename };
    if (input_file.fail()) return "";
    input_file.seekg(0, std::ios_base::end);
    const size_t cBytes = input_file.tellg();
    std::string buf(cBytes, ' ');
    input_file.seekg(0);
    input_file.read(&buf[0], cBytes);
    return buf;
}

auto multiply_strings(const std::string &str1, const std::string &str2) -> int
{
    int mul_1 {}, mul_2 {};
    std::from_chars(str1.data(), str1.data() + str1.size(), mul_1);
    std::from_chars(str2.data(), str2.data() + str2.size(), mul_2);
    return mul_1 * mul_2;
}

auto puzzle1(const char *filename) -> int 
{
    int res {};
    std::string code = parse_file(filename);
    std::regex mul { "mul\\(([0-9]+),([0-9]+)\\)" };
    auto begin = std::sregex_iterator(code.begin(), code.end(), mul);
    auto end = std::sregex_iterator();
    for (std::sregex_iterator it = begin; it != end; ++it)
    {
        const std::smatch &match = *it;
        assert(3 == match.size());
        res += multiply_strings(match.str(1), match.str(2));
    }
    return res;
}

auto puzzle2(const char *filename) -> int
{
    int res {};
    std::string code = parse_file(filename);
    std::regex mul { "mul\\(([0-9]+),([0-9]+)\\)|do\\(\\)|don't\\(\\)" };
    auto begin = std::sregex_iterator(code.begin(), code.end(), mul);
    auto end = std::sregex_iterator();
    std::string mul_str;
    const std::string enable = "do()";
    const std::string disable = "don't()";
    bool enabled = true;
    for (std::sregex_iterator it = begin; it != end; ++it)
    {
        const std::smatch &match = *it;
        mul_str = match.str();
        if (enable == mul_str) enabled = true;
        else if (disable == mul_str) enabled = false;
        else if (enabled)
        {
            assert(3 == match.size());
            res += multiply_strings(match.str(1), match.str(2));
        }
    }
    return res;
}

int main(int argc, char *argv[])
{
    if (3 != argc)
    {
        std::cout << "Incorrect number of argumnets";
        return 1;
    }
    else if ('1' == argv[1][0])
    {
        std::cout << puzzle1(argv[2]);
    }
    else if ('2' == argv[1][0])
    {
        std::cout << puzzle2(argv[2]);
    }
    else
    {
        std::cout << "Unexpected argument";
        return 1;
    }

    return 0;
}