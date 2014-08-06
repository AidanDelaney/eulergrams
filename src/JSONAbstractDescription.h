#ifndef JSONABSTRACTDESCRIPTION_H
#define JSONABSTRACTDESCRIPTION_H

#include <sstream>
#include <memory>
#include <map>
#include <utility>
#include <cmath>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/iteration_macros.hpp>

#include <libcola/cola.h>

using std::stringstream;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
using namespace boost;

using namespace cola;

typedef adjacency_list < vecS,
  vecS,
  undirectedS,
  property<vertex_name_t, unsigned long>
  > AbstractDualGraph;

struct DrawableGraph {
  std::shared_ptr<RootCluster> root;
  int num_nodes;
};

class JSONAbstractDescription {
 public:
  JSONAbstractDescription(std::shared_ptr<stringstream>);
  std::shared_ptr<DrawableGraph> toDrawableGraph();

  std::pair<std::shared_ptr<AbstractDualGraph>, long> toAbstractDualGraph(std::shared_ptr<ptree>);
  std::shared_ptr<RootCluster> toClusterHeirarchy(std::shared_ptr<std::vector<unsigned long>> bvs, unsigned long num_contours);
 private:
  std::shared_ptr<ptree> propertyMap;
};

#endif // JSONABSTRACTDESCRIPTION_H
