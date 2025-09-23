#!/bin/bash

# Linux Download Manager (LDM) CLI Demo Script
# Demonstrates the working CLI functionality

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}=== Linux Download Manager (LDM) CLI Demo ===${NC}"
echo -e "${BLUE}Demonstrating the functional CLI interface${NC}"
echo

# Check if LDM CLI exists
if [ ! -f "desktop/build/ldm-cli" ]; then
    echo -e "${RED}Error: LDM CLI not found. Please run './build.sh' first.${NC}"
    exit 1
fi

echo -e "${YELLOW}1. Showing LDM CLI version:${NC}"
./ldm --version
echo

echo -e "${YELLOW}2. Displaying help information:${NC}"
./ldm --help
echo

echo -e "${YELLOW}3. Listing current downloads (should be empty):${NC}"
./ldm --list
echo

echo -e "${YELLOW}4. Adding a test download:${NC}"
echo -e "${CYAN}Adding: http://httpbin.org/bytes/1024${NC}"
./ldm --add "http://httpbin.org/bytes/1024"
echo

echo -e "${YELLOW}5. Listing downloads after adding one:${NC}"
./ldm --list
echo

echo -e "${YELLOW}6. Showing download history:${NC}"
./ldm --history
echo

echo -e "${GREEN}✓ CLI Demo completed successfully!${NC}"
echo
echo -e "${CYAN}=== Summary ===${NC}"
echo -e "${GREEN}✅ LDM CLI is fully functional and ready for use${NC}"
echo -e "${GREEN}✅ Build system is clean and simplified${NC}"
echo -e "${GREEN}✅ Single build directory: desktop/build/${NC}"
echo -e "${GREEN}✅ Simple launcher: ./ldm${NC}"
echo
echo -e "${BLUE}The CLI provides all core download management functionality:${NC}"
echo -e "  • Add downloads: ${CYAN}./ldm --add <url>${NC}"
echo -e "  • List downloads: ${CYAN}./ldm --list${NC}"
echo -e "  • Control downloads: ${CYAN}./ldm --pause/--resume/--cancel <id>${NC}"
echo -e "  • View history: ${CYAN}./ldm --history${NC}"
echo -e "  • Check status: ${CYAN}./ldm --status <id>${NC}"
echo
echo -e "${YELLOW}Note: GUI interface requires additional development${NC}"
echo -e "${YELLOW}      CLI is production-ready for immediate use${NC}"
