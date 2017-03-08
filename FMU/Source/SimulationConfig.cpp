// Copyright 2016 Jacob Chapman

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <locale>
#include <functional>
#include <cstring>

#include <cstddef>
#include "rapidxml_utils.hpp"
#include "rapidxml.hpp"
#include "Log.h"
#include "Utility.h"
#include "SimulationConfig.h"

std::vector<buildingStruct> SimulationConfig::buildings;
std::vector<std::string> SimulationConfig::outputRegexs;
std::vector<LVNNodeStruct> SimulationConfig::lvn;
std::map<int, windowStruct> SimulationConfig::windows;
std::map<int, shadeStruct> SimulationConfig::shades;
simulationStruct SimulationConfig::info;
int SimulationConfig::stepCount = -1;
std::string SimulationConfig::FileActivity;
std::string SimulationConfig::FileLargeAppliance;
std::string SimulationConfig::FolderSmallAppliance;
std::string SimulationConfig::RunLocation;

SimulationConfig::SimulationConfig() {}

void SimulationConfig::reset() {
  buildings.clear();
  windows.clear();
  shades.clear();
  outputRegexs.clear();
  info = simulationStruct();
  stepCount = -1;
  FileActivity = "";
  FileLargeAppliance = "";
  FolderSmallAppliance = "";
  RunLocation = "";
}

void SimulationConfig::parseLVN(rapidxml::xml_node<> *node) {
    buildings.clear();
    rapidxml::xml_node<> *cnode = node->first_node();
    LVNNodeStruct l;
    l.id = -1;
    parseLVNNode(cnode, &l);
}

void SimulationConfig::parseLVNNode(rapidxml::xml_node<> *node,
                                                        LVNNodeStruct * p) {
    rapidxml::xml_node<> *cnode = node->first_node();
    bool children = false;
    LVNNodeStruct l;
    l.parent = p->id;
    while (cnode) {
        if (nodeNameIs(cnode, "children")) {
          children = true;
        } else if (nodeNameIs(cnode, "id")) {
          l.id = std::stoi(cnode->value());
        } else if (nodeNameIs(cnode, "impedance")) {
          l.impedance = std::stod(cnode->value());
        }
        cnode = cnode->next_sibling();
    }
    if (children) {
      cnode = node->first_node();
      while (cnode) {
          if (nodeNameIs(cnode, "children")) {
            parseLVNNode(cnode->first_node(), &l);
          }
          cnode = cnode->next_sibling();
      }
    }
    p->children.push_back(l.id);
    lvn.push_back(l);
}

void SimulationConfig::parseBuildings(rapidxml::xml_node<> *node) {
    buildings.clear();
    rapidxml::xml_node<> *cnode = node->first_node();
    int id = 0;
    while (cnode) {
        if (nodeNameIs(cnode, "building")) {
              parseBuilding(cnode, id);
              id++;
        }
        cnode = cnode->next_sibling();
    }
}

void SimulationConfig::parseBuilding(rapidxml::xml_node<> *node, const int id) {
    buildingStruct b;
    b.id = id;
    int zonecount = 0;
    std::pair<std::string, ZoneStruct> zsOut;
    zsOut.first = "out";
    zsOut.second.name = "out";
    zsOut.second.activities.push_back(9);
    zsOut.second.id = zonecount;
    zsOut.second.active = false;
    b.zones.insert(zsOut);
    rapidxml::xml_node<> *cnode = node->first_node();
    while (cnode) {
        if (nodeNameIs(cnode, "name")) {
            b.name = cnode->value();
        } else if (nodeNameIs(cnode, "agents")) {
            parseOccupants(cnode, &b);
        } else if (nodeNameIs(cnode, "id")) {
            b.id = std::stoi(cnode->value());
        } else if (nodeNameIs(cnode, "appliances")) {
            parseAppliances(cnode, &b);
        } else if (nodeNameIs(cnode, "zone")) {
            zonecount++;
            std::pair<std::string, ZoneStruct> zone;
            zone.second.id = zonecount;
            zone.second.active = true;
            rapidxml::xml_node<> *znode = cnode->first_node();
            while (znode) {
                if (nodeNameIs(znode, "name")) {
                    zone.first = znode->value();
                    zone.second.name = znode->value();
                } else if (nodeNameIs(znode, "activities")
                          || nodeNameIs(znode, "activity")) {
                    zone.second.activities =
                          activityNamesToIds(Utility::splitCSV(znode->value()));
                } else if (nodeNameIs(znode, "groundfloor")) {
                    zone.second.groundFloor = std::stoi(znode->value());
                } else if (nodeNameIs(znode, "windowcount")) {
                    zone.second.windowCount = std::stoi(znode->value());
                }
                znode = znode->next_sibling();
            }

            if (zone.second.activities.empty()) {
                LOG << "Please define activities for zone: ";
                LOG << zone.second.name << "\n";
                LOG << "This can be done manually in the No-MASS simulation ";
                LOG << "configuration file\nOr through DesignBuilders";
                LOG << " detailed occupant interface\n";
                LOG.error();
            }
            b.zones.insert(zone);
        }
        cnode = cnode->next_sibling();
    }
    buildings.push_back(b);
}

