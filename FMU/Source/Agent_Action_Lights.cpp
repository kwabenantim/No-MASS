// Copyright 2015 Jacob Chapman

#include <vector>
#include "Model_Lights.h"
#include "Agent_Action_Lights.h"

Agent_Action_Lights::Agent_Action_Lights() {}

void Agent_Action_Lights::setOffDuringAudioVisual(bool OffDuringAudioVisual) {
  this->OffDuringAudioVisual = OffDuringAudioVisual;
}

void Agent_Action_Lights::setOffDuringSleep(bool OffDuringSleep) {
  this->OffDuringSleep = OffDuringSleep;
}

void Agent_Action_Lights::step(const Building_Zone& zone, const bool inZone,
    const bool previouslyInZone, const std::vector<double> &activities) {

    float Lumint = zone.getDaylightingReferencePoint1Illuminance();
    bool lightState = zone.getLightState();
    Model_Lights m_lightUsage;

    if (inZone && !previouslyInZone) {
            lightState = m_lightUsage.arrival(lightState, Lumint);
    } else if (inZone && previouslyInZone) {
            lightState = m_lightUsage.intermediate(lightState, Lumint);
    } else if (!inZone && previouslyInZone) {
            int pffs = getFutureDurationOfAbsenceState(activities);
            lightState = m_lightUsage.departure(lightState, pffs);
    }
    result = lightState;
}

bool Agent_Action_Lights::BDI(const std::vector<double> &activities) {
  bool bdi = false;
  int stepCount = SimulationConfig::getStepCount();
  if (OffDuringSleep && activities.at(stepCount) == 0) {
      result = 0;
      bdi = true;
  }
  if (OffDuringAudioVisual && activities.at(stepCount) == 2) {
      result = 0;
      bdi = true;
  }
  return bdi;
}
