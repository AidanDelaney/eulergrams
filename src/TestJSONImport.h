#ifndef TESTJSONIMPORT_H
#define TESTJSONIMPORT_H

#include <memory>
#include <map>

#include "JSONAbstractDescription.h"

#include <cppunit/extensions/HelperMacros.h>

class TestJSONImport : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TestJSONImport );
  CPPUNIT_TEST( testSimpleImport );
  CPPUNIT_TEST( testVenn2Import );
  CPPUNIT_TEST( testComplexImport );
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();

  void testSimpleImport();
  void testVenn2Import();
  void testComplexImport();

  std::pair<std::shared_ptr<std::vector<unsigned long>>, unsigned long> getIdVector(std::string&);
};

#endif  // TESTJSONIMPORT_H
