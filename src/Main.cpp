#include <memory>
#include <iostream>

#include "JSONAbstractDescription.h"

int main(int argc, char ** argv) {
  std::ifstream file("myFile");
  if (file) {
    std::shared_ptr<std::stringstream> buffer;
    *buffer << file.rdbuf();
    file.close();

    JSONAbstractDescription jad(buffer);
    std::cout << jad.toAbstractGraph() << std::endl;
  }
}
