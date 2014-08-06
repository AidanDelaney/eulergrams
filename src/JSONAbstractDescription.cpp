#include "JSONAbstractDescription.h"

JSONAbstractDescription::JSONAbstractDescription(std::shared_ptr<stringstream> json) : propertyMap(new ptree) {
  read_json(*json, *propertyMap);
}

RootCluster * buildClusters(std::shared_ptr<AbstractDualGraph> graph,
                   int num_contours,
                   std::vector<RectangularCluster *> clusters) {
  // We're going to assume a 1-to-1 mapping between nodes and zone_index
  auto zone_index = get(vertex_name, *graph);

  // Each graph may contain several clusters that should be considered
  // "top level" i.e. the graph contains multiple disconnected subgraphs.
  // The key here is to compare zone identifiers, which are a bitvector
  // representation of the containment of the Euler diagram.  The zone ids
  // are contained in graph vertices.
  std::vector<long> top_levels;
  BGL_FORALL_VERTICES(zone, *graph, AbstractDualGraph) {
    const long z_id = zone_index[zone];

    // The outside zone (bitvector 0) is a special case
    // all clusters are children of bitvector 0
    // it will become our RootCluster
    if(0 == z_id) {
      continue;
    }

    // if, in this subgraph, this node is the top-level node, "save" it
    // it is a top-level node if childTest returns false on all of it's
    // neighbours
    bool zone_is_toplevel = true;
    AbstractDualGraph::adjacency_iterator n_iter, n_end;
    for(std::tie(n_iter, n_end) =boost::adjacent_vertices (zone, *graph); n_iter != n_end; ++n_iter) {
      const long n_id = zone_index[*n_iter];
      std::cout << z_id << " is a neighbour of " << zone_index[*n_iter] << std::endl;

      // test if z_id is more top-level than n_id
      if((z_id & n_id) != z_id) {
        zone_is_toplevel = false;
      }
    }

    if(zone_is_toplevel) {
      top_levels.push_back(z_id);
    }

    // Find out which clusters this zone is in
    //    for(int mask = 1; mask <= pow(2, num_contours); mask*=2) {
    //  if((z_id & mask) > 0) {
    //    clusters[log2(mask)]->addChildNode(z_id);
    //  }
    // }
  }

  if(top_levels.empty()) {
    // There is only one connected graph rooted at z_id == 0
    top_levels.push_back(0);
  }

  RootCluster * root = new RootCluster();
  for(int i = 1; i< clusters.size(); i++) {

    // find out if this cluster is a child of any of it's graph neighbours
    //    auto parents = findParents()
    //if

    // else
    root->addChildCluster(clusters[i]);
  }
}

std::shared_ptr<DrawableGraph> JSONAbstractDescription::toDrawableGraph() {
  auto graph_count_pair = toAbstractDualGraph(propertyMap);
  auto graph = graph_count_pair.first;
  const auto num_contours = graph_count_pair.second;
  const int num_zones = num_vertices(*graph);

  // There's a cluster for each contour, one for each key in the bitvector
  std::vector<RectangularCluster *> clusters(num_contours);

  // init clusters
  for(int i=0; i< num_contours; i++) {
    clusters[i] = new RectangularCluster();
  }
  RootCluster * root = buildClusters(graph, num_contours, clusters);

  DrawableGraph * dg = new DrawableGraph();
  dg->root = root;
  dg->num_nodes = num_zones;
  return std::make_shared<DrawableGraph>(*dg);
}

unsigned long hammingDist(unsigned long x, unsigned long y) {
  unsigned dist = 0, val = x ^ y;

  while(val) {
    ++dist;
    val &= val - 1;
  }

  return dist;
}


std::shared_ptr<ContainmentHierarchy> JSONAbstractDescription::toContainmentHierarchy(std::shared_ptr<AbstractDualGraph> adg, unsigned long num_contours) {
  ContainmentHierarchy ch;
  return std::make_shared<ContainmentHierarchy>(ch);
  /*
  ContainmentHierarchy ch;
  // We know our vertex lables encode containment as a bitvector
  auto zone_index = get(vertex_index, *graph);

  // Get the labels in ascending order.
  std::vector<long> 
  // Build the Hasse diagram.

  // Zone containment is build up level-by-level from level 0 to level
  // num_contours.  Each level is a level in the Hasse diagram of all possible
  // containments.  The level of the Hasse diagram corresponds with the number
  // of bits set to `1' in the vertex index.
  for(int level=0; level<num_contours; level++){
  }

  // for every possible zone
  for(int i = 0; i < pow(2, num_contours); i++) {
    
  } */
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
  AbstractDualGraph dual(d_zones.size()); // FIXME: allocate on heap
  unsigned long i = 0;
  BGL_FORALL_VERTICES(vertex, dual, AbstractDualGraph) {
    boost::put(vertex_name_t(), dual, vertex, d_zones[i]);
    i++;
  }
  std::shared_ptr<AbstractDualGraph> graph = std::make_shared<AbstractDualGraph>(dual);

  for(i=0; i < d_zones.size(); i++) {
    for(unsigned long j=i+1; j < d_zones.size(); j++) {
      if(1 == hammingDist(d_zones[i], d_zones[j])) {
        add_edge(i, j, *graph);
      }
    }
  }

  return std::make_pair(graph, bv_index.size());
}
