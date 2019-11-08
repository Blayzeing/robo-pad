/*
--------------------------------------------------------------------
Author: Blayze F Millward

This header file contains global configuration settings for
everything in the RoboPad ecosystem, hopefully easing transition
from arduino to ESP and back.
---------------------------------------------------------------------
*/
#ifndef GLOBAL_ROBOPAD_CONFIG_H // Guard.
#define GLOBAL_ROBOPAD_CONFIG_H

// Add PWMRANGE definitions for non-ESP chips (i.e. Arduinos)
// It is assumed that an arduino (with 8-bit resolution) is used if not an ESP.
// If additional hardware support is added, PWMRANGE will have to be defined in the new hardware correctly.
#ifndef PWMRANGE
#define PWMRANGE 254
#endif

#endif
