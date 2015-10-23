// Copyright 2015 Jacob Chapman
#include <limits.h>
#include <fstream>

#include "Model_Appliance_Usage.h"

#include "gtest/gtest.h"


TEST(Appliance_Usage, on) {
  Model_Appliance_Usage mau;
  std::string country = "France1";
  mau.setCountry(country);
  mau.parseConfiguration("tests/Files/ApplianceUsage.xml");

  double onTv[144] = {0.1276,0.1112,0.0951,0.083,0.0727,0.0636,0.0554,0.0498,0.0451,0.0412,0.0384,0.036,0.0335,0.0327,0.0315,0.0304,0.0298,0.0287,0.0279,0.0274,0.0267,0.0264,0.026,0.0262,0.0254,0.0249,0.0247,0.0249,0.0248,0.0247,0.0246,0.0244,0.0249,0.0251,0.0257,0.0281,0.0293,0.0307,0.0349,0.0395,0.0485,0.0667,0.082,0.097,0.1107,0.1236,0.1373,0.1489,0.1485,0.1373,0.1332,0.1324,0.1297,0.1319,0.134,0.1337,0.1338,0.1351,0.1362,0.1356,0.1367,0.1362,0.1368,0.1378,0.14,0.1421,0.1451,0.1495,0.1538,0.1631,0.171,0.1797,0.1895,0.2036,0.2164,0.231,0.2413,0.246,0.2417,0.2333,0.2253,0.218,0.2129,0.2097,0.2059,0.2036,0.201,0.1927,0.1831,0.1787,0.1754,0.1707,0.1695,0.1666,0.1645,0.1649,0.1653,0.167,0.1724,0.182,0.1907,0.1985,0.2067,0.2146,0.2224,0.2298,0.2388,0.2489,0.2623,0.2746,0.2838,0.2896,0.2973,0.3008,0.3007,0.3066,0.3125,0.3271,0.3435,0.3578,0.3682,0.3763,0.3864,0.3934,0.4048,0.41,0.4152,0.4179,0.419,0.4175,0.4119,0.4056,0.398,0.3861,0.3564,0.3103,0.2708,0.2382,0.2115,0.1901,0.1713,0.1546,0.1411,0.1287};
  double onWashingMachine[144] = {0.03767,0.0236,0.02125,0.01776,0.01685,0.01516,0.01444,0.01334,0.01211,0.01111,0.01061,0.01563,0.01497,0.01393,0.01321,0.01262,0.01227,0.01356,0.01346,0.01296,0.0129,0.01171,0.01007,0.00907,0.0081,0.00813,0.00734,0.00694,0.00672,0.00694,0.007,0.00741,0.00797,0.00832,0.00985,0.01139,0.01293,0.01622,0.01983,0.02341,0.02931,0.03643,0.04651,0.05514,0.06396,0.07306,0.08172,0.09063,0.09826,0.10158,0.10585,0.10704,0.10902,0.10999,0.11222,0.11533,0.11774,0.11875,0.1195,0.11894,0.11868,0.11614,0.11523,0.11423,0.11247,0.11147,0.10993,0.1082,0.10566,0.10497,0.10328,0.10513,0.10745,0.10858,0.10776,0.10528,0.10572,0.10337,0.1045,0.1051,0.10538,0.10601,0.10466,0.10208,0.09813,0.09418,0.08887,0.08398,0.08024,0.0783,0.07557,0.07296,0.06938,0.06772,0.06643,0.06502,0.06217,0.06163,0.05975,0.05997,0.06104,0.06248,0.06116,0.06339,0.06443,0.06493,0.06593,0.06643,0.06772,0.06766,0.07058,0.07202,0.07249,0.07378,0.07387,0.07346,0.07371,0.0735,0.07312,0.07215,0.07114,0.06898,0.06756,0.06794,0.0681,0.06719,0.06738,0.06439,0.06198,0.06035,0.0568,0.05391,0.05034,0.0482,0.04742,0.04541,0.04488,0.0434,0.04161,0.03926,0.03574,0.03235,0.03082,0.04121};
  double onMicroWave[144] = {0.0002,0.00013,0.0001,0.00002,0.00003,0.00002,0.00003,0.00003,0.00003,0.00003,0.00006,0.00006,0.00003,0.00002,0.00005,0.00006,0.00004,0.00006,0.00006,0.0001,0.00008,0.00008,0.0001,0.00056,0.0007,0.00032,0.00021,0.00022,0.00025,0.00033,0.00038,0.00045,0.00166,0.00331,0.00361,0.01341,0.02329,0.04017,0.10819,0.13616,0.15955,0.2996,0.34348,0.34536,0.32521,0.29854,0.26809,0.24473,0.21659,0.20355,0.17545,0.16172,0.15933,0.14535,0.12359,0.09453,0.07162,0.0602,0.04753,0.0415,0.03055,0.0263,0.02112,0.01927,0.01617,0.01681,0.0187,0.02351,0.03936,0.08944,0.15868,0.22966,0.29166,0.33702,0.33676,0.29166,0.20931,0.14702,0.10588,0.08529,0.06755,0.04445,0.02591,0.0187,0.01356,0.00875,0.00694,0.00517,0.00392,0.00331,0.00333,0.00345,0.00372,0.00523,0.00503,0.00611,0.00818,0.00926,0.01208,0.01814,0.02005,0.0225,0.01705,0.01445,0.01057,0.0107,0.00992,0.01083,0.01424,0.01771,0.02928,0.05358,0.1109,0.16281,0.18272,0.20332,0.20763,0.21757,0.22051,0.1871,0.16107,0.12263,0.0996,0.08171,0.07219,0.04392,0.03427,0.02845,0.02653,0.02125,0.01571,0.01137,0.00665,0.00534,0.00395,0.00314,0.00209,0.00163,0.00115,0.00057,0.00029,0.00023,0.00023,0.00019};
  double onCooker[144] = {0.0275,0.0272,0.0263,0.0249,0.0234,0.0245,0.025,0.0243,0.0233,0.0215,0.0244,0.0243,0.0238,0.023,0.023,0.0238,0.024,0.0233,0.0222,0.0222,0.0241,0.0245,0.0265,0.0217,0.0223,0.0243,0.023,0.0236,0.0232,0.0226,0.0228,0.0237,0.0225,0.0253,0.023,0.024,0.0232,0.024,0.0293,0.0389,0.0433,0.0444,0.0459,0.0475,0.0407,0.0421,0.0436,0.0476,0.0477,0.0513,0.0542,0.0534,0.0583,0.0631,0.0659,0.0705,0.0752,0.0767,0.0849,0.0901,0.0917,0.1006,0.112,0.123,0.1349,0.1481,0.1696,0.1897,0.2206,0.255,0.2767,0.2829,0.2744,0.2381,0.2069,0.1819,0.1627,0.1454,0.1278,0.1182,0.1078,0.0942,0.0849,0.0777,0.0701,0.0651,0.0629,0.0613,0.0592,0.0598,0.0574,0.058,0.0576,0.0575,0.0565,0.05612,0.057,0.059,0.0589,0.0604,0.0645,0.0684,0.0727,0.0756,0.0839,0.0947,0.1069,0.1292,0.1513,0.1866,0.2177,0.2482,0.2709,0.2768,0.273,0.2681,0.261,0.256,0.2532,0.2313,0.2157,0.1902,0.1718,0.1563,0.1415,0.1255,0.1095,0.0955,0.0864,0.0738,0.0661,0.0603,0.0534,0.0481,0.0446,0.0409,0.0381,0.036,0.0339,0.0318,0.0311,0.0307,0.0286,0.0279};
  double onDishWasher[144] = {0.00094,0.00079,0.00079,0.00086,0.00093,0.00079,0.00093,0.00108,0.001,0.001,0.001,0.00115,0.00115,0.00115,0.00107,0.00107,0.00107,0.00107,0.00107,0.00107,0.00107,0.00107,0.001,0.001,0.00115,0.00115,0.00108,0.00123,0.00116,0.00116,0.00116,0.00123,0.00131,0.00124,0.00168,0.00169,0.00236,0.00318,0.00364,0.00438,0.00502,0.00652,0.00936,0.01167,0.01215,0.01728,0.02221,0.02542,0.03094,0.03629,0.03682,0.03885,0.04543,0.04632,0.04984,0.05258,0.05579,0.04257,0.04221,0.04082,0.03428,0.03416,0.0343,0.0239,0.0219,0.02258,0.02039,0.02544,0.02887,0.04668,0.06093,0.06688,0.0687,0.08518,0.09714,0.11721,0.13555,0.15222,0.16752,0.17361,0.17785,0.1639,0.16665,0.16585,0.14078,0.13608,0.13226,0.08676,0.08583,0.084,0.07433,0.07294,0.07166,0.04633,0.04522,0.04349,0.03641,0.03613,0.03727,0.02226,0.02326,0.02357,0.02157,0.02256,0.02233,0.01544,0.01581,0.01568,0.01538,0.01773,0.01967,0.02533,0.03408,0.0394,0.04508,0.05808,0.06938,0.0895,0.1051,0.11562,0.12223,0.13169,0.1364,0.12687,0.13095,0.13314,0.1267,0.12661,0.12458,0.1005,0.09934,0.0957,0.06248,0.05737,0.05353,0.02652,0.02466,0.02379,0.01855,0.01735,0.0119,0.00058,0.00058,0.00072};
  double onFridge[144] = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};

  int sims = 100000;
  for (int app = 0; app < 6; app++) {
    double on[144] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for (int i = 0; i < sims; i++) {
      for (int timeStep = 0; timeStep < 144; timeStep++) {
        on[timeStep] += mau.onAt(app, timeStep);
      }
    }
    std::string file = "";
    switch (app) {
      case 0:
          file = "switchon" + country + "cooker.csv";
        break;
      case 1:
          file = "switchon" + country + "microwave.csv";
        break;
      case 2:
          file = "switchon" + country + "tv.csv";
        break;
      case 3:
          file = "switchon" + country + "washingmachine.csv";
        break;
      case 4:
          file = "switchon" + country + "dishwasher.csv";
        break;
      case 5:
          file = "switchon" + country + "fridge.csv";
        break;
    }

    std::ofstream myfile;
    myfile.open(file);

    for (int timeStep = 0; timeStep < 144; timeStep++) {
      switch (app) {
        case 0:
            //ASSERT_NEAR(on[timeStep] / sims, onCooker[timeStep], 0.005);
          break;
        case 1:
            //ASSERT_NEAR(on[timeStep] / sims, onMicroWave[timeStep], 0.004);
          break;
        case 2:
            //ASSERT_NEAR(on[timeStep] / sims, onTv[timeStep], 0.005);
          break;
        case 3:
            //ASSERT_NEAR(on[timeStep] / sims, onWashingMachine[timeStep], 0.004);
          break;
        case 4:
            //ASSERT_NEAR(on[timeStep] / sims, onDishWasher[timeStep], 0.005);
          break;
        case 5:
            //ASSERT_NEAR(on[timeStep] / sims, onFridge[timeStep], 1);
          break;
      }
      myfile << on[timeStep] / sims;
      myfile << std::endl;
    }

    myfile.close();
  }
}


