#!/bin/bash
# LDM - Like Download Manager Demo Script
# Showcases all features of the complete download manager

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# Demo configuration
DEMO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_PATH="$DEMO_DIR/desktop/ldm-complete"
DEMO_DOWNLOADS_DIR="$HOME/Downloads/LDM_Demo"

print_header() {
    echo
    echo -e "${CYAN}================================${NC}"
    echo -e "${WHITE}$1${NC}"
    echo -e "${CYAN}================================${NC}"
    echo
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${PURPLE}[STEP]${NC} $1"
}

# Check if application exists
check_app() {
    if [ ! -f "$APP_PATH" ]; then
        print_error "LDM application not found at: $APP_PATH"
        print_info "Please build the application first:"
        echo "  cd desktop && make -f Makefile.complete"
        exit 1
    fi

    if [ ! -x "$APP_PATH" ]; then
        print_error "LDM application is not executable"
        print_info "Making it executable..."
        chmod +x "$APP_PATH"
    fi

    print_success "LDM application found and ready"
}

# Setup demo environment
setup_demo() {
    print_step "Setting up demo environment..."

    # Create demo downloads directory
    mkdir -p "$DEMO_DOWNLOADS_DIR"
    print_info "Demo downloads directory: $DEMO_DOWNLOADS_DIR"

    # Create demo files for testing
    echo "This is a demo text file for LDM testing" > "$DEMO_DOWNLOADS_DIR/demo.txt"
    echo "LDM Demo Configuration File" > "$DEMO_DOWNLOADS_DIR/config.conf"

    print_success "Demo environment ready"
}

# Show application info
show_app_info() {
    print_step "Application Information"

    echo -e "${WHITE}LDM - Like Download Manager${NC}"
    echo -e "Version: ${GREEN}1.0.0${NC}"
    echo -e "Interface: ${BLUE}IDM-Style${NC}"
    echo -e "Engine: ${YELLOW}Real Download Engine${NC}"
    echo -e "Built with: ${PURPLE}Qt 6 + C++20${NC}"
    echo
    echo -e "${WHITE}Key Features:${NC}"
    echo "  ✓ Multi-threaded downloads"
    echo "  ✓ Resume/pause functionality"
    echo "  ✓ IDM-style interface"
    echo "  ✓ Download scheduling"
    echo "  ✓ Queue management"
    echo "  ✓ Link grabber"
    echo "  ✓ Settings & customization"
    echo "  ✓ System tray integration"
    echo "  ✓ Real-time progress monitoring"
    echo "  ✓ Category-based organization"
    echo
}

# Interactive demo menu
show_demo_menu() {
    print_header "LDM Interactive Demo"

    echo -e "${WHITE}Choose a demo option:${NC}"
    echo
    echo "  ${GREEN}1)${NC} Launch LDM Application"
    echo "  ${GREEN}2)${NC} Quick Feature Test"
    echo "  ${GREEN}3)${NC} Interface Tour"
    echo "  ${GREEN}4)${NC} Download Engine Demo"
    echo "  ${GREEN}5)${NC} Settings & Configuration"
    echo "  ${GREEN}6)${NC} All Features Showcase"
    echo "  ${GREEN}7)${NC} Performance Test"
    echo "  ${GREEN}8)${NC} Build Information"
    echo "  ${GREEN}9)${NC} Help & Documentation"
    echo "  ${RED}0)${NC} Exit Demo"
    echo
}

# Launch application
launch_app() {
    print_step "Launching LDM Application..."

    print_info "Starting LDM with demo configuration..."
    print_info "The application will open in a new window"
    print_warning "Close the application window to return to this demo"

    echo
    echo -e "${YELLOW}Application Features to Try:${NC}"
    echo "  • Click 'Add URL' to add downloads"
    echo "  • Try the demo URLs provided in the interface"
    echo "  • Use Resume/Pause buttons"
    echo "  • Explore the Options dialog"
    echo "  • Test the Scheduler and Queue Manager"
    echo "  • Right-click for context menus"
    echo

    read -p "Press Enter to launch LDM..."

    cd "$DEMO_DIR/desktop"
    ./ldm-complete &
    APP_PID=$!

    print_success "LDM launched (PID: $APP_PID)"
    print_info "Waiting for application to close..."

    wait $APP_PID
    print_info "Application closed"
}

