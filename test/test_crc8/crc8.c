#include <unity.h>
#include <runner.h>
#include <crc.h>

// Helpers

void setUp(){}
void tearDown(){}

void crc8_test_sequence1(){
  uint8_t data[] = {0x01,0x02,0x03,0x04,0x05};
  uint8_t crc = cmux_crc(data, sizeof(data));

  TEST_ASSERT_EQUAL(0x01, crc);
}

void crc8_test_sequence2(){
  uint8_t data[] = {0x01,0x02,0x03,0x04,0x05,0x06};
  uint8_t crc = cmux_crc(data, sizeof(data));

  TEST_ASSERT_EQUAL(0x45, crc);
}

MAIN(){
  UNITY_BEGIN();
  RUN_TEST(crc8_test_sequence1);
  RUN_TEST(crc8_test_sequence2);
  UNITY_END();
  return 0;
}
