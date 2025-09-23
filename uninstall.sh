#!/bin/bash

# LDM Uninstall Script for Linux
# This script removes all installed LDM files and cleans up the system

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
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

# Function to check if running with sudo
check_sudo() {
    if [ "$EUID" -ne 0 ]; then
        print_error "This script must be run with sudo privileges"
        echo "Usage: sudo $0"
        exit 1
    fi
}

# Function to confirm uninstall
confirm_uninstall() {
    print_warning "This will completely remove LDM from your system."
    echo "The following will be removed:"
    echo "  - LDM executables from /usr/local/bin/"
    echo "  - Desktop file from /usr/local/share/applications/"
    echo "  - Icons from /usr/local/share/pixmaps/ and /usr/local/share/icons/"
    echo "  - Configuration files from ~/.config/LDM/"
    echo "  - Download history and settings"
    echo ""
    read -p "Are you sure you want to continue? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_status "Uninstall cancelled."
        exit 0
    fi
}

# Function to remove installed files
remove_installed_files() {
    print_status "Removing installed LDM files..."

    # Remove executables
    rm -f /usr/local/bin/ldm-complete
    rm -f /usr/local/bin/ldm-gui-simple
    rm -f /usr/local/bin/ldm-cli
    rm -f /usr/local/bin/ldm-native-host

    # Remove desktop file
    rm -f /usr/local/share/applications/ldm.desktop

    # Remove icons
    rm -f /usr/local/share/pixmaps/ldm.png
    rm -f /usr/local/share/icons/hicolor/scalable/apps/ldm.svg

    print_success "Installed files removed"
}

# Function to remove user configuration
remove_user_config() {
    print_status "Removing user configuration files..."

    local config_dir="$HOME/.config/LDM"
    if [ -d "$config_dir" ]; then
        rm -rf "$config_dir"
        print_success "Configuration directory removed: $config_dir"
    else
        print_status "No configuration directory found"
    fi

    # Also check for any other potential locations
    local cache_dir="$HOME/.cache/LDM"
    if [ -d "$cache_dir" ]; then
        rm -rf "$cache_dir"
        print_success "Cache directory removed: $cache_dir"
    fi

    local data_dir="$HOME/.local/share/LDM"
    if [ -d "$data_dir" ]; then
        rm -rf "$data_dir"
        print_success "Data directory removed: $data_dir"
    fi
}

# Function to clean up desktop integration
cleanup_desktop_integration() {
    print_status "Cleaning up desktop integration..."

    # Remove from application menu (if using update-desktop-database)
    if command -v update-desktop-database &> /dev/null; then
        update-desktop-database /usr/local/share/applications/ 2>/dev/null || true
    fi

    # Remove from icon cache (if using gtk-update-icon-cache)
    if command -v gtk-update-icon-cache &> /dev/null; then
        gtk-update-icon-cache /usr/local/share/icons/hicolor/ 2>/dev/null || true
    fi

    print_success "Desktop integration cleaned up"
}

# Function to check for remaining files
check_remaining_files() {
    print_status "Checking for any remaining LDM files..."

    local found_files=false

    # Check for executables
    if [ -f /usr/local/bin/ldm-complete ] || [ -f /usr/local/bin/ldm-gui-simple ] || [ -f /usr/local/bin/ldm-cli ]; then
        print_warning "Some LDM executables may still exist"
        found_files=true
    fi

    # Check for desktop file
    if [ -f /usr/local/share/applications/ldm.desktop ]; then
        print_warning "Desktop file may still exist"
        found_files=true
    fi

    # Check for icons
    if [ -f /usr/local/share/pixmaps/ldm.png ] || [ -f /usr/local/share/icons/hicolor/scalable/apps/ldm.svg ]; then
        print_warning "Some icons may still exist"
        found_files=true
    fi

    if [ "$found_files" = false ]; then
        print_success "No remaining LDM files found"
    fi
}

# Main uninstall process
main() {
    print_status "Starting LDM uninstallation..."

    # Check sudo
    check_sudo

    # Confirm uninstall
    confirm_uninstall

    # Remove installed files
    remove_installed_files

    # Remove user configuration
    remove_user_config

    # Clean up desktop integration
    cleanup_desktop_integration

    # Check for remaining files
    check_remaining_files

    print_success "LDM uninstallation completed!"
    echo ""
    echo "LDM has been completely removed from your system."
    echo "If you want to reinstall LDM later, you can run the install.sh script again."
}

# Run main function
main "$@"