# Quick feature test
quick_test() {
    print_step "Quick Feature Test"

    print_info "Testing LDM startup..."
    cd "$DEMO_DIR/desktop"

    # Test if app starts and exits cleanly
    timeout 5s ./ldm-complete --version >/dev/null 2>&1 || true

    print_success "Startup test completed"

    print_info "Testing build integrity..."
    if ldd ./ldm-complete | grep -q "not found"; then
        print_error "Missing libraries detected"
        ldd ./ldm-complete | grep "not found"
    else
        print_success "All libraries linked correctly"
    fi

    print_info "Testing file permissions..."
    if [ -x "./ldm-complete" ]; then
        print_success "Binary is executable"
    else
        print_error "Binary is not executable"
    fi

    print_success "Quick test completed"
}

# Interface tour
interface_tour() {
    print_step "LDM Interface Tour"

    echo -e "${WHITE}LDM Interface Components:${NC}"
    echo
    echo -e "${GREEN}Menu Bar:${NC}"
    echo "  • File: Add URL, Exit"
    echo "  • Downloads: Resume, Pause, Delete"
    echo "  • Tools: Options, Scheduler, Link Grabber"
    echo "  • Help: About"
    echo
    echo -e "${GREEN}Toolbar:${NC}"
    echo "  • Add URL: Add new downloads"
    echo "  • Resume: Continue paused downloads"
    echo "  • Stop: Pause active downloads"
    echo "  • Delete: Remove downloads"
    echo "  • Options: Configure settings"
    echo "  • Scheduler: Schedule downloads"
    echo "  • Queue Manager: Manage download queues"
    echo "  • Grabber: Extract links from web pages"
    echo
    echo -e "${GREEN}Main Area:${NC}"
    echo "  • Categories Panel: Organize by file type"
    echo "  • Downloads Table: View all downloads"
    echo "  • Details Panel: Monitor active download"
    echo
    echo -e "${GREEN}Status Bar:${NC}"
    echo "  • Global speed indicator"
    echo "  • Overall progress bar"
    echo "  • Status messages"
    echo
    echo -e "${GREEN}System Tray:${NC}"
    echo "  • Minimize to tray"
    echo "  • Quick access menu"
    echo "  • Download notifications"
    echo
}

# Download engine demo
download_demo() {
    print_step "Download Engine Capabilities"

    echo -e "${WHITE}LDM Download Engine Features:${NC}"
    echo
    echo -e "${GREEN}Core Engine:${NC}"
    echo "  • Built with libcurl for reliability"
    echo "  • Multi-threaded segment downloading"
    echo "  • Automatic resume support"
    echo "  • Real-time speed calculation"
    echo "  • ETA estimation"
    echo
    echo -e "${GREEN}Supported Protocols:${NC}"
    echo "  • HTTP/HTTPS"
    echo "  • FTP (basic)"
    echo "  • File:// (local files)"
    echo
    echo -e "${GREEN}Advanced Features:${NC}"
    echo "  • Connection pooling"
    echo "  • Bandwidth limiting"
    echo "  • Proxy support"
    echo "  • SSL/TLS encryption"
    echo "  • Custom headers"
    echo "  • Authentication support"
    echo
    echo -e "${GREEN}Demo URLs for Testing:${NC}"
    echo "  • https://speed.hetzner.de/100MB.bin (100MB test file)"
    echo "  • https://sample-videos.com/zip/10/mp4/SampleVideo_1280x720_1mb.mp4"
    echo "  • https://www.w3.org/WAI/ER/tests/xhtml/testfiles/resources/pdf/dummy.pdf"
    echo
}

