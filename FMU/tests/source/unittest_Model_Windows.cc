// Copyright 2015 Jacob Chapman

#include <limits.h>
#include <string>
#include "Model_Windows.h"
#include "Utility.h"

#include "gtest/gtest.h"

class Test_Windows : public ::testing::Test {
 protected:
    Model_Windows mw;

    virtual void SetUp();
    virtual void AfterConfiguration();
};

void Test_Windows::SetUp() {
}
void Test_Windows::AfterConfiguration() {
}

TEST_F(Test_Windows, Arrival) {
  Utility::setSeed(1);
  double indoorTemperature = 35;
  double outdoorTemperature = 10;
  double previousDuration = 5*12*10*60;
  bool rain = false;
  int timeStepLengthInMinutes = 5;

  for (int i =0; i < 1; i++) {
    mw.arrival(indoorTemperature, outdoorTemperature,
        previousDuration, rain, timeStepLengthInMinutes);
    EXPECT_EQ(false, mw.getWindowState());
  }

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(true, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  ASSERT_NEAR(9, mw.getDurationOpen(), 0.01);
  EXPECT_EQ(true, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  ASSERT_NEAR(9, mw.getDurationOpen(), 0.01);
  EXPECT_EQ(true, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  ASSERT_NEAR(9, mw.getDurationOpen(), 0.01);
  EXPECT_EQ(true, mw.getWindowState());
}


TEST_F(Test_Windows, Inter) {
  Utility::setSeed(1);
  double indoorTemperature = 35;
  double outdoorTemperature = 10;
  double previousDuration = 5*12*10*60;
  bool rain = false;
  int timeStepLengthInMinutes = 5;
  int dur = 0;

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(false, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(true, mw.getWindowState());

  EXPECT_EQ(0, dur);



  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(true, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(true, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  dur = mw.getDurationOpen();
  EXPECT_EQ(9, dur);
  EXPECT_EQ(true, mw.getWindowState());

  mw.intermediate(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  dur = mw.getDurationOpen();
  EXPECT_EQ(4, dur);
  EXPECT_EQ(true, mw.getWindowState());

  mw.intermediate(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  dur = mw.getDurationOpen();
  EXPECT_EQ(0, dur);
  EXPECT_EQ(false, mw.getWindowState());


  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(false, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(false, mw.getWindowState());

  mw.arrival(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(false, mw.getWindowState());

  while (mw.getWindowState() == false || dur < 300) {
    mw.arrival(indoorTemperature, outdoorTemperature,
        previousDuration, rain, timeStepLengthInMinutes);
    dur = mw.getDurationOpen();
  }

  EXPECT_EQ(true, mw.getWindowState());

  EXPECT_EQ(1934, dur);
  int durCount = dur;
  while (dur > timeStepLengthInMinutes) {
    mw.intermediate(indoorTemperature, outdoorTemperature,
        previousDuration, rain, timeStepLengthInMinutes);
    dur = mw.getDurationOpen();
    durCount = durCount - timeStepLengthInMinutes;
    EXPECT_EQ(durCount, dur);
    EXPECT_EQ(true, mw.getWindowState());
  }
  mw.intermediate(indoorTemperature, outdoorTemperature,
      previousDuration, rain, timeStepLengthInMinutes);
  EXPECT_EQ(false, mw.getWindowState());
}

TEST_F(Test_Windows, depart) {
  Utility::setSeed(1);
  double indoorTemperature = 35;
  double dailyMeanTemperature = 34;
  double futureDuration = 5*12*10*60;
  bool rain = false;
  int timeStepLengthInMinutes = 5;
  int dur = 0;

  EXPECT_EQ(false, mw.getWindowState());
  for (int i =0; i < 69; i++) {
    mw.departure(indoorTemperature, dailyMeanTemperature, 300, 0);
    EXPECT_EQ(false, mw.getWindowState());
  }
  for (int i =0; i < 56; i++) {
    mw.departure(indoorTemperature, dailyMeanTemperature, 300, 0);
    EXPECT_EQ(true, mw.getWindowState());
  }
}
