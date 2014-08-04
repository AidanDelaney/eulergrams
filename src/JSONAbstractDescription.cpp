#include "JSONAbstractDescription.h"

JSONAbstractDescription::JSONAbstractDescription(std::shared_ptr<stringstream> json) : propertyMap(new ptree) {
  read_json(*json, *propertyMap);
}

std::shared_ptr<RootCluster> JSONAbstractDescription::toAbstractGraph() {
  auto boost_graph = toAbstractDualGraph(propertyMap);
  return std::make_shared<RootCluster>();
}

unsigned long hammingDist(unsigned long x, unsigned long y) {
  unsigned dist = 0, val = x ^ y;

  while(val) {
    ++dist;
    val &= val - 1;
  }

  return dist;
}

std::shared_ptr<AbstractDualGraph> JSONAbstractDescription::toAbstractDualGraph(std::shared_ptr<ptree> propertyMap) {
  // Set a map from contour, as key, to index into bitvector.
  std::map<std::string, long> bv_index;

  // Get the contour array
  unsigned long index = 1;
  for (const auto& contours : propertyMap->get_child("AbstractDiagram.Contours")) {
    bv_index[contours.second.data()] = index;
    index *= 2;
  }

  // Now, each of our contours are represened by an individual bit in a long.
  // Thus, each zone can be represented as a long with specific bits set.
  std::vector<unsigned long> d_zones;
  for (const auto& zone : propertyMap->get_child("AbstractDiagram.Zones")) {
    unsigned long z = 0;
    for(const auto& inc: zone.second.get_child("in")) {
      z = z | bv_index[inc.second.data()];
    }
    d_zones.push_back(z);
  }

  // if there's no outside zone, add it
  if (std::find(d_zones.begin(), d_zones.end(), 0) == d_zones.end()) {
    d_zones.push_back(0);
  }

  //  Our zones are the vertices.  If the hamming distance between two zones is
  //  1, then there is an edge between them.
  AbstractDualGraph adg(d_zones.size());
  std::shared_ptr<AbstractDualGraph> graph = std::make_shared<AbstractDualGraph>(adg);

  for(int i=0; i < d_zones.size(); i++) {
    for(int j=i; j < d_zones.size(); j++) {
      if(1 == hammingDist(d_zones[i], d_zones[j])) {
        add_edge(i, j, *graph);
      }
    }
  }

  return graph;
}
