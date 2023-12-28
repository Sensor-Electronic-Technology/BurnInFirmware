#pragma once
#include <ArduinoJson.h>

typedef struct SerialDataOutput{
     double voltages[6]={0,0,0,0,0,0};
     double currents[6]={0,0,0,0,0,0};
     double temperatures[3]={0,0,0};
     double probeRuntimes[6]={0,0,0,0,0,0};
     bool heater1=false,heater2=false,heater3=false;
     bool running=false,paused=false;
     int currentSP=150;
     double temperatureSP=85;
     long runTimeSecs=54234;
     long elapsedSecs=12345;
     void Serialize(StaticJsonDocument<384>* doc,bool initialize){
        if(initialize){
            JsonArray v_array = doc->createNestedArray("Voltages");

            JsonArray c_array = doc->createNestedArray("Currents");

            JsonArray t_array = doc->createNestedArray("Temperatures");

            JsonArray rt_array = doc->createNestedArray("ProbeRuntimes");

            for(int i=0;i<6;i++){
                v_array[i]=this->voltages[i];
                c_array[i]=this->currents[i];
                rt_array[i]=this->probeRuntimes[i];
            }

            for(int i=0;i<3;i++){
                t_array[i]=this->temperatures[i];
            }

            (*doc)["Heater1State"] = this->heater1;
            (*doc)["Heater2State"] = this->heater2;
            (*doc)["Heater3State"] = this->heater3;
            (*doc)["CurrentSetPoint"] = this->currentSP;
            (*doc)["TemperatureSetPoint"] = this->temperatureSP;
            (*doc)["RuntimeSeconds"] = this->runTimeSecs;
            (*doc)["ElapsedSeconds"] = this->elapsedSecs;
            (*doc)["Running"] = this->running;
            (*doc)["Paused"] = this->paused;
        }else{
            for(int i=0;i<6;i++){
                (*doc)["Voltages"][i]=this->voltages[i];
                (*doc)["Currents"][i]=this->currents[i];
                (*doc)["ProbeRuntimes"][i]=this->probeRuntimes[i];
            }
            for(int i=0;i<3;i++){
                (*doc)["Temperatures"][i]=this->temperatures[i];
            }
            (*doc)["Heater1State"] = this->heater1;
            (*doc)["Heater2State"] = this->heater2;
            (*doc)["Heater3State"] = this->heater3;
            (*doc)["CurrentSetPoint"] = this->currentSP;
            (*doc)["TemperatureSetPoint"] = this->temperatureSP;
            (*doc)["RuntimeSeconds"] = this->runTimeSecs;
            (*doc)["ElapsedSeconds"] = this->elapsedSecs;
            (*doc)["Running"] = this->running;
            (*doc)["Paused"] = this->paused;
        }
        //serializeJson(*doc, Serial);
     }

}SerialDataOutput;


class SerialComManager{
private: 
    StaticJsonDocument<384> outputDoc;
public:
    void CreateJsonDocuments(){

    }
};


