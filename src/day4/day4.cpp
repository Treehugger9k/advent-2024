#include "assert.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

using grid = std::vector<std::string>;
static const std::string TARGET_WORD = "XMAS";

auto parse_file(const char *filename) -> grid
{
    std::ifstream input_file(filename);
    if (input_file.fail()) { return {}; }
    std::vector<std::string> ret {};
    for (std::string line; std::getline(input_file, line); )
        ret.emplace_back(std::move(line));
    return ret;
}

auto check_target(const grid &g, size_t ixRow, size_t ixCol) -> int
{
    assert(ixRow < g.size());
    assert(ixCol < g[ixRow].size());
    assert(TARGET_WORD.size() > 0);

    if (g[ixRow][ixCol] != TARGET_WORD[0]) return 0;

    size_t ixRowAhead = ixRow, ixRowBehind = ixRow;
    size_t ixColAhead = ixCol, ixColBehind = ixCol;
    std::array<int, 8> count { 0, 0, 0, 0, 0, 0, 0, 0 };
    for (size_t ixChar = 0; ixChar < TARGET_WORD.size(); ixChar++)
    {
        const char c = TARGET_WORD[ixChar];
        count[0] += static_cast<int>(c == g[ixRow][ixColAhead]);
        count[1] += static_cast<int>(c == g[ixRow][ixColBehind]);
        count[2] += static_cast<int>(c == g[ixRowAhead][ixCol]);
        count[3] += static_cast<int>(c == g[ixRowBehind][ixCol]);
        if (ixCol < g[ixRow].size() - 3 && ixRow < g.size() - 3)
            count[4] += static_cast<int>(c == g[ixRowAhead][ixColAhead]);
        if (ixCol >= 3 && ixRow < g.size() - 3)
            count[5] += static_cast<int>(c == g[ixRowAhead][ixColBehind]);
        if (ixCol < g[ixRow].size() - 3 && ixRow >= 3)
            count[6] += static_cast<int>(c == g[ixRowBehind][ixColAhead]);
        if (ixCol >= 3 && ixRow >= 3)
            count[7] += static_cast<int>(c == g[ixRowBehind][ixColBehind]);

        ixRowAhead = std::max<size_t>(ixRowAhead, ixRowAhead + 1);
        ixColAhead = std::max<size_t>(ixColAhead, ixColAhead + 1);
        ixRowBehind = std::min<size_t>(ixRowBehind, ixRowBehind - 1);
        ixColBehind = std::min<size_t>(ixColBehind, ixColBehind - 1);
        if (ixRowAhead >= g.size()) ixRowAhead = g.size() - 1;
        if (ixColAhead >= g[ixRow].size()) ixColAhead = g[ixRow].size() - 1;
    }

    int sum = 0;
    for (const auto c : count) sum += static_cast<int>(c == TARGET_WORD.size());
    return sum;
}

auto puzzle1(const char *filename) -> int
{
    int ret {};
    const grid g = parse_file(filename);
    // invariant: each row has the same number of columns
    for (size_t ixRow = 0; ixRow < g.size(); ixRow++)
    {
        for (size_t ixCol = 0; ixCol < g[ixRow].size(); ixCol++)
        {
            ret += check_target(g, ixRow, ixCol);
        }
    }
    return ret;
}

auto puzzle2(const char *filename) -> int
{
    int ret {};
    const grid g = parse_file(filename);
    // invariant: each row has the same number of columns
    const size_t last_row = g.size() - 1;
    const size_t last_col = g[0].size() - 1;
    for (size_t ixRow = 1; ixRow < last_row; ixRow++)
    {
        for (size_t ixCol = 0; ixCol < last_col; ixCol++)
        {
            if (g[ixRow][ixCol] != 'A') continue;

            int legs = 0;
            legs += g[ixRow - 1][ixCol - 1] == 'M' &&
                    g[ixRow + 1][ixCol + 1] == 'S';
            legs += g[ixRow - 1][ixCol - 1] == 'S' &&
                    g[ixRow + 1][ixCol + 1] == 'M';
            legs += g[ixRow + 1][ixCol - 1] == 'M' &&
                    g[ixRow - 1][ixCol + 1] == 'S';
            legs += g[ixRow + 1][ixCol - 1] == 'S' &&
                    g[ixRow - 1][ixCol + 1] == 'M';
            ret += (2 == legs);
        }
    }
    return ret;
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
        std::cout << puzzle1(argv[2]) << '\n';
    }
    else if ('2' == argv[1][0])
    {
        std::cout << puzzle2(argv[2]) << '\n';
    }
    else
    {
        std::cout << "Unexpected argument";
        return 1;
    }

    return 0;
}
