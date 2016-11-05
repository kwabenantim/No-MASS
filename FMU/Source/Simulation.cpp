// Copyright 2015 Jacob Chapman

#include <iostream>
#include <string>

#include "Environment.h"
#include "SimulationConfig.h"
#include "DataStore.h"
#include "Building.h"
#include "LVN_Node.h"
#include "SimulationTime.h"
#include "Simulation.h"

Simulation::Simulation() {
        simulationConfigurationFile = "SimulationConfig.xml";
}

void Simulation::setSimulationConfigurationFile(const std::string & filename) {
  this->simulationConfigurationFile = filename;
}
/**
 * @brief Calls the simulation preprocess
 * @details Reads in the configuration file and sends to parser.
 * Sets up the EnergyPlus processor, the AgentModel and the ZoneManager.
 */
void Simulation::preprocess() {
  SimulationTime::preprocess();
  parseConfiguration(SimulationConfig::RunLocation
    + simulationConfigurationFile);
  if (!LOG.getError()) {
    setupSimulationModel();
  }
  GridPowerDataId = DataStore::addVariable("grid_power");
  GridCostDataId = DataStore::addVariable("grid_cost");
}

void Simulation::parseConfiguration(const std::string & file) {
    SimulationConfig::parseConfiguration(file);
}

void Simulation::setupSimulationModel() {
    for (buildingStruct b : SimulationConfig::buildings) {
      buildings.push_back(Building());
      buildings.back().setup(b);
      buildings.back().preprocess();
    }
    lvn.setup();
}

/**
 * @brief Calls the simulation prostprocess
 *
 */
void Simulation::postprocess() {
  for (Building &b : buildings) {
    b.postprocess();
  }
  DataStore::print();
  DataStore::clearValues();
  SimulationTime::reset();
}

/**
 * @brief processes before timestep
 */
void Simulation::preTimeStep() {
  SimulationTime::trackTime();
  Environment::calculateDailyMeanTemperature();
}


/**
 * @brief Increments the timestep for the simulation
 * @details Increments the timestep for the EnergyPlus processor, the AgentModel and the ZoneManager.
 * Also we send any effects the agent have to the zones they are located in.
 */
void Simulation::timeStep() {

  for (Building &b : buildings) {
      b.step();
      b.stepAppliancesUse();
      b.addContactsTo(&building_negotiation);
  }
  double diff = building_negotiation.getDifference();
  if (diff < 0.0) {
    Contract m;
    m.id = -1;
    m.buildingID = -1;
    m.supplied = std::abs(diff);

    if (SimulationConfig::info.GridCost.size() == 24) {
      int stepCount = SimulationConfig::getStepCount();
      int hour = (stepCount * SimulationConfig::lengthOfTimestep()) / 3600;
      int hourOfDay = hour % 24;
      m.suppliedCost = SimulationConfig::info.GridCost[hourOfDay];
    } else if (SimulationConfig::info.GridCost.size() == 48) {
      int stepCount = SimulationConfig::getStepCount();
      int hour = (stepCount * SimulationConfig::lengthOfTimestep()) / 1800;
      int halfHourOfDay = hour % 48;
      m.suppliedCost = SimulationConfig::info.GridCost[halfHourOfDay];
    } else {
      m.suppliedCost = SimulationConfig::info.GridCost[0];
    }
    m.requested = 0;
    building_negotiation.submit(m);
    DataStore::addValue(GridPowerDataId, m.supplied);
    DataStore::addValue(GridCostDataId, m.suppliedCost);
  } else {
    DataStore::addValue(GridPowerDataId, 0);
    DataStore::addValue(GridCostDataId, 0);
  }
  building_negotiation.process();
  for (Building &b : buildings) {
    b.stepAppliancesNegotiation(building_negotiation);
  }
  building_negotiation.clear();
}

/**
 * @brief processes After timestep
 */
void Simulation::postTimeStep() {
  for (Building &b : buildings) {
      b.postTimeStep();
      lvn.setPowerForID(b.getPower(), b.getID());
  }
  lvn.postTimeStep();
}
