#pragma once



#ifdef INCLUDE_COM
#include "Communication/ComHandler.hpp"
#include "Communication/SerialData.hpp"
#include "Communication/Serializable.hpp"
#endif

#ifdef INCLUDE_HEATER
#include "Heaters/HeaterConfiguration.hpp"
#include "Heaters/HeaterController.hpp"
    #ifdef INCLUDE_HEATER_INTERNAL
        #include "Heaters/Heater.hpp"
        #include "Heaters/TemperatureSensor.hpp"
        #include "Heaters/PID/PID.hpp"
        #include "Heaters/PID/PID_AutoTune.hpp"
    #endif
#endif

#ifdef INCLUDE_PROBE
#include "Probes/Probe.hpp"
#include "Probes/ProbeConfiguration.hpp"
    #ifdef INCLUDE_PROBE_INTERNAL
        #include "Probes/Probe.hpp"
        #include "Probes/VoltageSensor.hpp"
        #include "Probes/CurrentSensor.hpp"
    #endif
#endif

#ifdef INCLUDE_CONTROLLER
#include "Controller/Controller.hpp"
#include "Controller/ControllerConfiguration.hpp"
    #ifdef INCLUDE_CONTROLLER_INTERNAL
        #include "Controller/CurrentSelector.hpp"
    #endif
#endif

#ifdef INCLUDE_LOGGER
#include "Logging/StationLogger.hpp"
    #ifdef INCLUDE_LOGGER_TESTS
        #include "Logging/Tests/LoggingTests.hpp"
    #endif
#endif
