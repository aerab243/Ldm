#!/bin/bash

# Linux Download Manager (LDM) Build Script
# Simple build for ldm with ldm-cli

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Linux Download Manager Build Script ===${NC}"
echo -e "${BLUE}Building ldm with ldm-cli${NC}"

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Build directory
BUILD_DIR="$PROJECT_ROOT/desktop/build"

# Clean previous build
echo -e "${YELLOW}Cleaning previous build...${NC}"
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"/*
fi
mkdir -p "$BUILD_DIR"

# Change to build directory
cd "$BUILD_DIR"

# Configure with CMake
echo -e "${YELLOW}Configuring build with CMake...${NC}"
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_CLI=ON \
    -DBUILD_TESTING=OFF

# Build CLI first
echo -e "${YELLOW}Building ldm-cli...${NC}"
make ldm-cli -j$(nproc)

# Build GUI if Qt6 is available
if pkg-config --exists Qt6Core Qt6Widgets; then
    echo -e "${YELLOW}Building GUI (ldm-gui-simple)...${NC}"
    make ldm-gui-simple -j$(nproc)
else
    echo -e "${YELLOW}Qt6 not found, skipping GUI build${NC}"
fi

# Check if CLI build was successful
if [ -f "$BUILD_DIR/ldm-cli" ]; then
    echo -e "${GREEN}✓ CLI build completed successfully!${NC}"
else
    echo -e "${RED}✗ CLI build failed!${NC}"
    exit 1
fi

# Check GUI build
if [ -f "$BUILD_DIR/ldm-gui-simple" ]; then
    echo -e "${GREEN}✓ GUI build completed successfully!${NC}"
    HAS_GUI=1
else
    echo -e "${YELLOW}! GUI not built (Qt6 dependency missing)${NC}"
    HAS_GUI=0
fi

echo -e "${GREEN}Built files:${NC}"
echo -e "  ${GREEN}• ldm-cli (Command Line Interface)${NC}"
if [ $HAS_GUI -eq 1 ]; then
    echo -e "  ${GREEN}• ldm-gui-simple (Graphical Interface)${NC}"
fi

echo ""
echo -e "${BLUE}Usage:${NC}"
echo -e "  ${BLUE}CLI: ./ldm or cd desktop/build && ./ldm-cli${NC}"
if [ $HAS_GUI -eq 1 ]; then
    echo -e "  ${BLUE}GUI: ./ldm-gui or cd desktop/build && ./ldm-gui-simple${NC}"
fi
echo -e "  ${BLUE}Demo CLI: ./demo-cli.sh${NC}"
if [ $HAS_GUI -eq 1 ]; then
    echo -e "  ${BLUE}Demo GUI: ./demo-gui.sh${NC}"
fi
echo ""
echo -e "${BLUE}Install: cd desktop/build && sudo make install${NC}"
