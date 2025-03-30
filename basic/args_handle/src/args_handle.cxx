#include <iostream>
#include <map>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::map<std::string, std::string> options;
  std::vector<std::string> positional_args;

  // Parsing command-line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    // Check if current argument is an option
    if (arg.size() > 1 && arg[0] == '-') {
      std::string option_name = arg.substr(1);

      // Check if option has value
      if (i + 1 < argc && argv[i + 1][0] != '-') {
        options[option_name] = argv[i + 1];
        i++; // Skip value in next iteration
      } else {
        options[option_name] = ""; // Option without value
      }
    } else {
      positional_args.push_back(arg);
    }
  }

  // Contoh penggunaan hasil parsing
  std::cout << "Options:\n";
  for (const auto &pair : options) {
    std::cout << "-" << pair.first << ": "
              << (pair.second.empty() ? "(no value)" : pair.second) << "\n";
  }

  std::cout << "\nPositional arguments:\n";
  for (const auto &arg : positional_args) {
    std::cout << arg << "\n";
  }

  return 0;
}