std::vector<int> SimulationConfig::activityNamesToIds(
          const std::vector<std::string> & activities) {
  std::vector<int> act;
  for (const std::string & activity : activities) {
    if (activity == "Sleep") {
      act.push_back(0);
    } else if (activity == "Passive") {
      act.push_back(1);
    } else if (activity == "AudioVisual") {
      act.push_back(2);
    } else if (activity == "IT") {
      act.push_back(3);
    } else if (activity == "Cooking") {
      act.push_back(4);
    } else if (activity == "Cleaning") {
      act.push_back(5);
    } else if (activity == "Washing") {
      act.push_back(6);
    } else if (activity == "Metabolic") {
      act.push_back(7);
    } else if (activity == "WashingAppliance") {
      act.push_back(8);
    }
  }
  return act;
}

std::vector<double> SimulationConfig::prioritiesToVector(
                            const std::string & priorities) {
  std::vector<double> prior(csvToDouble(priorities));
  while (prior.size() < 24) {
    prior.push_back(prior.back());
  }
  return prior;
}

void SimulationConfig::parseAppliances(rapidxml::xml_node<> *node,
                                                            buildingStruct *b) {
  rapidxml::xml_node<> *cnode = node->first_node();
  while (cnode) {
    if (nodeNameIs(cnode, "large") ||
                nodeNameIs(cnode, "largelearning") ||
                            nodeNameIs(cnode, "grid")) {
      rapidxml::xml_node<> *anode = cnode->first_node();
      appLargeStruct s;
      while (anode) {
        const std::string name = nameToLower(anode);
        const std::string value = anode->value();
        setValFromNodeIfName(&s.id, value, name, "id");
        setValFromNodeIfName(&s.cost, value, name, "cost");
        setValFromNodeIfName(&s.epsilon, value, name, "epsilon");
        setValFromNodeIfName(&s.alpha, value, name, "alpha");
        setValFromNodeIfName(&s.gamma, value, name, "gamma");
        setValFromNodeIfName(&s.update, value, name, "updateqtable");
        if (nodeNameIs(anode, "priority")) {
          s.priority = prioritiesToVector(anode->value());
        } else if (nodeNameIs(anode, "timerequired")) {
          s.timeRequired = prioritiesToVector(anode->value());
        } else if (nodeNameIs(anode, "activities")) {
          s.activities = activityNamesToIds(Utility::splitCSV(anode->value()));
        }
        anode = anode->next_sibling();
      }
      if (nodeNameIs(cnode, "large")) {
        b->AppliancesLarge.push_back(s);
      } else if (nodeNameIs(cnode, "largelearning")) {
        b->AppliancesLargeLearning.push_back(s);
      } else if (nodeNameIs(cnode, "grid")) {
        b->AppliancesGrid.push_back(s);
      }
    } else if (nodeNameIs(cnode, "small")) {
      rapidxml::xml_node<> *anode = cnode->first_node();
      appSmallStruct s;
      while (anode) {
        const std::string name = nameToLower(anode);
        const std::string value = anode->value();
        setValFromNodeIfName(&s.id, value, name, "id");
        setValFromNodeIfName(&s.WeibullParameters, value, name, "weibullparameters");
        setValFromNodeIfName(&s.StateProbabilities, value, name, "stateprobabilities");
        setValFromNodeIfName(&s.Fractions, value, name, "fractions");
        setValFromNodeIfName(&s.SumRatedPowers, value, name, "sumratedpowers");
        if (nodeNameIs(anode, "priority")) {
          s.priority = prioritiesToVector(anode->value());
        }
        anode = anode->next_sibling();
      }
      b->AppliancesSmall.push_back(s);
    }  else if (nodeNameIs(cnode, "fmi")) {
      rapidxml::xml_node<> *anode = cnode->first_node();
      appFMIStruct s;
      while (anode) {
        const std::string name = nameToLower(anode);
        const std::string value = anode->value();
        setValFromNodeIfName(&s.id, value, name, "id");
        setValFromNodeIfName(&s.variableName, value, name, "variablename");
        if (nodeNameIs(anode, "priority")) {
          s.priority = prioritiesToVector(anode->value());
        }
        anode = anode->next_sibling();
      }
      b->AppliancesFMI.push_back(s);
    }  else if (nodeNameIs(cnode, "battery") || nodeNameIs(cnode, "batterygridcostreward")) {
      rapidxml::xml_node<> *anode = cnode->first_node();
      appBatteryStruct s;
      while (anode) {
        const std::string name = nameToLower(anode);
        const std::string value = anode->value();
        setValFromNodeIfName(&s.id, value, name, "id");
        setValFromNodeIfName(&s.epsilon, value, name, "epsilon");
        setValFromNodeIfName(&s.alpha, value, name, "alpha");
        setValFromNodeIfName(&s.gamma, value, name, "gamma");
        setValFromNodeIfName(&s.update, value, name, "updateqtable");
        if (nodeNameIs(anode, "priority")) {
          s.priority = prioritiesToVector(anode->value());
        }
        anode = anode->next_sibling();
      }
      if (nodeNameIs(cnode, "battery")) {
        b->AppliancesBattery.push_back(s);
      } else {
        b->AppliancesBatteryGrid.push_back(s);
      }
    } else if (nodeNameIs(cnode, "pv") || nodeNameIs(cnode, "csv")) {
        rapidxml::xml_node<> *anode = cnode->first_node();
        appCSVStruct s;
        while (anode) {
          const std::string name = nameToLower(anode);
          const std::string value = anode->value();
          setValFromNodeIfName(&s.fileSupply, value, name, "filename");
          setValFromNodeIfName(&s.fileSupply, value, name, "supply");
          setValFromNodeIfName(&s.fileDemand, value, name, "demand");
          setValFromNodeIfName(&s.id, value, name, "id");
          if (nodeNameIs(anode, "priority")) {
            s.priority = prioritiesToVector(anode->value());
          } else if (nodeNameIs(anode, "cost")) {
            s.cost = csvToDouble(anode->value());
          }
          anode = anode->next_sibling();
        }
        b->AppliancesCSV.push_back(s);
      }
    cnode = cnode->next_sibling();
  }
}

