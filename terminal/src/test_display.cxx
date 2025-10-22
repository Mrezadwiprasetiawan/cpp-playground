#include <display-term.hxx>

int main() {
  using namespace std;
  Display& disp = Display::getInstance();

  int w = disp.width();
  int h = disp.height();
  cout << "Available  display size in char = " << w << "x" << h << "\n";
}
