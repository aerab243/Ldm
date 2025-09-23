#!/bin/bash

# Build script for LDM Simple GUI with icon support
# Usage: ./build_simple_gui.sh

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Linux Download Manager - Simple GUI Build Script ===${NC}"
echo -e "${BLUE}Building with icon support...${NC}"
echo

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo -e "${RED}Error: CMakeLists.txt not found. Please run this script from the desktop directory.${NC}"
    exit 1
fi

# Check if Qt6 is available
if ! command -v qmake6 &> /dev/null && ! command -v qmake-qt6 &> /dev/null; then
    echo -e "${YELLOW}Warning: Qt6 qmake not found. Make sure Qt6 is installed.${NC}"
fi

# Create build directory
BUILD_DIR="build_simple"
echo -e "${BLUE}Creating build directory: ${BUILD_DIR}${NC}"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure with CMake
echo -e "${BLUE}Configuring with CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the simple GUI
echo -e "${BLUE}Building ldm-gui-simple...${NC}"
make ldm-gui-simple -j$(nproc)

# Check if build was successful
if [ -f "ldm-gui-simple" ]; then
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo -e "${GREEN}✓ Executable: ${PWD}/ldm-gui-simple${NC}"
    echo

    # Show file info
    echo -e "${BLUE}Executable information:${NC}"
    file ldm-gui-simple
    ls -lh ldm-gui-simple
    echo

    # Check if Qt resources are embedded
    echo -e "${BLUE}Checking for embedded resources...${NC}"
    if strings ldm-gui-simple | grep -q "logo-ldm-opensource"; then
        echo -e "${GREEN}✓ Icon resources found in executable${NC}"
    else
        echo -e "${YELLOW}⚠ Icon resources might not be embedded${NC}"
    fi
    echo

    # Option to run the application
    echo -e "${BLUE}Do you want to run the application now? [y/N]${NC}"
    read -r response
    if [[ "$response" =~ ^([yY][eE][sS]|[yY])$ ]]; then
        echo -e "${GREEN}Starting LDM Simple GUI...${NC}"
        ./ldm-gui-simple &
        echo -e "${GREEN}Application started in background (PID: $!)${NC}"
    fi

else
    echo -e "${RED}✗ Build failed!${NC}"
    exit 1
fi

echo
echo -e "${GREEN}=== Build Complete ===${NC}"
echo -e "${BLUE}To run manually: cd ${BUILD_DIR} && ./ldm-gui-simple${NC}"
echo -e "${BLUE}To install system-wide: sudo make install${NC}"
