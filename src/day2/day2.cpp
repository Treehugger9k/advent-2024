#include "assert.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <vector>

using codes = std::vector<std::vector<int>>;

constexpr char CODE_DELIMITER = ' ';

auto parse_lists(const char *filename) -> codes
{
    codes ret;
    const auto emplace = [&](std::vector<int> &code_list,
                             const std::string &line,
                             size_t first,
                             size_t last)
    {
        auto& num = code_list.emplace_back(0);
        std::from_chars(line.data() + first, line.data() + last, num);
        return line.find_first_of(CODE_DELIMITER, last + 1);
    };

    std::ifstream input { filename };
    for (std::string line; std::getline(input, line);)
    {
        auto &code_list = ret.emplace_back();
        size_t ixDelim = line.find_first_of(CODE_DELIMITER), ixOffset = 0;
        while (ixDelim != std::string::npos)
        {
            ixOffset = emplace(code_list, line, ixOffset, ixDelim);
            std::swap(ixOffset, ixDelim);
            ixOffset++;
        }
        if (ixOffset < line.size())
        {
            emplace(code_list, line, ixOffset, line.size());
        }
    }

    return ret;
}

/// @brief Returs if the code list is valid;
///        that is if the list is either all ascending or descending
///        and the distance of each adjacent pair is not greater than 3
///        but at least 1.
/// @param begin - begin iterator
/// @param end   - end iterator
/// @return end iterator if valid, the forward most iterator of a failed
///         pair otherwise
template <typename T>
auto codes_are_valid(T begin, T end) -> std::pair<bool, T>
{
    constexpr auto is_safe = [](int diff, bool direction) -> bool
    {
        constexpr int MAX_VAL = 4;
        return diff != 0 &&
               std::abs(diff) < MAX_VAL &&
               std::signbit(diff) == direction;
    };

    bool valid = true;
    auto it_last = begin;

    assert(begin < end);
    if (begin + 1 == end) { return std::make_pair(false, end); }

    const bool direction = std::signbit(*(begin + 1) - *begin);
    auto it = begin + 1;
    for (; it < end; it++)
    {
        const int diff = *it - *it_last;
        valid = is_safe(diff, direction);
        if (!valid) break;
        it_last = it;
    }

    return std::make_pair(valid, it);
}

auto puzzle1(const char *filename)
{
    auto codes = parse_lists(filename);
    size_t cSafeCodes = 0;
    for (const auto &code_list : codes)
    {
        const auto [valid, it] =
            codes_are_valid(code_list.cbegin(), code_list.cend());
        if (valid) { cSafeCodes++; }
    }
    return cSafeCodes;
}

auto puzzle2(const char *filename)
{
    auto codes = parse_lists(filename);
    size_t cSafeCodes = 0;
    for (auto &code_list : codes)
    {
        auto [valid, it] = codes_are_valid(code_list.begin(), code_list.end());
        if (valid) { cSafeCodes++; continue; }
        // Encountered unsafe list, check if within tolerance

        // Beginning may have different direction than end
        auto valid_it = codes_are_valid(code_list.begin() + 1, code_list.end());
        if (valid_it.first) { cSafeCodes++; continue;}

        const int erased_val = *it;
        const size_t ix = std::distance(code_list.begin(), it - 1);
        code_list.erase(it);
        valid_it = codes_are_valid(code_list.begin(), code_list.end());
        if (valid_it.first) { cSafeCodes++; continue; }

        code_list[ix] = erased_val;
        valid_it = codes_are_valid(code_list.begin(), code_list.end());
        if (valid_it.first) { cSafeCodes++; continue; }
    }
    return cSafeCodes;
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
