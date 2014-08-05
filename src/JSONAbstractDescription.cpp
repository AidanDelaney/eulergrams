#include "JSONAbstractDescription.h"

JSONAbstractDescription::JSONAbstractDescription(std::shared_ptr<stringstream> json) : propertyMap(new ptree) {
  read_json(*json, *propertyMap);
}

typedef boost::vec_adj_list_vertex_id_map<boost::property<boost::vertex_index_t, long unsigned int>, long unsigned int> IndexMap;
void buildClusters(std::shared_ptr<AbstractDualGraph> graph,
                   int num_contours,
                   std::vector<RectangularCluster *> clusters,
                   std::vector<vpsc::Rectangle *> nodes,
                   IndexMap zone_index) {
  BGL_FORALL_VERTICES(zone, *graph, AbstractDualGraph) {
    const long z_id = zone_index[zone];

    // The outside zone (bitvector 0) is a special case
    // all clusters are children of bitvector 0
    // it will become our RootCluster
    if(0 == z_id) {
      continue;
    }

    // Find out which clusters this zone is in
    for(int mask = 1; mask <= pow(2, num_contours); mask*=2) {
      if((z_id & mask) > 0) {
        clusters[log2(mask)]->addChildNode(z_id);
      }
    }
  }
}

std::shared_ptr<RootCluster> JSONAbstractDescription::toAbstractGraph() {
  auto graph_count_pair = toAbstractDualGraph(propertyMap);
  auto graph = graph_count_pair.first;
  const auto num_contours = graph_count_pair.second;
  const int num_zones = num_vertices(*graph);

  // There's a cluster for each contour, one for each key in the bitvector
  std::vector<RectangularCluster *> clusters(num_contours);
  // We need one node per zone i.e. vertex in our graph
  std::vector<vpsc::Rectangle *> nodes(num_zones);

  // init clusters
  for(int i=0; i< num_contours; i++) {
    clusters[i] = new RectangularCluster();
  }

  // init nodes: Give nodes some initial position
  const double width = 5;
  const double height = 5;
  double pos = 0;
  for(int i=0; i< num_zones; i++) {
    vpsc::Rectangle * rect = new vpsc::Rectangle(pos, pos +width, pos, pos +height);
    nodes[i] = rect;

    // XXX randomness is needed because COLA doesn't currently untangle
    // the graph properly if all the nodes begin at the same position.
    pos += (rand() % 10) - 5;
  }

  // We're going to assume a 1-to-1 mapping between nodes and zone_index
  auto zone_index = get(vertex_index, *graph);
  buildClusters(graph, num_contours, clusters, nodes, zone_index);
  RootCluster * root = new RootCluster();
  for(int i = 1; i< clusters.size(); i++) {
    root->addChildCluster(clusters[i]);
  }

  /// FIXME: move this code
    EdgeLengths eLengths;
    std::vector<Edge> es;
    ConstrainedFDLayout alg(nodes, es, 10, true, eLengths);
    CompoundConstraints ccs;
    alg.setConstraints(ccs);

    UnsatisfiableConstraintInfos unsatisfiableX, unsatisfiableY;
    alg.setUnsatisfiableConstraintInfo(&unsatisfiableX, &unsatisfiableY);

    alg.setClusterHierarchy(root);
    //alg.makeFeasible();
    alg.run();
    alg.outputInstanceToSVG("overlappingClusters02");
  ///

  nodes.clear();
  clusters.clear();
  // For each vertex in the graph, create a cluster
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

std::pair<std::shared_ptr<AbstractDualGraph>, long> JSONAbstractDescription::toAbstractDualGraph(std::shared_ptr<ptree> propertyMap) {
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
  AbstractDualGraph adg(d_zones.size()); // FIXME: allocate on heap
  std::shared_ptr<AbstractDualGraph> graph = std::make_shared<AbstractDualGraph>(adg);

  for(unsigned long i=0; i < d_zones.size(); i++) {
    for(unsigned long j=i+1; j < d_zones.size(); j++) {
      if(1 == hammingDist(d_zones[i], d_zones[j])) {
        add_edge(i, j, *graph);
      }
    }
  }

  return std::make_pair(graph, bv_index.size());
}
