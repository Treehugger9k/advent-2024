#include "assert.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Vec2
{
  int x, y;
};

struct character
{
public:
  static constexpr char DIR_UP    = '^';
  static constexpr char DIR_RIGHT = '>';
  static constexpr char DIR_DOWN  = 'v';
  static constexpr char DIR_LEFT  = '<';

  enum class direction
  {
    up,
    down,
    left,
    right
  };

  character(int position, char cur) :
    m_position(position),
    m_dir(direction::up)
  {
    assert(is_character(cur));
    switch (cur)
    {
    case DIR_UP:    m_dir = direction::up;    break;
    case DIR_DOWN:  m_dir = direction::down;  break;
    case DIR_LEFT:  m_dir = direction::left;  break;
    case DIR_RIGHT: m_dir = direction::right; break;
    };
  }

  void turn_right()
  {
    switch (m_dir)
    {
    case direction::up:    m_dir = direction::right; break;
    case direction::down:  m_dir = direction::left;  break;
    case direction::left:  m_dir = direction::up;    break;
    case direction::right: m_dir = direction::down;  break;
    }
  }

  static auto is_character(char c) -> bool 
  { 
    return c == DIR_UP || c == DIR_RIGHT || c == DIR_DOWN || c == DIR_LEFT;
  }

  auto get_direction() const -> direction { return m_dir; }
  auto get_position() const -> int { return m_position; }
  auto set_position(int pos) { m_position = pos; }
private:
  int m_position;
  direction m_dir;
};

struct map_grid
{
public:
  static constexpr char VISITED_UD     = '|';
  static constexpr char VISITED_LR     = '-';
  static constexpr char VISITED_INT    = '+';
  static constexpr char NOT_VISITED    = '.';
  static constexpr char OBSTACLE       = '#';
  static constexpr char GRID_DELIMITER = '\n';

  map_grid(std::string &&map, int cCols) :
    m_map(std::move(map)),
    m_dimensions{ cCols, static_cast<int>(map.length()) / cCols } {}

  character extract_character() const
  {
    const auto it = 
      std::find_if(m_map.begin(), m_map.end(), character::is_character);
    assert(it != m_map.end());
    const size_t pos = std::distance(m_map.begin(), it);
    return character{ static_cast<int>(pos), *it };
  }

  template <typename F>
  void trace_character(character &cur, bool leave_trace, F &&f)
  {
    while (true)
    {
      const character::direction dir = cur.get_direction();
      const int jump = [&]() -> int
        {
          switch (dir)
          {
          case character::direction::left:  return  -1;
          case character::direction::right: return   1;
          case character::direction::up:    return -m_dimensions.x;
          case character::direction::down:  return  m_dimensions.x;
          }
        }();

      const int position = cur.get_position();
      const int destination = position + jump;
      
      f(m_map[position]);

      if (leave_trace)
      {
        if (NOT_VISITED == m_map[position])
        {
          switch (dir)
          {
          case character::direction::left:  m_map[position] = VISITED_LR; break;
          case character::direction::right: m_map[position] = VISITED_LR; break;
          case character::direction::up:    m_map[position] = VISITED_UD; break;
          case character::direction::down:  m_map[position] = VISITED_UD; break;
          }
        }

        if (VISITED_LR == m_map[position])
        {
          switch (dir)
          {
          case character::direction::up:    m_map[position] = VISITED_INT; break;
          case character::direction::down:  m_map[position] = VISITED_INT; break;
          default: break;
          }
        }

        if (VISITED_UD == m_map[position])
        {
          switch (dir)
          {
          case character::direction::left:    m_map[position] = VISITED_INT; break;
          case character::direction::right:   m_map[position] = VISITED_INT; break;
          default: break;
          }
        }

        // case if go off map vertically or horizontally at very beginning / end.
        if (destination < 0 || destination >= static_cast<int>(m_map.length()))
        {
          break;
        }

        // case if go left off map boundary
        if (0 == (position % m_dimensions.x) &&
            character::direction::left == dir)
        {
          break;
        }

        // case if go right off map boundary
        if (0 == ((position + 1) % m_dimensions.x) &&
            character::direction::right == dir)
        {
          break;
        }
      }

      if (OBSTACLE == m_map[destination])
      { // case if hit obstacle (turn right)
        cur.turn_right();
      }
      else 
      { // case if can move to destination
        cur.set_position(destination);
      }
    }
  }

  static auto is_visit_not_start(char c) -> bool 
  {
    return VISITED_LR == c || VISITED_UD == c || VISITED_INT == c;
  }

  auto visited_count() const -> int
  {
    return std::count_if(m_map.begin(), m_map.end(), [&](char c)
      {
        return is_visit_not_start(c) || character::is_character(c);
      });
  }

  auto buf() const -> const std::string& { return m_map; }
private:
  Vec2 m_dimensions{};
  std::string m_map;
};

auto parse_file(const char *filename) -> 
  std::optional<std::pair<character, map_grid>>
{
  std::ifstream input_file{ filename };
  if (input_file.fail()) { return std::nullopt; }
  
  input_file.seekg(0, std::ios::end);
  const size_t cBytes = input_file.tellg();
  input_file.seekg(0);
  std::string map_bytes;
  map_bytes.reserve(cBytes);

  std::string line;
  size_t cColumnLength = 0;
  for (std::string line; std::getline(input_file, line); )
  {
    map_bytes.append(line);
    cColumnLength = line.length();
  }

  assert(0 != cColumnLength);
  map_grid map{ std::move(map_bytes), static_cast<int>(cColumnLength) };
  character cur = map.extract_character();
  return std::make_pair(cur, std::move(map));
}

auto puzzle1(const char *filename) -> int
{
  auto cur_map = parse_file(filename);
  if (false == cur_map.has_value()) return 0;
  auto &[cur, map] = *cur_map;
  int sum = 0;
  map.trace_character(cur, true, [&](char c)
    {
      sum += static_cast<int>(map_grid::NOT_VISITED == c);
    });
  return sum + 1;
}

auto puzzle2(const char *filename) -> int
{
  auto cur_map = parse_file(filename);
  if (false == cur_map.has_value()) return 0;
  auto &[cur, map] = *cur_map;
  //map.trace_character(cur);
  //const auto &map_buf = map.buf();
  //for (const auto c : map_buf)
  //{
  //  // can only process points on path
  //  if (false == map_grid::is_visit_not_start(c)) continue;

  //}

  return 0;
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
