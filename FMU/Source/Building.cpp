// Copyright 2015 Jacob Chapman

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <utility>
#include "SimulationConfig.h"
#include "DataStore.h"
#include "Agent.h"
#include "State_Out.h"
#include "State_Present.h"
#include "State_Sleep.h"
#include "State_Passive.h"
#include "State_IT.h"
#include "State_Washing_Appliance.h"
#include "State_Audio_Visual.h"
#include "State_Cleaning.h"
#include "State_Cooking.h"
#include "State_Washing.h"
#include "State_Metabolic.h"
#include "Utility.h"
#include "Model_Presence.h"
#include "Building.h"

Building::Building() {}

void Building::setup(const buildingStruct &b) {
    name = b.name;
    for (std::pair<std::string, ZoneStruct> z : b.zones) {
      zones.push_back(std::make_shared<Building_Zone>(Building_Zone(z.second)));
    }
    int popSize = SimulationConfig::numberOfAgents();
    std::list<int> pop = Utility::randomIntList(popSize);
    // setup each agent randomly
    for (int a : pop) {
          population.push_back(Agent(a, zones));
    }
    initialiseStates();
}

void Building::setZones(const std::vector<std::shared_ptr<Building_Zone>> & zones) {
    this->zones = zones;
}

void Building::initialiseStates() {
    State_Present present;
    if (SimulationConfig::info.presencePage) {
        State_IT it;
        matchStateToZone(it);
        present.addState(it);
    } else {
        State_Sleep sleep;
        matchStateToZone(sleep);
        State_Passive passive;
        matchStateToZone(passive);
        State_Washing_Appliance washingAppliance;
        matchStateToZone(washingAppliance);
        State_Washing washing;
        matchStateToZone(washing);
        State_Audio_Visual audioVisual;
        matchStateToZone(audioVisual);
        State_Cleaning cleaning;
        matchStateToZone(cleaning);
        State_Cooking cooking;
        matchStateToZone(cooking);
        State_Metabolic metabolic;
        matchStateToZone(metabolic);
        State_IT it;
        matchStateToZone(it);
        present.addState(sleep);
        present.addState(passive);
        present.addState(washingAppliance);
        present.addState(washing);
        present.addState(audioVisual);
        present.addState(cleaning);
        present.addState(cooking);
        present.addState(metabolic);
        present.addState(it);
    }
    stateMachine.addState(present);
    State_Out out;
    matchStateToZone(out);
    stateMachine.addState(out);

    int popSize = population.size();
    std::list<int> pop = Utility::randomIntList(popSize);
    // step each agent randomly
    for (int a : pop) {
        population[a].setState(out);
    }
}

void Building::matchStateToZone(State &s) {
    for (unsigned int i =0; i < zones.size(); i++) {
      if (zones[i]->hasActivity(s.getId())) {
          s.setZonePtr(zones[i]);
          break;
      }
    }
}

void Building::step() {
    int popSize = population.size();
    std::list<int> pop = Utility::randomIntList(popSize);
    for (int a : pop) {
        population[a].step(&stateMachine);
    }
    for (std::shared_ptr<Building_Zone> &zone : zones) {
        if (!zone->isActive()) {
            continue;
        }
        setAgentCountForZone(zone);

        if (SimulationConfig::info.windows
          || SimulationConfig::info.windowsLearn) {
            setAgentWindowDecisionForZone(zone);
        }
        if (SimulationConfig::info.shading) {
            setAgentBlindDecisionForZone(zone);
        }
        if (SimulationConfig::info.lights) {
            setAgentLightDecisionForZone(zone);
        }
        setAgentHeatDecisionsForZone(zone);
        setAgentGainsForZone(zone);
    }

    buildingInteractions();
    for (std::shared_ptr<Building_Zone> &zone : zones) {
      if (zone->isActive()) {
        zone->step();
      }
    }
}

void Building::buildingInteractions() {
  if (SimulationConfig::info.ShadeClosedDuringNight) {
    int hourOfDay = DataStore::getValue("hourOfDay");
    if (hourOfDay > 19 || hourOfDay < 6) {
      for (std::shared_ptr<Building_Zone> &zone : zones) {
        if (zone->isActive()) {
          zone->setBlindState(0);
        }
      }
    }
  }
}

void Building::setAgentHeatDecisionsForZone(std::shared_ptr<Building_Zone> zone) {
  double currentState = zone->getHeatingState();
  double totalIncrease = 0;
  double totalDecrease = 0;
  double increase = 0;
  double decrease = 0;
  double same = 0;
  double increasePower = 0;
  double decreasePower = 0;
  double samePower = 0;

  for (Agent &agent : population) {
    //  if (agent.InteractionOnZone(*zone)) {
          double d = agent.getDesiredHeatState(*zone);
          double power = agent.getPower();

          if (d < currentState) {
              decrease++;
              totalDecrease = totalDecrease + d;
              decreasePower = decreasePower + power;

          } else if (d > currentState) {
              increase++;
              totalIncrease = totalIncrease + d;
              increasePower = increasePower + power;
          } else {
              same++;
              samePower = samePower + power;
          }
      //}
  }

  double state = currentState;
  if (samePower > increasePower && samePower > decreasePower) {
      state = currentState;
  } else if (samePower < increasePower && increasePower > decreasePower) {
      state = totalIncrease / increase;
  } else if (samePower < decreasePower && increasePower < decreasePower) {
      state = totalDecrease / decrease;
  } else if (samePower == increasePower && samePower > decreasePower) {
      if (Utility::tossACoin()) {
          state = totalIncrease / increase;
      }
  } else if (samePower > increasePower && samePower == decreasePower) {
      if (Utility::tossACoin()) {
          state = totalDecrease / decrease;
      }
  } else if (samePower < increasePower
      && samePower < decreasePower
      && increasePower == decreasePower) {
      if (Utility::tossACoin()) {
          state = totalIncrease / increase;
      } else {
          state = totalDecrease / decrease;
      }
  } else if ((samePower == increasePower) == decreasePower) {
      double i = Utility::randomDouble(0, 1);
      double d = Utility::randomDouble(0, 1);
      double s = Utility::randomDouble(0, 1);
      if (i > d && i > s) {
          state = totalIncrease / increase;
      } else if (d > s && d > i) {
          state = totalDecrease / decrease;
      } else if (s > i && s > d) {
          state = currentState;
      }
  }
  zone->setHeatingState(state);
}

