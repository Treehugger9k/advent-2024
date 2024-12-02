#include <assert.h>
#include <algorithm>
#include <charconv>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using list_pair = std::pair<std::vector<int>, std::vector<int>>;

auto parse_lists(const char *filename) -> list_pair
{
    list_pair ret {};
    auto &[left, right] = ret;
    std::ifstream input { filename };
    for (std::string line; std::getline(input, line);)
    {
        size_t ixDelim = line.find_first_of(' ');
        if (std::string::npos == ixDelim) continue;

        auto &lhs = left.emplace_back(0);
        auto &rhs = right.emplace_back(0);
        std::from_chars(line.data(), line.data() + ixDelim, lhs);
        while ((ixDelim + 1) < line.size() && ' ' == line[++ixDelim]);
        std::from_chars(line.data() + ixDelim, line.data() + line.size(), rhs);
    }

    return ret;
}

auto puzzle1(const char *filename) -> int
{
    auto [left_list, right_list] = parse_lists(filename);
    std::sort(left_list.begin(), left_list.end());
    std::sort(right_list.begin(), right_list.end());
    assert(left_list.size() == right_list.size());
    
    int sum = 0;
    for (size_t ix = 0; ix < left_list.size(); ix++)
    {
        sum += std::abs(right_list[ix] - left_list[ix]);
    }
    return sum;
}

auto puzzle2(const char *filename) -> int
{
    auto [left_list, right_list] = parse_lists(filename);
    int sum = 0;
    for (const auto &lhs : left_list)
    {
        const auto cNum = std::count(right_list.begin(), right_list.end(), lhs);
        sum += (lhs * cNum);
    }
    return sum;
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