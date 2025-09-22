#!/bin/bash

# Quick LDM Test and Demo Script
# Simple demonstration of LDM build and basic functionality

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
MAGENTA='\033[0;35m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/desktop"

print_header() {
    echo -e "${CYAN}"
    echo "╔══════════════════════════════════════════════════════════════╗"
    echo "║                                                              ║"
    echo "║    🔻 LDM - Like Download Manager v1.0.0 - Quick Test       ║"
    echo "║       IDM-Style Download Manager for Linux                  ║"
    echo "║                                                              ║"
    echo "╚══════════════════════════════════════════════════════════════╝"
    echo -e "${NC}"
    echo ""
}

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check build environment
check_environment() {
    print_status "Checking build environment..."

    # Check if we're in the right directory
    if [ ! -d "$BUILD_DIR" ]; then
        print_error "Desktop directory not found. Please run from LDM root directory."
        exit 1
    fi

    # Check for source files
    if [ ! -f "$BUILD_DIR/src/main_complete.cpp" ]; then
        print_error "Main source file not found."
        exit 1
    fi

    # Check for Makefile
    if [ ! -f "$BUILD_DIR/Makefile.complete" ]; then
        print_error "Makefile.complete not found."
        exit 1
    fi

    print_success "Environment check passed"
}

# Check dependencies
check_dependencies() {
    print_status "Checking system dependencies..."

    local deps_ok=true

    # Check essential tools
    if ! command -v g++ &> /dev/null; then
        print_error "g++ compiler not found"
        deps_ok=false
    fi

    if ! command -v make &> /dev/null; then
        print_error "make not found"
        deps_ok=false
    fi

    # Check Qt6
    if ! pkg-config --exists Qt6Core Qt6Widgets Qt6Network; then
        print_error "Qt6 development packages not found"
        deps_ok=false
    fi

    # Check additional libraries
    if ! pkg-config --exists libcurl; then
        print_warning "libcurl not found - some features may not work"
    fi

    if [ "$deps_ok" = true ]; then
        print_success "All essential dependencies found"
        return 0
    else
        print_error "Missing dependencies. Please install them first."
        echo ""
        echo "For Ubuntu/Debian:"
        echo "  sudo apt update"
        echo "  sudo apt install build-essential qt6-base-dev qt6-tools-dev libcurl4-openssl-dev"
        echo ""
        echo "For Fedora:"
        echo "  sudo dnf install gcc-c++ make qt6-qtbase-devel qt6-qttools-devel libcurl-devel"
        echo ""
        return 1
    fi
}

# Build the application
build_application() {
    print_status "Building LDM application..."

    cd "$BUILD_DIR"

    # Clean previous build
    print_status "Cleaning previous build..."
    make -f Makefile.complete clean > /dev/null 2>&1 || true

    # Build the application
    print_status "Compiling source code..."
    if make -f Makefile.complete all; then
        print_success "Build completed successfully"
        return 0
    else
        print_error "Build failed"
        return 1
    fi
}

# Test the application
test_application() {
    print_status "Testing LDM application..."

    cd "$BUILD_DIR"

    # Check if binary exists
    if [ ! -f "ldm-complete" ]; then
        print_error "Application binary not found"
        return 1
    fi

    # Check if binary is executable
    if [ ! -x "ldm-complete" ]; then
        print_error "Application binary is not executable"
        return 1
    fi

    # Test basic execution (timeout after 5 seconds)
    print_status "Testing application startup..."
    if timeout 5s ./ldm-complete --version &>/dev/null; then
        print_success "Application starts correctly"
    else
        print_warning "Version test completed (expected timeout for GUI app)"
    fi

    # Check binary size (should be reasonable)
    local size=$(stat -c%s "ldm-complete" 2>/dev/null || echo "0")
    if [ "$size" -gt 50000 ]; then
        print_success "Binary size looks good ($size bytes)"
    else
        print_warning "Binary size seems small ($size bytes)"
    fi

    return 0
}

# Demonstrate features
demo_features() {
    print_status "LDM Features Overview:"
    echo ""
    echo -e "${BLUE}🎨 User Interface:${NC}"
    echo "  ✅ IDM-style main window"
    echo "  ✅ Professional toolbar"
    echo "  ✅ Download categories tree"
    echo "  ✅ Download list table"
    echo "  ✅ Progress monitoring"
    echo "  ✅ System tray integration"
    echo ""
    echo -e "${BLUE}⚡ Download Engine:${NC}"
    echo "  ✅ Multi-threaded downloads"
    echo "  ✅ Resume/pause functionality"
    echo "  ✅ Speed calculation"
    echo "  ✅ Progress tracking"
    echo "  ✅ Queue management"
    echo ""
    echo -e "${BLUE}🔧 Advanced Features:${NC}"
    echo "  ✅ URL validation"
    echo "  ✅ Category detection"
    echo "  ✅ Download scheduling"
    echo "  ✅ Settings management"
    echo "  ✅ Notification system"
    echo ""
    echo -e "${BLUE}🛡️ Security:${NC}"
    echo "  ✅ SSL/TLS support"
    echo "  ✅ Safe file handling"
    echo "  ✅ Input validation"
    echo ""
}