void Building::setAgentGainsForZone(std::shared_ptr<Building_Zone> zone) {
    double numberOfAgents = 0;
    double totalRadientGains = 0;
    double aveRadientGains = 0;

    for (Agent &agent : population) {
        if  (agent.isActionHeatGains(*zone)) {
            numberOfAgents++;
            totalRadientGains += agent.getCurrentRadientGains(*zone);
        }
    }
    if (totalRadientGains > 0) {
        aveRadientGains = totalRadientGains / numberOfAgents;
    }
    zone->setCurrentAgentGains(aveRadientGains);
}

void Building::setAgentWindowDecisionForZone(std::shared_ptr<Building_Zone> zone) {
    double open = 0;
    double close = 0;
    int numberOfActiveAgents = 0;

    for (Agent &agent : population) {
        if (agent.isActionWindow(*zone)) {
            double power = agent.getPower();
          //  if ( power < 2 ) continue;
            numberOfActiveAgents++;
            if (agent.getDesiredWindowState(*zone)) {
                open = open + power;
            } else {
                close = close + power;
            }
        }
    }
    if (numberOfActiveAgents > 0) {
        if (open < close) {
            zone->setWindowState(false);
        } else if (open > close) {
            zone->setWindowState(true);
        } else {
            zone->setWindowState(Utility::tossACoin());
                zone->setWindowState(true);
        }
    }
}

void Building::setAgentBlindDecisionForZone(std::shared_ptr<Building_Zone> zone) {
    double currentState = zone->getBlindState();
    double totalIncrease = 0;
    double totalDecrease = 0;
    double increase = 0;
    double decrease = 0;
    double same = 0;
    double increasePower = 0;
    double decreasePower = 0;
    double samePower = 0;

    for (Agent &agent : population) {
        if (agent.isActionShades(*zone)) {
            double d = agent.getDesiredShadeState(*zone);
            double power = agent.getPower();

            if (d < currentState) {
                decrease++;
                totalDecrease = totalDecrease + d;
                decreasePower = decreasePower + power;

            } else if (d > currentState) {
                increase++;
                totalIncrease = totalIncrease + d;
                increasePower = increasePower + power;
            } else {
                same++;
                samePower = samePower + power;
            }
        }
    }

    double state = currentState;
    if (samePower > increasePower && samePower > decreasePower) {
        state = currentState;
    } else if (samePower < increasePower && increasePower > decreasePower) {
        state = totalIncrease / increase;
    } else if (samePower < decreasePower && increasePower < decreasePower) {
        state = totalDecrease / decrease;
    } else if (samePower == increasePower && samePower > decreasePower) {
        if (Utility::tossACoin()) {
            state = totalIncrease / increase;
        }
    } else if (samePower > increasePower && samePower == decreasePower) {
        if (Utility::tossACoin()) {
            state = totalDecrease / decrease;
        }
    } else if (samePower < increasePower
        && samePower < decreasePower
        && increasePower == decreasePower) {
        if (Utility::tossACoin()) {
            state = totalIncrease / increase;
        } else {
            state = totalDecrease / decrease;
        }
    } else if ((samePower == increasePower) == decreasePower) {
        double i = Utility::randomDouble(0, 1);
        double d = Utility::randomDouble(0, 1);
        double s = Utility::randomDouble(0, 1);
        if (i > d && i > s) {
            state = totalIncrease / increase;
        } else if (d > s && d > i) {
            state = totalDecrease / decrease;
        } else if (s > i && s > d) {
            state = currentState;
        }
    }
    zone->setBlindState(state);
}
void Building::setAgentLightDecisionForZone(std::shared_ptr<Building_Zone> zone) {
    double open = 0;
    double close = 0;
    double numberOfActiveAgents = 0;
    for (Agent &agent : population) {
        if (agent.isActionLights(*zone)) {
            numberOfActiveAgents++;
            double power = agent.getPower();
            if (agent.getDesiredLightState(*zone)) {
                open = open + power;
            } else {
                close = close + power;
            }
        }
    }
    if (numberOfActiveAgents > 0.0) {
        if (open < close) {
            zone->setLightState(false);
        } else if (open > close) {
            zone->setLightState(true);
        } else {
            zone->setLightState(Utility::tossACoin());
        }
    }
//     if(zone->getLightState())

}

void Building::setAgentCountForZone(std::shared_ptr<Building_Zone> zone) {
    double numberOfAgents = 0;
    for (Agent &agent : population) {
        if (agent.currentlyInZone(*zone)) {
            numberOfAgents++;
        }
    }
    zone->setOccupantFraction(numberOfAgents / population.size());
}

void Building::postTimeStep() {
  for (Agent &agent : population) {
      agent.postTimeStep();
  }
}

void Building::postprocess() {
    for (Agent &agent : population) {
        agent.postprocess();
    }
}

bool Building::hasZone(const std::string& zoneName) const {
  bool has = false;
  for (const std::shared_ptr<Building_Zone> & z : zones) {
    if (zoneName == z->getName()) {
      has = true;
    }
  }
  return has;
}