void SimulationConfig::parseOccupants(rapidxml::xml_node<> *node,
                                                            buildingStruct *b) {
  info.presencePage = false;
  rapidxml::xml_node<> *cnode = node->first_node();
  while (cnode) {
    if (nodeNameIs(cnode, "agent")) {
      agentStruct agent;
      rapidxml::xml_node<> *anode = cnode->first_node();
      while (anode) {
        const std::string name = nameToLower(anode);
        const std::string value = anode->value();
        if (nodeNameIs(name, "profile")) {
          rapidxml::xml_node<> *pnode = anode->first_node();
          while (pnode) {
            std::string text = pnode->name();
            if (text == "file") {
              SimulationConfig::FileActivity = SimulationConfig::RunLocation
                                                + pnode->value();
            } else {
              std::pair<int, std::string> a;
              if (text == "monday") {
                info.presencePage = true;
                a.first = 0;
              } else if (text == "tuesday") {
                a.first = 1;
              } else  if (text == "wednesday") {
                a.first = 2;
              } else  if (text == "thursday") {
                a.first = 3;
              } else  if (text == "friday") {
                a.first = 4;
              } else  if (text == "saturday") {
                a.first = 5;
              } else  if (text == "sunday") {
                a.first = 6;
              } else {
                text.erase(0, 1);
                a.first = std::stoi(text);
              }
              a.second = pnode->value();
              agent.profile.insert(a);
            }
            pnode = pnode->next_sibling();
          }
        }
        setValFromNodeIfName(&agent.bedroom, value, name, "bedroom");
        setValFromNodeIfName(&agent.office, value, name, "office");
        setValFromNodeIfName(&agent.power, value, name, "power");
        setValFromNodeIfName(&agent.windowId, value, name, "window");
        setValFromNodeIfName(&agent.shadeId, value, name, "shade");
        setValFromNodeIfName(&agent.edtry, value, name, "edtry");
        setValFromNodeIfName(&agent.age, value, name, "age");
        setValFromNodeIfName(&agent.computer, value, name, "computer");
        setValFromNodeIfName(&agent.civstat, value, name, "civstat");
        setValFromNodeIfName(&agent.unemp, value, name, "unemp");
        setValFromNodeIfName(&agent.retired, value, name, "retired");
        setValFromNodeIfName(&agent.sex, value, name, "sex");
        setValFromNodeIfName(&agent.famstat, value, name, "famstat");
        setValFromNodeIfName(&agent.ShadeClosedDuringSleep, value, name, "shadeclosedduringsleep");
        setValFromNodeIfName(&agent.ShadeClosedDuringWashing, value, name, "shadeclosedduringwashing");
        setValFromNodeIfName(&agent.ShadeDuringNight, value, name, "shadeduringnight");
        setValFromNodeIfName(&agent.ShadeDuringAudioVisual, value, name, "shadeduringaudiovisual");
        setValFromNodeIfName(&agent.LightOffDuringAudioVisual, value, name, "lightoffduringaudiovisual");
        setValFromNodeIfName(&agent.LightOffDuringSleep, value, name, "lightoffduringsleep");
        setValFromNodeIfName(&agent.WindowOpenDuringCooking, value, name, "windowopenduringcooking");
        setValFromNodeIfName(&agent.WindowOpenDuringWashing, value, name, "windowopenduringwashing");
        setValFromNodeIfName(&agent.WindowOpenDuringSleeping, value, name, "windowopenduringsleeping");
        setValFromNodeIfName(&agent.ApplianceDuringDay, value, name, "applianceduringday");
        anode = anode->next_sibling();
      }

      if (SimulationConfig::info.presencePage
        && agent.profile.size() != 7) {
        LOG << "Occupant presence has not been defined using the Page ";
        LOG << "method\nPlease add a presence profile for each day of ";
        LOG << "the week in the No-MASS simulation configuration file";
        LOG << "\nIn DesignBuilder please select the correct activity ";
        LOG << "profile\n";
        LOG.error();
      } else if (!SimulationConfig::info.presencePage
        && agent.profile.size() != 24
        && SimulationConfig::FileActivity == "") {
        LOG << "The activity profile is not defined for each hour ";
        LOG << "using the Said method\nPlease add a activity profile ";
        LOG << "for hour in the No-MASS simulation configuration file";
        LOG << "\nIn DesignBuilder please select the correct activity ";
        LOG << "profile\n";
        LOG.error();
      }
      b->agents.push_back(agent);
    }
    cnode = cnode->next_sibling();
  }
}

