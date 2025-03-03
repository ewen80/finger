#include "my_common.h"

const char* eventToString(my_event_t event) {
    switch(event) {
        case TOUCH_START: return "TOUCH_START";
        case PRESSURE_END: return "PRESSURE_END";
        case PRESSURE_START: return "PRESSURE_START";
        case SERVO_RUNBACK: return "SERVO_RUNBACK";
        case SERVO_END: return "SERVO_END";
        case FINGER_TOUCHED: return "FINGER_TOUCHED";
        default: return "UNKNOWN";
    }
  }