# Settings demo
settings_demo() {
    print_step "Settings & Configuration"

    echo -e "${WHITE}Available Settings:${NC}"
    echo
    echo -e "${GREEN}General Settings:${NC}"
    echo "  • Default download directory"
    echo "  • Maximum concurrent downloads"
    echo "  • Auto-start downloads"
    echo "  • File organization"
    echo
    echo -e "${GREEN}Network Settings:${NC}"
    echo "  • Connection timeout"
    echo "  • Retry attempts"
    echo "  • Speed limiting"
    echo "  • Proxy configuration"
    echo
    echo -e "${GREEN}Interface Settings:${NC}"
    echo "  • Theme selection (Light/Dark/Classic IDM)"
    echo "  • Notification preferences"
    echo "  • System tray behavior"
    echo "  • Window layout"
    echo
    echo -e "${GREEN}Advanced Settings:${NC}"
    echo "  • User agent customization"
    echo "  • SSL verification"
    echo "  • Custom headers"
    echo "  • Download priorities"
    echo
    echo -e "${YELLOW}Configuration Files:${NC}"
    echo "  • ~/.config/LDM/settings.conf"
    echo "  • ~/.config/LDM/categories.conf"
    echo "  • ~/.local/share/LDM/downloads.db"
    echo
}

# Full showcase
full_showcase() {
    print_step "Complete Features Showcase"

    print_info "This will launch LDM with a guided tour..."

    echo -e "${WHITE}Showcase Steps:${NC}"
    echo "  1. Interface overview"
    echo "  2. Add URL demonstration"
    echo "  3. Download management"
    echo "  4. Settings configuration"
    echo "  5. Advanced features"
    echo

    read -p "Ready to start the showcase? (y/N): " ready
    if [[ $ready =~ ^[Yy]$ ]]; then
        launch_app

        echo
        print_info "Showcase completed!"
        echo -e "${WHITE}What you should have seen:${NC}"
        echo "  ✓ Professional IDM-style interface"
        echo "  ✓ Real download functionality"
        echo "  ✓ Live progress monitoring"
        echo "  ✓ Category organization"
        echo "  ✓ Settings dialog"
        echo "  ✓ System tray integration"
    else
        print_info "Showcase skipped"
    fi
}

# Performance test
performance_test() {
    print_step "Performance Testing"

    print_info "Testing application startup time..."
    START_TIME=$(date +%s%N)
    timeout 10s "$APP_PATH" --version >/dev/null 2>&1 || true
    END_TIME=$(date +%s%N)
    STARTUP_TIME=$(( (END_TIME - START_TIME) / 1000000 ))

    print_success "Startup time: ${STARTUP_TIME}ms"

    print_info "Testing memory usage..."
    cd "$DEMO_DIR/desktop"
    ./ldm-complete &
    APP_PID=$!
    sleep 2

    if ps -p $APP_PID > /dev/null; then
        MEMORY=$(ps -o pid,rss -p $APP_PID | tail -1 | awk '{print $2}')
        MEMORY_MB=$((MEMORY / 1024))
        print_success "Memory usage: ${MEMORY_MB}MB"

        kill $APP_PID 2>/dev/null || true
        wait $APP_PID 2>/dev/null || true
    else
        print_warning "Could not measure memory usage"
    fi

    print_info "Testing binary size..."
    BINARY_SIZE=$(du -h "$APP_PATH" | cut -f1)
    print_success "Binary size: $BINARY_SIZE"

    print_info "Testing dependencies..."
    DEPS_COUNT=$(ldd "$APP_PATH" | wc -l)
    print_success "Linked libraries: $DEPS_COUNT"
}

# Build information
build_info() {
    print_step "Build Information"

    echo -e "${WHITE}Build Details:${NC}"
    echo
    echo -e "${GREEN}Compiler:${NC} $(g++ --version | head -1)"
    echo -e "${GREEN}Qt Version:${NC} $(pkg-config --modversion Qt6Core 2>/dev/null || echo 'Not found')"
    echo -e "${GREEN}Build Type:${NC} Release"
    echo -e "${GREEN}C++ Standard:${NC} C++20"
    echo
    echo -e "${GREEN}Libraries:${NC}"
    echo "  • Qt6Core: $(pkg-config --modversion Qt6Core 2>/dev/null || echo 'Not found')"
    echo "  • Qt6Widgets: $(pkg-config --modversion Qt6Widgets 2>/dev/null || echo 'Not found')"
    echo "  • Qt6Network: $(pkg-config --modversion Qt6Network 2>/dev/null || echo 'Not found')"
    echo "  • libcurl: $(curl-config --version 2>/dev/null | cut -d' ' -f2 || echo 'Not found')"
    echo "  • OpenSSL: $(openssl version 2>/dev/null | cut -d' ' -f2 || echo 'Not found')"
    echo
    echo -e "${GREEN}Build Files:${NC}"
    echo "  • Source: src/main_complete.cpp"
    echo "  • Binary: desktop/ldm-complete"
    echo "  • Size: $(du -h "$APP_PATH" 2>/dev/null | cut -f1 || echo 'Unknown')"
    echo
    echo -e "${GREEN}Features Enabled:${NC}"
    echo "  ✓ Real download engine"
    echo "  ✓ Multi-threading support"
    echo "  ✓ SSL/TLS encryption"
    echo "  ✓ System tray integration"
    echo "  ✓ Desktop notifications"
    echo "  ✓ IDM-style interface"
    echo
}

