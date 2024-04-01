#include <gtest/gtest.h>
// uncomment line below if you plan to use GMock
// #include <gmock/gmock.h>

// TEST(...)
// TEST_F(...)

#include <almost-decent.h>

#if defined(ARDUINO)
#include <Arduino.h>

void setup()
{
  // should be the same value as for the `test_speed` option in "platformio.ini"
  // default value is test_speed=115200
  Serial.begin(115200);

  ::testing::InitGoogleTest();
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock();
}

void loop()
{
  // Run tests
  if (RUN_ALL_TESTS())
    ;

  // sleep for 1 sec
  delay(1000);
}

#else
int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  // if you plan to use GMock, replace the line above with
  // ::testing::InitGoogleMock(&argc, argv);

  if (RUN_ALL_TESTS())
    ;

  // std::cout << formatWeightWithTimestamp(22, 0, 0, 10) << std::endl;
  // std::cout << formatWeightWithTimestamp(0, 1, 2, 3) << std::endl;
  // std::cout << formatWeightWithTimestamp(101, 1, 2, 4) << std::endl;
  // std::cout << formatWeightWithTimestamp(1940, 1, 2, 5) << std::endl;
  // std::cout << formatWeightWithTimestamp(7059, 1, 2, 6) << std::endl;
  // std::cout << formatWeightWithTimestamp(11180, 1, 2, 7) << std::endl;

  // Always return zero-code and allow PlatformIO to parse results
  return 0;
}
#endif

TEST(decent, formatsMessage) {
  EXPECT_EQ("03CE001600000A0000DB", formatWeightWithTimestamp(22, 0, 0, 10));
  EXPECT_EQ("03CE00000102030000CD", formatWeightWithTimestamp(0, 1, 2, 3));
  EXPECT_EQ("03CE00650102040000A8", formatWeightWithTimestamp(101, 1, 2, 4));
  EXPECT_EQ("03CE079401020500005E", formatWeightWithTimestamp(1940, 1, 2, 5));
  EXPECT_EQ("03CE1B93010206000045", formatWeightWithTimestamp(7059, 1, 2, 6));
  EXPECT_EQ("03CE2BAC01020700004A", formatWeightWithTimestamp(11180, 1, 2, 7));
}