void SimulationConfig::parseModels(rapidxml::xml_node<> *node) {
    rapidxml::xml_node<> *cnode = node->first_node();
    while (cnode) {
        if (nodeNameIs(cnode, "agentheatgains")) {
            SimulationConfig::info.agentHeatGains = std::stoi(cnode->value());
        } else if (nodeNameIs(cnode, "lights")) {
            SimulationConfig::info.lights = std::stoi(cnode->value());
        } else if (nodeNameIs(cnode, "heating")) {
            SimulationConfig::info.heating = std::stoi(cnode->value());
        } else if (nodeNameIs(cnode, "windows")) {
            parseWindows(cnode);
        } else if (nodeNameIs(cnode, "shades")) {
            parseShades(cnode);
        }
        cnode = cnode->next_sibling();
    }
}

void SimulationConfig::parseWindows(rapidxml::xml_node<> *node) {
  windows.clear();
  rapidxml::xml_node<> *cnode = node->first_node();
  while (cnode) {
      if (nodeNameIs(cnode, "windowslearn")) {
        SimulationConfig::info.windowsLearn = std::stoi(cnode->value());
      } else if (nodeNameIs(cnode, "enabled")) {
        SimulationConfig::info.windows = std::stoi(cnode->value());
      } else if (nodeNameIs(cnode, "window")) {
        rapidxml::xml_node<> *snode = cnode->first_node();
        std::pair<int, windowStruct> ws;
        while (snode) {
          const std::string name = nameToLower(snode);
          const std::string value = snode->value();
          setValFromNodeIfName(&ws.first, value, name, "id");
          setValFromNodeIfName(&ws.second.aop, value, name, "aop");
          setValFromNodeIfName(&ws.second.bopout, value, name, "bopout");
          setValFromNodeIfName(&ws.second.shapeop, value, name, "shapeop");
          setValFromNodeIfName(&ws.second.a01arr, value, name, "a01arr");
          setValFromNodeIfName(&ws.second.b01inarr, value, name, "b01inarr");
          setValFromNodeIfName(&ws.second.b01outarr, value, name, "b01outarr");
          setValFromNodeIfName(&ws.second.b01absprevarr, value, name, "b01absprevarr");
          setValFromNodeIfName(&ws.second.b01rnarr, value, name, "b01rnarr");
          setValFromNodeIfName(&ws.second.a01int, value, name, "a01int");
          setValFromNodeIfName(&ws.second.b01inint, value, name, "b01inint");
          setValFromNodeIfName(&ws.second.b01outint, value, name, "b01outint");
          setValFromNodeIfName(&ws.second.b01presint, value, name, "b01presint");
          setValFromNodeIfName(&ws.second.b01rnint, value, name, "b01rnint");
          setValFromNodeIfName(&ws.second.a01dep, value, name, "a01dep");
          setValFromNodeIfName(&ws.second.b01outdep, value, name, "b01outdep");
          setValFromNodeIfName(&ws.second.b01absdep, value, name, "b01absdep");
          setValFromNodeIfName(&ws.second.b01gddep, value, name, "b01gddep");
          setValFromNodeIfName(&ws.second.a10dep, value, name, "a10dep");
          setValFromNodeIfName(&ws.second.b10indep, value, name, "b10indep");
          setValFromNodeIfName(&ws.second.b10outdep, value, name, "b10outdep");
          setValFromNodeIfName(&ws.second.b10absdep, value, name, "b10absdep");
          setValFromNodeIfName(&ws.second.b10gddep, value, name, "b10gddep");
          snode = snode->next_sibling();
        }

        windows.insert(ws);
      }
      cnode = cnode->next_sibling();
  }
}


