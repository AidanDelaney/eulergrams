#include <memory>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "JSONAbstractDescription.h"

namespace bpo = boost::program_options;

enum errors {E_NO_OPTIONS=400, E_OPTION_REQUIRED};

int main(int argc, char ** argv) {
  bpo::options_description od("Available options");
  od.add_options()
    ("help,h", "help message")
    ("json,j", bpo::value<std::string>(), "input JSON file");

  bpo::variables_map vm;
  try {
      bpo::store(bpo::parse_command_line(argc, argv, od), vm);
  } catch(const std::exception& ex) {
      std::cout << "Error checking program options: " << ex.what() << std::endl;
      std::cout << od << std::endl;
      return E_NO_OPTIONS;
  }
  bpo::notify(vm);

  if(!vm.count("json")) {
    std::cout << "A JSON file must be specified" << std::endl;
    std::cout << od << std::endl;
    return E_OPTION_REQUIRED;
  }

  std::string fname = vm["json"].as<std::string>();
  std::ifstream file(fname);
  if (file) {
    std::shared_ptr<std::stringstream> buffer = std::make_shared<std::stringstream>();
    *buffer << file.rdbuf();
    file.close();

    JSONAbstractDescription jad(buffer);
    auto dg = jad.toDrawableGraph();

    // We need one node per zone i.e. vertex in our graph
    std::vector<vpsc::Rectangle *> nodes(dg->num_nodes);
    // init nodes: Give nodes some initial position
    const double width = 5;
    const double height = 5;
    double pos = 0;
    for(int i=0; i< dg->num_nodes; i++) {
      vpsc::Rectangle * rect = new vpsc::Rectangle(pos, pos +width, pos, pos +height);
      nodes[i] = rect;

      // XXX randomness is needed because COLA doesn't currently untangle
      // the graph properly if all the nodes begin at the same position.
      pos += (rand() % 10) - 5;
    }

    EdgeLengths eLengths;
    std::vector<Edge> es;
    ConstrainedFDLayout alg(nodes, es, 10, true, eLengths);
    CompoundConstraints ccs;
    alg.setConstraints(ccs);

    UnsatisfiableConstraintInfos unsatisfiableX, unsatisfiableY;
    alg.setUnsatisfiableConstraintInfo(&unsatisfiableX, &unsatisfiableY);

    alg.setClusterHierarchy(dg->root);
    alg.makeFeasible();
    alg.run();
    alg.outputInstanceToSVG(fname);
  }
}
