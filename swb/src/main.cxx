#include <miner.hxx>
// #include <iostream>
#include <sha3-512.hxx>

int main(int argc, const char **argv) {
  using namespace std;
  Miner512 miner;
  miner.mine_concurrent(std::thread::hardware_concurrency());
  // SHA3_512 &sha3_512Dev = SHA3_512::get_instance();
  // if (argc < 2) cout << "input = " << endl << "output = " << sha3_512Dev.generate("").second << endl;
  // for (int i = 1; i < argc; ++i) {
  //   cout << "input = " << argv[i] << endl;
  //   cout << "output = " << sha3_512Dev.generate(argv[i]).second << endl;
  // }
  return 0;
}
