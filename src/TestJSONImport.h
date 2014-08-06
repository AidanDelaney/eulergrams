#ifndef TESTJSONIMPORT_H
#define TESTJSONIMPORT_H

#include <cppunit/extensions/HelperMacros.h>

class TestJSONImport : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TestJSONImport );
  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();

  void testConstructor();
};

#endif  // TESTJSONIMPORT_H