void SimulationConfig::parseShades(rapidxml::xml_node<> *node) {
  shades.clear();
  rapidxml::xml_node<> *cnode = node->first_node();
  while (cnode) {
      if (nodeNameIs(cnode, "enabled")) {
        SimulationConfig::info.shading = std::stoi(cnode->value());
      } else if (nodeNameIs(cnode, "shade")) {
        rapidxml::xml_node<> *snode = cnode->first_node();
        std::pair<int, shadeStruct> ws;
        while (snode) {
          const std::string name = nameToLower(snode);
          const std::string value = snode->value();
          setValFromNodeIfName(&ws.first, value, name, "id");
          setValFromNodeIfName(&ws.second.a01arr, value, name, "a01arr");
          setValFromNodeIfName(&ws.second.b01inarr, value, name, "b01inarr");
          setValFromNodeIfName(&ws.second.b01sarr, value, name, "b01sarr");
          setValFromNodeIfName(&ws.second.a10arr, value, name, "a10arr");
          setValFromNodeIfName(&ws.second.b10inarr, value, name, "b10inarr");
          setValFromNodeIfName(&ws.second.b10sarr, value, name, "b10sarr");
          setValFromNodeIfName(&ws.second.a01int, value, name, "a01int");
          setValFromNodeIfName(&ws.second.b01inint, value, name, "b01inint");
          setValFromNodeIfName(&ws.second.b01sint, value, name, "b01sint");
          setValFromNodeIfName(&ws.second.a10int, value, name, "a10int");
          setValFromNodeIfName(&ws.second.b10inint, value, name, "b10inint");
          setValFromNodeIfName(&ws.second.b10sint, value, name, "b10sint");
          setValFromNodeIfName(&ws.second.afullraise, value, name, "afullraise");
          setValFromNodeIfName(&ws.second.boutfullraise, value, name, "boutfullraise");
          setValFromNodeIfName(&ws.second.bsfullraise, value, name, "bsfullraise");
          setValFromNodeIfName(&ws.second.bsfulllower, value, name, "bsfulllower");
          setValFromNodeIfName(&ws.second.boutfulllower, value, name, "boutfulllower");
          setValFromNodeIfName(&ws.second.afulllower, value, name, "afulllower");
          setValFromNodeIfName(&ws.second.aSFlower, value, name, "asflower");
          setValFromNodeIfName(&ws.second.bSFlower, value, name, "bsflower");
          setValFromNodeIfName(&ws.second.shapelower, value, name, "shapelower");
          snode = snode->next_sibling();
        }
        shades.insert(ws);
      }
      cnode = cnode->next_sibling();
  }
}



