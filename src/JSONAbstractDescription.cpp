#include "JSONAbstractDescription.h"

JSONAbstractDescription::JSONAbstractDescription(std::shared_ptr<stringstream> json) : propertyMap(new ptree) {
  read_json(*json, *propertyMap);
}

std::shared_ptr<DrawableGraph> JSONAbstractDescription::toDrawableGraph() {
  auto graph_count_pair = toAbstractDualGraph(propertyMap);
  auto graph = graph_count_pair.first;
  const auto num_contours = graph_count_pair.second;
  const int num_zones = num_vertices(*graph);

  auto bvs = std::make_shared<std::vector<unsigned long>>();
  // FIXME: There should be a way of getting bvs without iterating through all
  // the vertices.
  auto name_index = get(vertex_name, *graph);

  BGL_FORALL_VERTICES(vertex, *graph, AbstractDualGraph) {
    unsigned long bitvector = name_index[vertex];
    bvs->push_back(bitvector);
  }
  // end FIXME

  auto dg = std::make_shared<DrawableGraph>();
  dg->root = toClusterHeirarchy(bvs, num_contours);
  dg->num_nodes = num_zones;
  return dg;
}

unsigned long hammingDist(unsigned long x, unsigned long y) {
  unsigned dist = 0, val = x ^ y;

  while(val) {
    ++dist;
    val &= val - 1;
  }

  return dist;
}

// don't overoptimise and use HAKMEM
int popCount(unsigned long n) {
  unsigned long u = n;

  do {
    n = n>>1;
    u -= n;
  } while (n);

  return (int) u;
}

std::shared_ptr<RootCluster> JSONAbstractDescription::toClusterHeirarchy(std::shared_ptr<std::vector<unsigned long>> bvs, unsigned long num_contours) {
  auto root = std::make_shared<RootCluster>();
  // allocate these on heap as they get passed back in RootCluster.
  std::vector<RectangularCluster *> clusters;
  for(int i=0; i< num_contours;++i) {
    clusters.push_back(new RectangularCluster());
  }

  // sort the vector by the number of bits in each long or, if they have the
  // same number of bits, just use < on longs.
  std::sort(bvs->begin(),
            bvs->end(),
            [] (unsigned long x, unsigned long y) {int xc=popCount(x), yc=popCount(y); bool r=false; (xc==yc)? r=x<y : r=xc<yc; return r;}
            );
  for(int i=0; i<num_contours; ++i) {
    unsigned long c_mask = pow(2, i);
    std::bitset<64> b(c_mask);
    for(auto j: *bvs) {
      std::cout << "Comparing " << j << " and " << b << std::endl;
      if(j == (c_mask | j)) {
        // rectangle j is a child of cluster i
        std::cout << "Adding " << j << " as a child of " << b << " pointer " << clusters[i] << std::endl;
        clusters[i]->addChildNode(j);
      }
    }
  }

  for(auto cluster: clusters) {
    root->addChildCluster(cluster);
  }

  return root;
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
    // FIXME: Use bitset instead of unisigned long
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
