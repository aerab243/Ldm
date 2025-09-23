#!/bin/bash

# Build script for LDM CLI version
# Simple command-line interface that works without Qt GUI components

set -e

echo "======================================"
echo "Building LDM CLI Version"
echo "======================================"
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if we're in the correct directory
if [ ! -d "desktop" ]; then
    echo -e "${RED}Error: Please run this script from the ldm root directory${NC}"
    exit 1
fi

echo -e "${BLUE}Step 1: Building CLI version...${NC}"

cd desktop

# Build the CLI version
echo -e "${CYAN}Compiling LDM CLI...${NC}"
if g++ -std=c++17 -O2 src/main_cli_simple.cpp -o ldm-cli; then
    echo -e "${GREEN}✓ CLI version compiled successfully${NC}"
else
    echo -e "${RED}✗ CLI compilation failed${NC}"
    exit 1
fi

echo
echo -e "${BLUE}Step 2: Testing CLI version...${NC}"

echo -e "${CYAN}Testing version command...${NC}"
if ./ldm-cli --version; then
    echo -e "${GREEN}✓ Version command works${NC}"
else
    echo -e "${RED}✗ Version command failed${NC}"
    exit 1
fi

echo
echo -e "${CYAN}Testing help command...${NC}"
if ./ldm-cli --help >/dev/null 2>&1; then
    echo -e "${GREEN}✓ Help command works${NC}"
else
    echo -e "${RED}✗ Help command failed${NC}"
    exit 1
fi

echo
echo -e "${CYAN}Testing info command...${NC}"
if ./ldm-cli --info >/dev/null 2>&1; then
    echo -e "${GREEN}✓ Info command works${NC}"
else
    echo -e "${RED}✗ Info command failed${NC}"
    exit 1
fi

cd ..

echo
echo -e "${BLUE}Step 3: Creating launch script...${NC}"

cat > launch_ldm_cli.sh << 'EOF'
#!/bin/bash

# LDM CLI Launch Script

cd "$(dirname "$0")"

if [ -f "desktop/ldm-cli" ]; then
    ./desktop/ldm-cli "$@"
else
    echo "Error: LDM CLI not found. Please run build_cli.sh first."
    exit 1
fi
EOF

chmod +x launch_ldm_cli.sh

echo -e "${GREEN}✓ Created launch_ldm_cli.sh${NC}"

echo
echo "======================================"
echo -e "${GREEN}Build Complete!${NC}"
echo "======================================"
echo
echo -e "${YELLOW}Available executables:${NC}"
echo -e "${GREEN}✓ desktop/ldm-cli${NC}        - Command line interface"
echo -e "${GREEN}✓ launch_ldm_cli.sh${NC}     - Convenient launcher"

echo
echo -e "${YELLOW}Usage examples:${NC}"
echo -e "${GREEN}./desktop/ldm-cli --version${NC}"
echo -e "${GREEN}./desktop/ldm-cli --help${NC}"
echo -e "${GREEN}./desktop/ldm-cli --info${NC}"
echo -e "${GREEN}./desktop/ldm-cli --status${NC}"
echo -e "${GREEN}./desktop/ldm-cli --add-url \"https://example.com/file.zip\"${NC}"
echo -e "${GREEN}./launch_ldm_cli.sh --version${NC}"

echo
echo -e "${CYAN}The CLI version provides:${NC}"
echo "• Version and help information"
echo "• System status reporting"
echo "• URL addition demonstration"
echo "• Full command-line interface"
echo "• No GUI dependencies required"

echo
echo -e "${BLUE}Developer: Anna-el Gerard RABENANDRASANA (aerab243)${NC}"
echo -e "${BLUE}Project: https://github.com/aerab243/ldm${NC}"