/**
 * @brief Parses the simulation configuration file
 * @details Parse the configuration file for the all the parameteres needed in the simulation
 *
 * @param filename location of the simulation file to parse.
 */

void SimulationConfig::parseConfiguration(const std::string & filename) {
  namespace rx = rapidxml;
  rx::file<> xmlFile(filename.c_str());  // Default template is char
  rx::xml_document<> doc;
  doc.parse<0>(xmlFile.data());    // 0 means default parse flags
  rx::xml_node<> *root_node = doc.first_node("simulation");
  rx::xml_node<> *node = root_node->first_node();
  while (node) {
    const std::string name = nameToLower(node);
    const std::string value = node->value();
    setValFromNodeIfName(&info.save, value, name, "save");
    setValFromNodeIfName(&info.timeStepsPerHour, value, name, "timestepsperhour");
    setValFromNodeIfName(&info.startDay, value, name, "beginmonth");
    setValFromNodeIfName(&info.endDay, value, name, "endday");
    setValFromNodeIfName(&info.endMonth, value, name, "endmonth");
    setValFromNodeIfName(&info.startMonth, value, name, "beginmonth");
    setValFromNodeIfName(&info.learn, value, name, "learn");
    setValFromNodeIfName(&info.startDayOfWeek, value, name, "startdayofweek");
    setValFromNodeIfName(&info.learnupdate, value, name, "learnupdate");
    setValFromNodeIfName(&info.learnep, value, name, "learnep");
    setValFromNodeIfName(&info.caseOrder, value, name, "case");
    setValFromNodeIfName(&info.ShadeClosedDuringNight, value, name, "shadeclosedduringnight");

    if (nodeNameIs(name, "seed")) {
        Utility::setSeed(std::stoi(node->value()));
    } else if (nodeNameIs(name, "precision")) {
        SimulationConfig::info.precision = std::stoi(node->value());
    } else if (nodeNameIs(name, "gridcost")) {
        SimulationConfig::info.GridCost = csvToDouble(node->value());
    } else if (nodeNameIs(name, "output")) {
      rapidxml::xml_node<> *cnode = node->first_node();
      while (cnode) {
          if (nodeNameIs(cnode, "regex")) {
            outputRegexs.push_back(cnode->value());
          }
          cnode = cnode->next_sibling();
      }
    } else if (nodeNameIs(name, "models")) {
       parseModels(node);
    } else if (nodeNameIs(name, "buildings")) {
        parseBuildings(node);
    } else if (nodeNameIs(name, "lvn")) {
        parseLVN(node);
    }

    node = node->next_sibling();
  }

  FileLargeAppliance = RunLocation + "AppliancesLarge.xml";
  FolderSmallAppliance = RunLocation + "SmallAppliances/";

  timeSteps();
  if (outputRegexs.empty()) {
    outputRegexs.push_back(".*");
  }
}

