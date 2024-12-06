#include "assert.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using rule_pair = std::pair<int, int>;
using rule_map = std::unordered_map<int, std::vector<int>>;
using update = std::vector<int>;

constexpr char RULE_DELIMITER = '|';
constexpr char UPDATE_DELIMITER = ',';

auto parse_rule(const std::string &rule) -> rule_pair
{
    rule_pair parsed_rule {};
    const size_t ixDelim = rule.find(RULE_DELIMITER);
    if (std::string::npos == ixDelim) return parsed_rule;

    auto &[lhs, rhs] = parsed_rule;
    std::from_chars(rule.data(), rule.data() + ixDelim, lhs);
    std::from_chars(rule.data() + ixDelim + 1, rule.data() + rule.size(), rhs);
    return parsed_rule;
}

auto parse_update(std::istringstream &&update_stream) -> update
{
    update update_pages;

    for (std::string tok; std::getline(update_stream, tok, UPDATE_DELIMITER);)
    {
        auto &v = update_pages.emplace_back();
        std::from_chars(tok.data(), tok.data() + tok.size(), v);
    }
    return update_pages;
}

auto parse_file(const char *filename) ->
    std::pair<std::vector<rule_pair>, std::vector<update>>
{
    std::vector<rule_pair> rules {};
    std::vector<update> update_list {};
    std::ifstream input_file { filename };
    if (input_file.fail()) return {};

    for (std::string line; std::getline(input_file, line); )
    {
        if (std::string::npos != line.find(RULE_DELIMITER))
            rules.emplace_back(parse_rule(line));
        else if (line.size() > 0)
            update_list.emplace_back(parse_update(std::istringstream(line)));
    }
    return std::make_pair(rules, update_list);
}

auto update_is_valid(const update &update_order, const rule_map &rules) -> bool
{
    std::vector<int> tmp;
    tmp.reserve(update_order.size());

    for (const auto &update_val : update_order)
    {
        tmp.emplace_back(update_val);
        const auto it = rules.find(update_val);
        if (rules.end() == it) continue;

        for (const auto &bad_value : tmp)
        {
            const auto &deps = it->second;
            const auto itBad = std::find(deps.begin(), deps.end(), bad_value);
            if (deps.end() != itBad) return false;
        }
    }

    return true;
}

auto puzzle1(const char *filename) -> int
{
    auto [rules, updates] = parse_file(filename);
    rule_map rule_dependencies {};
    rule_dependencies.reserve(rules.size());
    for (const auto &[rule, dependency] : rules)
    {
        auto &dependencies = rule_dependencies[rule];
        dependencies.emplace_back(dependency);
    }

    int sum = 0;
    for (const auto &update_order : updates)
    {
        const bool valid = update_is_valid(update_order, rule_dependencies);
        if (valid)
        {
            const size_t ixMid = (update_order.size() - 1) / 2;
            sum += update_order[ixMid];
        }
    }

    return sum;
}

auto update_less_than(int lhs, int rhs, const rule_map &rules) -> bool
{
    const auto it = rules.find(lhs);
    if (rules.end() == it) return false;

    const auto &deps = it->second;
    const auto itLess = std::find(deps.begin(), deps.end(), rhs);
    return deps.end() != itLess;
}

auto puzzle2(const char *filename) -> int
{
    auto [rules, updates] = parse_file(filename);
    rule_map rule_dependencies {};
    rule_dependencies.reserve(rules.size());
    for (const auto &[rule, dependency] : rules)
    {
        auto &dependencies = rule_dependencies[rule];
        dependencies.emplace_back(dependency);
    }

    int sum = 0;
    for (auto &update_order : updates)
    {
        const bool valid = update_is_valid(update_order, rule_dependencies);
        if (valid) { continue; }

        std::sort(update_order.begin(), update_order.end(),
            [&](const auto &lhs, const auto &rhs)
            {
                return update_less_than(lhs, rhs, rule_dependencies);
            });

        assert(update_is_valid(update_order, rule_dependencies));
        const size_t ixMid = (update_order.size() - 1) / 2;
        sum += update_order[ixMid];
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
