/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, std::string> get_args(int argc, const char **argv) {
  using namespace std;
  std::unordered_map<string, string> res;
  for (int i = 1; i <= argc >> 1; ++i) res.insert({argv[(i << 1) - 1], argv[i << 1]});
  return res;
}