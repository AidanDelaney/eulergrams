#include "TestJSONImport.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TestJSONImport );

std::pair<std::shared_ptr<std::vector<unsigned long>>, unsigned long> TestJSONImport::getIdVector(std::string& fname) {
  std::ifstream file(fname);
  if (file) {
    std::shared_ptr<std::stringstream> buffer = std::make_shared<std::stringstream>();
    *buffer << file.rdbuf();
    file.close();

    std::shared_ptr<ptree> propertyMap = std::make_shared<ptree>();
    try {
      read_json(*buffer, *propertyMap);
    } catch ( ... ) {
      std::cerr << "Some JSON exception" << std::endl;
      CPPUNIT_FAIL("JSON exception");
    }


    // reload the buffer (not nice)
    std::ifstream f(fname);
    *buffer << f.rdbuf();
    f.close();
    JSONAbstractDescription jad(buffer);
    auto dg_pair = jad.toAbstractDualGraph(propertyMap);
    auto adg = dg_pair.first;
    auto num_contours = dg_pair.second;

    // get the node names and check they're 0 and 1 respectively
    auto zone_index = get(vertex_name, *adg);
    std::vector<unsigned long> ids;
    BGL_FORALL_VERTICES(zone, *adg, AbstractDualGraph) {
      ids.push_back(zone_index[zone]);
    }

    return std::make_pair(std::make_shared<std::vector<unsigned long>>(ids), num_contours);
  } else {
    CPPUNIT_FAIL("Can't open JSON import file");
  }
}

void TestJSONImport::setUp() {
}

void TestJSONImport::tearDown() {
}

void TestJSONImport::testSimpleImport() {
  std::string file("../tests/data/a.json");

  auto pair = getIdVector(file);
  auto ids = pair.first;
  auto num_contours = pair.second;

  unsigned long expected_cs = 1;
  CPPUNIT_ASSERT_EQUAL(expected_cs, num_contours);
  CPPUNIT_ASSERT_EQUAL(2, (int) ids->size());

  if(std::find(ids->begin(), ids->end(), 0) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 0'th zone");
  }
  if(std::find(ids->begin(), ids->end(), 1) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 1'st zone");
  }
}

void TestJSONImport::testVenn2Import() {
  std::string file("../tests/data/a_b_ab.json");

  auto pair = getIdVector(file);
  auto ids = pair.first;
  auto num_contours = pair.second;

  unsigned long expected_cs = 2;
  CPPUNIT_ASSERT_EQUAL(expected_cs, num_contours);
  CPPUNIT_ASSERT_EQUAL(4, (int) ids->size());

  if(std::find(ids->begin(), ids->end(), 0) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 0'th zone");
  }
  if(std::find(ids->begin(), ids->end(), 1) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 1'st zone");
  }
  if(std::find(ids->begin(), ids->end(), 2) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 2'nd zone");
  }
  if(std::find(ids->begin(), ids->end(), 3) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 3'rd zone");
  }
}

void TestJSONImport::testComplexImport() {
  std::string file("../tests/data/a_b_ab_abc.json");

  auto pair = getIdVector(file);
  auto ids = pair.first;
  auto num_contours = pair.second;

  unsigned long expected_cs = 3;
  CPPUNIT_ASSERT_EQUAL(expected_cs, num_contours);
  CPPUNIT_ASSERT_EQUAL(5, (int) ids->size());

  if(std::find(ids->begin(), ids->end(), 0) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 0'th zone");
  }
  if(std::find(ids->begin(), ids->end(), 1) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 1'st zone");
  }
  if(std::find(ids->begin(), ids->end(), 2) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 2 zone");
  }
  if(std::find(ids->begin(), ids->end(), 3) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 3 zone");
  }
  if(std::find(ids->begin(), ids->end(), 4) != ids->end()) {
    CPPUNIT_FAIL("Simple import should not have 4 zone");
  }
  if(std::find(ids->begin(), ids->end(), 7) == ids->end()) {
    CPPUNIT_FAIL("Simple import does not get 7 zone");
  }
}
