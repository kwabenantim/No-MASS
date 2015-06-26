
#include "VariableStore.h"
#include "Model_HeatGains.h"
#include "Agent_Action_Heat_Gains.h"

Agent_Action_Heat_Gains::Agent_Action_Heat_Gains()
{
    //ctor

}

void Agent_Action_Heat_Gains::setup(int agentid){
        this->id = agentid;

        idAsString = std::to_string(id);
        VariableStore::addVariable("AgentGains" + idAsString);
        VariableStore::addVariable("Agent_Metabolic_Rate_" + idAsString);
        VariableStore::addVariable("Agent_clo_" + idAsString);
        VariableStore::addVariable("Agent_ppd_" + idAsString);
        VariableStore::addVariable("Agent_pmv_" + idAsString);
        VariableStore::addVariable("Agent_Fanger_Neutral_Temperature_" + idAsString);
}


void Agent_Action_Heat_Gains::prestep(int id, double clo, double metabolicRate){
    this->clo = clo;
    this->metabolicRate = metabolicRate;

}

void Agent_Action_Heat_Gains::step(const Zone& zone, bool inZone, bool previouslyInZone, const std::vector<double> &activities){
        ppd = 5;
        pmv = 0;
        result = 0;
        if (inZone) {
                Model_HeatGains h;
                /**
                 * Calculates the Fanger pmv and sets the instance varibles related to results.
                 * @param metabolicRate Metabolic Rate
                 * @param partialWaterPressure partial water vapour kPa
                 * @param meanRadiantTemperature mean radiant temperature C
                 * @param externalWork external work
                 * @param ta air temperature
                 * @param clo Clothing value
                 * @param airVelocity Air velocity
                 */

                double airTemp = zone.getMeanAirTemperature();
                double airHumid = zone.getAirRelativeHumidity();
                double meanRadient = zone.getMeanRadiantTemperature();

                h.calculate(metabolicRate, airHumid, meanRadient, 0, airTemp, clo, 0);
                result = h.getAllHeatGains();
                ppd = h.getPpd();
                pmv = h.getPmv();
                //std::cout << "PMV: " << pmv << std::endl;

        }
        std::string name = "AgentGains" + idAsString;
        VariableStore::addValue(name.c_str(), result);
        name = "Agent_Metabolic_Rate_" + idAsString;
        VariableStore::addValue(name.c_str(), metabolicRate);
        name = "Agent_clo_" + idAsString;
        VariableStore::addValue(name.c_str(), clo);
        name = "Agent_ppd_" + idAsString;
        VariableStore::addValue(name.c_str(), ppd);
        name = "Agent_pmv_" + idAsString;
        VariableStore::addValue(name.c_str(), pmv);

}

int Agent_Action_Heat_Gains::getPMV(){
    return pmv;
}
