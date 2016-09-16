// Copyright 2016 Jacob Chapman

#ifndef APPLIANCE_GENERIC_CSV_H_
#define APPLIANCE_GENERIC_CSV_H_

#include <vector>
#include <string>
#include "Appliance.h"
#include "Model_Appliance_Power_CSV.h"

/**
 * @brief appliance read in from csv class
 * @details The CSV agent, handles the csv model
 */

class Appliance_Generic_CSV : public Appliance {
 public:
    Appliance_Generic_CSV();
    void setup();
    void preprocess();
    void setFileSupply(const std::string & filename);
    void setFileDemand(const std::string & filename);

 private:
    Model_Appliance_Power_CSV modelSupply;
    Model_Appliance_Power_CSV modelDemand;
    std::string fileSupply;
    std::string fileDemand;
    bool enableSupply;
    bool enableDemand;
};

#endif  // APPLIANCE_GENERIC_CSV_H_