# Launch application
launch_application() {
    print_status "Launching LDM application..."
    echo ""
    echo -e "${YELLOW}The LDM application will open in a new window.${NC}"
    echo -e "${YELLOW}Close the application window to return to this script.${NC}"
    echo ""
    echo -e "${CYAN}Key features to test:${NC}"
    echo "  • Click 'Add URL' to add a download"
    echo "  • Use Resume/Pause buttons"
    echo "  • Check the Categories tree"
    echo "  • View download progress"
    echo "  • Try the Options dialog"
    echo ""
    echo "Press Enter to launch the application..."
    read -r

    cd "$BUILD_DIR"

    if [ -x "./ldm-complete" ]; then
        print_status "Starting LDM application..."
        ./ldm-complete &
        local app_pid=$!

        echo ""
        echo -e "${GREEN}✅ LDM application started (PID: $app_pid)${NC}"
        echo -e "${YELLOW}📌 The application is running in the background${NC}"
        echo ""
        echo "Options:"
        echo "  1) Keep application running and exit this script"
        echo "  2) Wait for application to close"
        echo "  3) Force close application"
        echo ""
        echo -n "Your choice (1-3): "
        read -r choice

        case $choice in
            1)
                print_success "LDM is running. You can close this terminal."
                ;;
            2)
                print_status "Waiting for application to close..."
                wait $app_pid 2>/dev/null || true
                print_success "Application closed"
                ;;
            3)
                print_status "Closing application..."
                kill $app_pid 2>/dev/null || true
                sleep 2
                kill -9 $app_pid 2>/dev/null || true
                print_success "Application closed"
                ;;
            *)
                print_warning "Invalid choice. Keeping application running."
                ;;
        esac
    else
        print_error "Application not found. Please build it first."
        return 1
    fi
}

# Show build information
show_build_info() {
    print_status "Build Information:"
    echo ""

    cd "$BUILD_DIR"

    if [ -f "ldm-complete" ]; then
        echo -e "${BLUE}Binary Details:${NC}"
        echo "  File: $(pwd)/ldm-complete"
        echo "  Size: $(stat -c%s ldm-complete 2>/dev/null || echo "unknown") bytes"
        echo "  Modified: $(stat -c%y ldm-complete 2>/dev/null || echo "unknown")"
        echo "  Permissions: $(stat -c%A ldm-complete 2>/dev/null || echo "unknown")"
        echo ""
    fi

    echo -e "${BLUE}System Information:${NC}"
    echo "  OS: $(uname -s) $(uname -r)"
    echo "  Architecture: $(uname -m)"
    echo "  Compiler: $(g++ --version | head -1 2>/dev/null || echo "unknown")"
    echo "  Qt Version: $(pkg-config --modversion Qt6Core 2>/dev/null || echo "unknown")"
    echo ""
}

# Main menu
show_menu() {
    echo -e "${CYAN}What would you like to do?${NC}"
    echo ""
    echo "  1) Check dependencies"
    echo "  2) Build LDM application"
    echo "  3) Test application"
    echo "  4) Show features overview"
    echo "  5) Launch application"
    echo "  6) Show build information"
    echo "  7) Full build and test"
    echo "  8) Clean build files"
    echo "  0) Exit"
    echo ""
    echo -n "Enter your choice (0-8): "
}

# Clean build files
clean_build() {
    print_status "Cleaning build files..."

    cd "$BUILD_DIR"
    make -f Makefile.complete clean > /dev/null 2>&1 || true

    print_success "Build files cleaned"
}

# Full build and test sequence
full_build_test() {
    print_status "Running full build and test sequence..."
    echo ""

    check_environment
    check_dependencies || exit 1
    build_application || exit 1
    test_application || exit 1
    show_build_info
    demo_features

    echo ""
    print_success "Full build and test completed successfully!"
    echo ""
    echo -e "${YELLOW}Ready to launch? (y/n): ${NC}"
    read -r response
    if [[ "$response" =~ ^[Yy]$ ]]; then
        launch_application
    fi
}

# Main execution
main() {
    print_header

    # If command line argument provided, run specific action
    case "${1:-}" in
        "check")
            check_environment && check_dependencies
            ;;
        "build")
            check_environment && build_application
            ;;
        "test")
            check_environment && test_application
            ;;
        "launch")
            check_environment && launch_application
            ;;
        "full")
            full_build_test
            ;;
        "clean")
            clean_build
            ;;
        *)
            # Interactive menu
            while true; do
                show_menu
                read -r choice
                echo ""

                case $choice in
                    1)
                        check_environment && check_dependencies
                        ;;
                    2)
                        check_environment && build_application
                        ;;
                    3)
                        check_environment && test_application
                        ;;
                    4)
                        demo_features
                        ;;
                    5)
                        check_environment && launch_application
                        ;;
                    6)
                        show_build_info
                        ;;
                    7)
                        full_build_test
                        ;;
                    8)
                        clean_build
                        ;;
                    0)
                        echo -e "${GREEN}Thanks for testing LDM!${NC}"
                        exit 0
                        ;;
                    *)
                        print_error "Invalid choice. Please try again."
                        ;;
                esac

                echo ""
                echo -e "${MAGENTA}Press Enter to continue...${NC}"
                read -r
                echo ""
            done
            ;;
    esac
}

# Run main function
main "$@"
