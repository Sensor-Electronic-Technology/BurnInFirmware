#pragma once
#include "../constants.h"



typedef struct ControllerConfiguration{
    bool currentSwitchEnabled=false;
    int  setTemperature;
    CurrentValue  SetCurrent;
	unsigned long updateInterval;
	unsigned long comInterval;
	unsigned long resetDelay;
	unsigned long probeDelay;
	

}ControllerConfiguration;

typedef struct BurnTimerConfiguration{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
}TimingConfiguration;


#pragma region ComponentConfigs

typedef struct CurrentSelectorConfig{
    int pin120mA=PIN_CURRENT_120mA;
    int pin60mA=PIN_CURRENT_60mA;
    int currentPin=PIN_CURRENT;
}CurrentSelectorConfig;

typedef struct CurrentSensorConfig{
	int Pin;
	double fWeight;
	CurrentSensorConfig(int pin=0,double fweight=DEFAULT_FWEIGHT)
	:Pin(pin),fWeight(fweight){}
}CurrentSensorConfig;

typedef struct VoltageSensorConfig{
	int Pin;
	double fWeight;
	VoltageSensorConfig(int pin=0,double fweight=DEFAULT_FWEIGHT)
		:Pin(pin),fWeight(fweight){}
}VoltageSensorConfig;

typedef struct ProbeConfig{
	VoltageSensorConfig voltageConfig;
	CurrentSensorConfig currentConfig;
	unsigned long readInterval;
	ProbeConfig(){}
	ProbeConfig(const VoltageSensorConfig& vConfig,const CurrentSensorConfig& cConfig)
		:voltageConfig(vConfig),currentConfig(cConfig){
	}
}ProbeConfig;

typedef struct ProbeControllerConfig{
	ProbeConfig	probeConfigs[PROBE_COUNT]={
		ProbeConfig(VoltageSensorConfig(PIN_PROBE1_VOLT),CurrentSensorConfig(PIN_PROBE1_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE2_VOLT),CurrentSensorConfig(PIN_PROBE2_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE3_VOLT),CurrentSensorConfig(PIN_PROBE3_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE4_VOLT),CurrentSensorConfig(PIN_PROBE4_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE5_VOLT),CurrentSensorConfig(PIN_PROBE5_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE6_VOLT),CurrentSensorConfig(PIN_PROBE6_CURRENT))
	};
}ProbeControllerConfig;

typedef struct NtcConfig{
	double aCoeff,bCoeff,cCoeff;
	int Pin;
	double fWeight;
	NtcConfig(int pin=0,double a=0,double b=0,double c=0,double fweight=DEFAULT_FWEIGHT)
			:Pin(pin),aCoeff(a),bCoeff(b),cCoeff(c),fWeight(fweight){
	}
}NtcConfig;

typedef struct HeaterConfig{
	double kp,kd,ki;
	unsigned long windowSize;
	int Pin;
	NtcConfig ntcConfig;
	HeaterConfig(const NtcConfig& ntc_config,int pin=0,int window=DEFAULT_WINDOW,double _kp=KP_DEFAULT,
		double _kd=KD_DEFAULT,double _ki=KI_DEFAULT)
		:Pin(pin),windowSize(window),kp(_kp),kd(_kd),ki(_ki),ntcConfig(ntc_config){
	}
}HeaterConfig;

typedef struct HeaterControllerConfig{
	HeaterConfig heaterConfigs[HEATER_COUNT]={
		HeaterConfig(NtcConfig(PIN_HEATER1_TEMP),PIN_HEATER1_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER2_TEMP),PIN_HEATER2_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER3_TEMP),PIN_HEATER3_HEATER)
	};
};


#pragma endregion