ZoneStruct SimulationConfig::getZone(std::string* zoneName) {
  ZoneStruct zone;
  for (buildingStruct b : buildings) {
    if (b.zones.find(*zoneName) != b.zones.end()) {
        zone = b.zones.at(*zoneName);
        break;
    }
  }
  return zone;
}

bool SimulationConfig::isZoneGroundFloor(std::string* zoneName) {
  bool floor = false;
  for (buildingStruct b : buildings) {
    if (b.zones.find(*zoneName) != b.zones.end()) {
        floor = b.zones.at(*zoneName).groundFloor;
        break;
    }
  }
  return floor;
}


double SimulationConfig::lengthOfTimestep() {
    return 3600 / SimulationConfig::info.timeStepsPerHour;
}

void SimulationConfig::step() {
    stepCount++;
}

int SimulationConfig::getStepCount() {
    return stepCount;
}

void SimulationConfig::setStepCount(const int stepcount) {
  stepCount = stepcount;
}

void SimulationConfig::timeSteps() {
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = daysInMonth[info.startMonth - 1] - (info.startDay - 1);
    for (int i = info.startMonth; i < info.endMonth -1; i++) {
            days += daysInMonth[i];
    }
    if (info.startMonth != info.endMonth) {
            days += info.endDay;
    }
    int hours = days * 24;
    SimulationConfig::info.timeSteps = hours * info.timeStepsPerHour;
}

const std::string SimulationConfig::nameToLower(
                                            const rapidxml::xml_node<> *node) {
    std::string name(node->name());
  for (unsigned int i = 0; i < name.size(); i++) {
    name[i] = std::tolower(name[i]);
  }
  return name;
}

bool SimulationConfig::nodeNameIs(const std::string & name, const char * str2) {
  return strComp(name.c_str(), str2);
}

bool SimulationConfig::nodeNameIs(const rapidxml::xml_node<> *node,
                                                        const char * str2) {
  return strComp(nameToLower(node).c_str(), str2);
}

bool SimulationConfig::strComp(const char * str1, const char * str2) {
  return std::strcmp(str1, str2) == 0;
}

std::vector<double> SimulationConfig::csvToDouble(const std::string & s) {
  std::vector<double> items;
  for (std::string &item : Utility::splitCSV(s)) {
      items.push_back(std::stod(item));
  }
  return items;
}


void SimulationConfig::setValFromNodeIfName(int * val,
                              const std::string & value,
                              const std::string & name,
                              const char * expected) {
    if (nodeNameIs(name, expected)) {
        *val = std::stoi(value);
    }
}

void SimulationConfig::setValFromNodeIfName(bool * val,
                              const std::string & value,
                              const std::string & name,
                              const char * expected) {
    if (nodeNameIs(name, expected)) {
        *val = std::stoi(value);
    }
}

void SimulationConfig::setValFromNodeIfName(double * val,
                              const std::string & value,
                              const std::string & name,
                              const char * expected) {
    if (nodeNameIs(name, expected)) {
        *val = std::stod(value);
    }
}

void SimulationConfig::setValFromNodeIfName(float * val,
                              const std::string & value,
                              const std::string & name,
                              const char * expected) {
    if (nodeNameIs(name, expected)) {
        *val = std::stof(value);
    }
}

void SimulationConfig::setValFromNodeIfName(std::string * val,
                              const std::string & value,
                              const std::string & name,
                              const char * expected) {
    if (nodeNameIs(name, expected)) {
        *val = value;
    }
}