TEST(Appliance_Usage, totalEnergy) {
  Model_Appliance_Usage mau;
  std::string country = "France1";
  mau.setCountry(country);
  mau.parseConfiguration("tests/Files/ApplianceUsage.xml");
  int sims = 365;
  for (int app = 0; app < 6; app++) {
    double consumption[144] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for (int i = 0; i < sims; i++) {
      for (int timeStep = 0; timeStep < 144; timeStep++) {
        consumption[timeStep] += mau.consumption(app, timeStep);
      }
    }

    std::string file = "";
    switch (app) {
      case 0:
          file = "totalEnergy" + country + "cooker.csv";
        break;
      case 1:
          file = "totalEnergy" + country + "microwave.csv";
        break;
      case 2:
          file = "totalEnergy" + country + "tv.csv";
        break;
      case 3:
          file = "totalEnergy" + country + "washingmachine.csv";
        break;
      case 4:
          file = "totalEnergy" + country + "dishwasher.csv";
        break;
      case 5:
          file = "totalEnergy" + country + "fridge.csv";
        break;
    }
    std::ofstream myfile;
    myfile.open(file);

    for (int timeStep = 0; timeStep < 144; timeStep++) {
      myfile << (consumption[timeStep]) / 6;
      myfile << std::endl;
    }
    myfile.close();
  }
}

