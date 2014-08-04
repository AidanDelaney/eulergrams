#ifndef JSONABSTRACTDESCRIPTION_H
#define JSONABSTRACTDESCRIPTION_H

#include <sstream>
#include <memory>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/foreach.hpp> // FIXME: can we get rid of this in C++11?

#include <libcola/cola.h>

using std::stringstream;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;
using namespace boost;

using cola::RootCluster;

typedef adjacency_list < vecS,
  vecS,
  undirectedS,
  property<vertex_index_t, unsigned long>
  > AbstractDualGraph;

class JSONAbstractDescription {
 public:
  JSONAbstractDescription(std::shared_ptr<stringstream>);
  std::shared_ptr<RootCluster> toAbstractGraph();
 private:
  std::shared_ptr<ptree> propertyMap;

  std::shared_ptr<AbstractDualGraph> toAbstractDualGraph(std::shared_ptr<ptree>);
};

#endif // JSONABSTRACTDESCRIPTION_H
