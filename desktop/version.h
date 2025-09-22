#ifndef VERSION_H
#define VERSION_H

// LDM - Like Download Manager Version Information
// This file contains version constants for the application

#define LDM_VERSION_MAJOR 1
#define LDM_VERSION_MINOR 0
#define LDM_VERSION_PATCH 0

#define LDM_VERSION_STRING "1.0.0"
#define LDM_VERSION_FULL "LDM - Like Download Manager v1.0.0"

// Build information
#define LDM_BUILD_DATE __DATE__
#define LDM_BUILD_TIME __TIME__

// Application information
#define LDM_APP_NAME "LDM"
#define LDM_APP_DISPLAY_NAME "LDM - Like Download Manager"
#define LDM_APP_DESCRIPTION "A powerful download manager with IDM-style interface"
#define LDM_APP_COMPANY "aerab243"
#define LDM_APP_COPYRIGHT "Copyright (C) 2025 aerab243 (Anna-el Gerard RABENANDRASANA)"
#define LDM_APP_URL "https://github.com/aerab243/ldm"

// User agent string for HTTP requests
#define LDM_USER_AGENT "LDM/1.0.0 (Like Download Manager)"

// Version comparison macros
#define LDM_VERSION_NUMBER ((LDM_VERSION_MAJOR << 16) | (LDM_VERSION_MINOR << 8) | LDM_VERSION_PATCH)
#define LDM_VERSION_CHECK(major, minor, patch) \
    (LDM_VERSION_NUMBER >= (((major) << 16) | ((minor) << 8) | (patch)))

// Release information
#define LDM_RELEASE_TYPE "Release"
#define LDM_IS_STABLE_RELEASE 1
#define LDM_IS_BETA_RELEASE 0
#define LDM_IS_ALPHA_RELEASE 0

#endif // VERSION_H