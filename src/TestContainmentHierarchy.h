#ifndef TESTCONTAINMENTHIERARCHY_H
#define TESTCONTAINMENTHIERARCHY_H

#include <memory>
#include <map>

#include "JSONAbstractDescription.h"

#include <cppunit/extensions/HelperMacros.h>

class TestContainmentHierarchy : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( TestContainmentHierarchy );
  CPPUNIT_TEST( testSimpleCreation );
  CPPUNIT_TEST( testVenn2Creation );
  CPPUNIT_TEST( testComplexCreation );
  CPPUNIT_TEST_SUITE_END();

 public:
  void setUp();
  void tearDown();

  void testSimpleCreation();
  void testVenn2Creation();
  void testComplexCreation();
};

#endif  // TESTCONTAINMENTHIERARCHY_H
