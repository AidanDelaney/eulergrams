#include "TestContainmentHierarchy.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestContainmentHierarchy );

void TestContainmentHierarchy::setUp() {
}

void TestContainmentHierarchy::tearDown() {
}

void TestContainmentHierarchy::testSimpleCreation() {
  unsigned long num_contours(1);
  auto bvs = std::make_shared<std::vector<unsigned long>>();
  bvs->push_back(0l);
  bvs->push_back(1l);

  std::shared_ptr<RootCluster> root = JSONAbstractDescription::toClusterHeirarchy(bvs, num_contours);

  auto clusters = root->clusters;
  CPPUNIT_ASSERT_EQUAL(num_contours, (unsigned long) clusters.size());
  // there should be 1 node in each cluster
  for(auto cluster: root->clusters) {
    //    auto quack = cluster->nodes.size();
    //    std::cout << "Cluster nodes: " << quack << std::endl;
    //    CPPUNIT_ASSERT_EQUAL(1, (int) cluster->nodes.size());
  }
}

void TestContainmentHierarchy::testVenn2Creation() {
  unsigned long num_contours(2);
  auto bvs = std::make_shared<std::vector<unsigned long>>();
  bvs->push_back(0l);
  bvs->push_back(1l);
  bvs->push_back(3l);

  std::shared_ptr<RootCluster> root = JSONAbstractDescription::toClusterHeirarchy(bvs, num_contours);

  auto clusters = root->clusters;
  CPPUNIT_ASSERT_EQUAL(num_contours, (unsigned long) clusters.size());
}

void TestContainmentHierarchy::testComplexCreation() {
  unsigned long num_contours(3);
  auto bvs = std::make_shared<std::vector<unsigned long>>();
  bvs->push_back(0l);
  bvs->push_back(1l);
  bvs->push_back(2l);
  bvs->push_back(3l);
  bvs->push_back(7l);

  std::shared_ptr<RootCluster> root = JSONAbstractDescription::toClusterHeirarchy(bvs, num_contours);

  auto clusters = root->clusters;
  CPPUNIT_ASSERT_EQUAL(num_contours, (unsigned long) clusters.size());
}
