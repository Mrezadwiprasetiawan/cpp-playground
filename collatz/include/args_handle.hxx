#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, std::string> get_args(int argc, const char **argv) {
  using namespace std;
  std::unordered_map<string, string> res;
  for (int i = 1; i <= argc >> 1; ++i) res.insert({argv[(i << 1) - 1], argv[i << 1]});
  return res;
}