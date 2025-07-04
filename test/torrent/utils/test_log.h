#include "helpers/test_main_thread.h"

class test_log : public TestFixtureWithMainThread {
  CPPUNIT_TEST_SUITE(test_log);

  CPPUNIT_TEST(test_basic);
  CPPUNIT_TEST(test_output_open);

  CPPUNIT_TEST(test_print);
  CPPUNIT_TEST(test_children);
  CPPUNIT_TEST(test_file_output);
  CPPUNIT_TEST(test_file_output_append);

  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override;

  void test_basic();
  void test_output_open();

  void test_print();
  void test_children();
  void test_file_output();
  void test_file_output_append();
};