TEST(Appliance_Usage, meanPower) {
  Model_Appliance_Usage mau;
  std::string country = "France1";
  mau.setCountry(country);
  mau.parseConfiguration("tests/Files/ApplianceUsage.xml");
  int sims = 365000;
  for (int app = 0; app < 6; app++) {
    double consumption[144] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

    for (int i = 0; i < sims; i++) {
      for (int timeStep = 0; timeStep < 144; timeStep++) {
        consumption[timeStep] += mau.consumption(app, timeStep);
      }
    }

    std::string file = "";
    switch (app) {
      case 0:
          file = "meanPower" + country + "cooker.csv";
        break;
      case 1:
          file = "meanPower" + country + "microwave.csv";
        break;
      case 2:
          file = "meanPower" + country + "tv.csv";
        break;
      case 3:
          file = "meanPower" + country + "washingmachine.csv";
        break;
      case 4:
          file = "meanPower" + country + "dishwasher.csv";
        break;
      case 5:
          file = "meanPower" + country + "fridge.csv";
        break;
    }
    std::ofstream myfile;
    myfile.open(file);

    for (int timeStep = 0; timeStep < 144; timeStep++) {
      myfile << (consumption[timeStep]) / sims;
      myfile << std::endl;
    }
    myfile.close();
  }
}

TEST(Appliance_Usage, meanFraction) {
  Model_Appliance_Usage mau;
  std::string country = "France1";
  mau.setCountry(country);
  mau.parseConfiguration("tests/Files/ApplianceUsage.xml");
  int sims = 365;
  for (int app = 0; app < 6; app++) {
    double consumption[144] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
    for (int i = 0; i < sims; i++) {
      for (int timeStep = 0; timeStep < 144; timeStep++) {
        if (mau.onAt(app, timeStep)) {
          consumption[timeStep] += mau.getMeanFraction(app);
        }
      }
    }

    std::string file2 = "";
    switch (app) {
      case 0:
          file2 = "meanFraction" + country + "cooker.csv";
        break;
      case 1:
          file2 = "meanFraction" + country + "microwave.csv";
        break;
      case 2:
          file2 = "meanFraction" + country + "tv.csv";
        break;
      case 3:
          file2 = "meanFraction" + country + "washingmachine.csv";
        break;
      case 4:
          file2 = "meanFraction" + country + "dishwasher.csv";
        break;
      case 5:
          file2 = "meanFraction" + country + "fridge.csv";
        break;
    }
    std::ofstream myfile2;
    myfile2.open(file2);
    for (int timeStep = 0; timeStep < 144; timeStep++) {
      myfile2 << (consumption[timeStep]) / 365;
      myfile2 << std::endl;
    }
    myfile2.close();
  }
}
