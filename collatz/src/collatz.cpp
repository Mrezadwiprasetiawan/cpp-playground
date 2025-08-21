#include <args_handle.hxx>
#include <collatz_matrix.hxx>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

int main(int argc, const char **argv) {
  using namespace std;
  unordered_map<string, string> args;
  args                       = get_args(argc, argv);
  uint64_t highestOddStarter = 0, longestPath = 0, seed = 0;
  if (args.empty() || args.find("help") != args.end() || args.find("h") != args.end()) return 0;

  try {
    highestOddStarter = stoull(args.at("highestOddStarter"));
    seed              = stoull(args.at("seed"));
    longestPath       = stoull(args.at("longestPath"));
  } catch (const out_of_range &ofer) { cout << ofer.what() << endl; }

  try {
    CollatzMatrix          colMat(highestOddStarter, longestPath);
    vector<Vec4<uint64_t>> path = std::move(colMat.find_path(seed));

    if (args.find("debug") != args.end() && args.at("debug") == "true") colMat.printMatrices();

    for (auto val : path)
      cout << setw(10) << "Matrix index = " << val.w << setw(10) << "row = " << val.x << setw(10) << "col = " << val.y << setw(10) << "value = " << val.z
           << endl;
    cout << endl << "Real Path on the original collatz function =";
    for (auto val : path) cout << 2 * val.z - 1 << " ";
    cout << endl;
  } catch (const exception &e) { cout << e.what() << endl; }

  return 0;
}