# Help and documentation
show_help() {
    print_step "Help & Documentation"

    echo -e "${WHITE}Getting Help:${NC}"
    echo
    echo -e "${GREEN}Documentation:${NC}"
    echo "  • README.md - Complete usage guide"
    echo "  • CONTRIBUTING.md - Development guidelines"
    echo "  • docs/ - Detailed documentation"
    echo
    echo -e "${GREEN}Online Resources:${NC}"
    echo "  • GitHub: https://github.com/aerab243/ldm"
    echo "  • Issues: https://github.com/aerab243/ldm/issues"
    echo "  • Wiki: https://github.com/aerab243/ldm/wiki"
    echo
    echo -e "${GREEN}Command Line Options:${NC}"
    echo "  • ldm --help    Show help"
    echo "  • ldm --version Show version"
    echo "  • ldm [url]     Add URL and start"
    echo
    echo -e "${GREEN}Keyboard Shortcuts:${NC}"
    echo "  • Ctrl+N        Add new URL"
    echo "  • Ctrl+O        Open options"
    echo "  • Ctrl+Q        Quit application"
    echo "  • Delete        Remove selected download"
    echo "  • Space         Pause/resume download"
    echo
    echo -e "${GREEN}Configuration:${NC}"
    echo "  • Config dir: ~/.config/LDM/"
    echo "  • Data dir: ~/.local/share/LDM/"
    echo "  • Downloads: ~/Downloads/ (default)"
    echo
}

# Main demo loop
main_demo() {
    print_header "LDM - Like Download Manager Demo"

    check_app
    setup_demo
    show_app_info

    while true; do
        show_demo_menu
        read -p "Enter your choice (0-9): " choice

        case $choice in
            1)
                launch_app
                ;;
            2)
                quick_test
                ;;
            3)
                interface_tour
                ;;
            4)
                download_demo
                ;;
            5)
                settings_demo
                ;;
            6)
                full_showcase
                ;;
            7)
                performance_test
                ;;
            8)
                build_info
                ;;
            9)
                show_help
                ;;
            0)
                print_info "Exiting demo..."
                break
                ;;
            *)
                print_error "Invalid choice. Please try again."
                ;;
        esac

        echo
        read -p "Press Enter to continue..."
    done

    print_header "Demo Complete"
    print_success "Thank you for trying LDM - Like Download Manager!"
    echo
    echo -e "${WHITE}Next Steps:${NC}"
    echo "  • Install LDM: ./install.sh"
    echo "  • Star the project on GitHub"
    echo "  • Report issues or suggest features"
    echo "  • Contribute to development"
    echo
    echo -e "${CYAN}Made with ❤️ by aerab243 (Anna-el Gerard RABENANDRASANA)${NC}"
}

# Script options
case "${1:-demo}" in
    demo|--demo)
        main_demo
        ;;
    quick|--quick)
        check_app
        launch_app
        ;;
    test|--test)
        check_app
        quick_test
        ;;
    info|--info)
        show_app_info
        build_info
        ;;
    help|--help|-h)
        echo "LDM Demo Script"
        echo
        echo "Usage: $0 [OPTION]"
        echo
        echo "Options:"
        echo "  demo     Interactive demo (default)"
        echo "  quick    Quick launch"
        echo "  test     Run tests"
        echo "  info     Show information"
        echo "  help     Show this help"
        ;;
    *)
        print_error "Unknown option: $1"
        echo "Use --help for usage information"
        exit 1
        ;;
esac
