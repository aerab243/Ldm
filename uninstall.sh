#!/bin/bash

# LDM - Linux Download Manager Uninstall Script
# Comprehensive removal of LDM installation and configuration files
# Developer: Anna-el Gerard RABENANDRASANA (aerab243)
# Project: https://github.com/aerab243/ldm

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

echo "=============================================="
echo -e "${RED}LDM - Linux Download Manager Uninstaller${NC}"
echo "=============================================="
echo
echo -e "${YELLOW}This will completely remove LDM from your system.${NC}"
echo

# Function to ask user confirmation
ask_confirmation() {
    echo -e "${CYAN}$1${NC}"
    read -p "Continue? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}Operation cancelled.${NC}"
        exit 0
    fi
}

# Function to remove file/directory with confirmation
remove_path() {
    local path="$1"
    local description="$2"

    if [ -e "$path" ]; then
        echo -e "${CYAN}Removing $description...${NC}"
        echo "  Path: $path"

        if [ -d "$path" ]; then
            rm -rf "$path"
        else
            rm -f "$path"
        fi

        if [ ! -e "$path" ]; then
            echo -e "${GREEN}✓ Removed successfully${NC}"
        else
            echo -e "${RED}✗ Failed to remove${NC}"
        fi
        echo
    else
        echo -e "${YELLOW}⚠ $description not found${NC}"
        echo "  Path: $path"
        echo
    fi
}

# Show what will be removed
echo -e "${WHITE}What will be removed:${NC}"
echo "• LDM executables and binaries"
echo "• Desktop application files"
echo "• System installation files (/usr/local/bin/, /usr/bin/)"
echo "• User configuration files (~/.config/LDM/)"
echo "• User data files (~/.local/share/LDM/)"
echo "• Desktop entries and shortcuts"
echo "• Cache and temporary files"
echo "• Browser integration files"
echo "• System service files"
echo

ask_confirmation "Do you want to proceed with uninstallation?"

echo -e "${BLUE}Starting LDM uninstallation...${NC}"
echo

# 1. Remove local build files and executables
echo -e "${WHITE}=== Removing Local Build Files ===${NC}"

if [ -d "desktop" ]; then
    echo -e "${CYAN}Removing desktop build files...${NC}"

    # Remove executables
    rm -f desktop/ldm-working
    rm -f desktop/ldm-cli
    rm -f desktop/ldm-complete*
    rm -f desktop/ldm-idm*
    rm -f desktop/test_*
    rm -f desktop/quick_*
    rm -f desktop/*.o
    rm -f desktop/*.moc
    rm -f desktop/qrc_*
    rm -f desktop/build.log
    rm -f desktop/test*.log

    # Remove build directories
    rm -rf desktop/build
    rm -rf desktop/CMakeFiles

    echo -e "${GREEN}✓ Local build files removed${NC}"
else
    echo -e "${YELLOW}⚠ Desktop directory not found${NC}"
fi

echo

# 2. Remove system-wide installations
echo -e "${WHITE}=== Removing System Installations ===${NC}"

# Check for system installations
SYSTEM_LOCATIONS=(
    "/usr/local/bin/ldm"
    "/usr/local/bin/ldm-cli"
    "/usr/local/bin/ldm-desktop"
    "/usr/local/bin/ldm-working"
    "/usr/bin/ldm"
    "/usr/bin/ldm-cli"
    "/usr/bin/ldm-desktop"
    "/opt/ldm"
    "/usr/share/applications/ldm.desktop"
    "/usr/share/applications/ldm-desktop.desktop"
    "/usr/local/share/applications/ldm.desktop"
    "/usr/share/pixmaps/ldm.png"
    "/usr/share/icons/hicolor/*/apps/ldm.png"
    "/usr/local/share/pixmaps/ldm.png"
)

SYSTEM_FOUND=false
for location in "${SYSTEM_LOCATIONS[@]}"; do
    if [ -e "$location" ] || [ -L "$location" ]; then
        SYSTEM_FOUND=true
        break
    fi
done

if [ "$SYSTEM_FOUND" = true ]; then
    echo -e "${YELLOW}System-wide installation detected. Root privileges required.${NC}"

    if command -v sudo >/dev/null 2>&1; then
        ask_confirmation "Remove system-wide files with sudo?"

        echo -e "${CYAN}Removing system executables...${NC}"
        sudo rm -f /usr/local/bin/ldm* /usr/bin/ldm*

        echo -e "${CYAN}Removing desktop entries...${NC}"
        sudo rm -f /usr/share/applications/ldm*.desktop
        sudo rm -f /usr/local/share/applications/ldm*.desktop

        echo -e "${CYAN}Removing icons...${NC}"
        sudo rm -f /usr/share/pixmaps/ldm*
        sudo rm -f /usr/local/share/pixmaps/ldm*
        sudo rm -rf /usr/share/icons/hicolor/*/apps/ldm*

        echo -e "${CYAN}Removing system directories...${NC}"
        sudo rm -rf /opt/ldm

        echo -e "${GREEN}✓ System installation removed${NC}"
    else
        echo -e "${RED}✗ sudo not available. Please remove system files manually as root:${NC}"
        echo "  rm -f /usr/local/bin/ldm* /usr/bin/ldm*"
        echo "  rm -f /usr/share/applications/ldm*.desktop"
        echo "  rm -f /usr/share/pixmaps/ldm*"
        echo "  rm -rf /opt/ldm"
    fi
else
    echo -e "${GREEN}✓ No system-wide installation found${NC}"
fi

echo

# 3. Remove user configuration and data
echo -e "${WHITE}=== Removing User Data ===${NC}"

remove_path "$HOME/.config/LDM" "user configuration files"
remove_path "$HOME/.config/ldm" "user configuration files (lowercase)"
remove_path "$HOME/.local/share/LDM" "user data files"
remove_path "$HOME/.local/share/ldm" "user data files (lowercase)"
remove_path "$HOME/.cache/LDM" "user cache files"
remove_path "$HOME/.cache/ldm" "user cache files (lowercase)"

# Remove Qt settings
if [ -f "$HOME/.config/aerab243/LDM.conf" ]; then
    remove_path "$HOME/.config/aerab243/LDM.conf" "Qt settings file"
fi

# Remove desktop shortcuts
remove_path "$HOME/.local/share/applications/ldm.desktop" "user desktop entry"
remove_path "$HOME/Desktop/LDM.desktop" "desktop shortcut"

# 4. Remove browser integration
echo -e "${WHITE}=== Removing Browser Integration ===${NC}"

# Chrome/Chromium native messaging
CHROME_LOCATIONS=(
    "$HOME/.config/google-chrome/NativeMessagingHosts/com.aerab243.ldm.json"
    "$HOME/.config/chromium/NativeMessagingHosts/com.aerab243.ldm.json"
    "/etc/chromium/native-messaging-hosts/com.aerab243.ldm.json"
    "/etc/opt/chrome/native-messaging-hosts/com.aerab243.ldm.json"
)

for location in "${CHROME_LOCATIONS[@]}"; do
    if [ -f "$location" ]; then
        remove_path "$location" "Chrome/Chromium native messaging host"
    fi
done

# Firefox native messaging
FIREFOX_LOCATIONS=(
    "$HOME/.mozilla/native-messaging-hosts/ldm.json"
    "/usr/lib/mozilla/native-messaging-hosts/ldm.json"
    "/usr/lib64/mozilla/native-messaging-hosts/ldm.json"
)

for location in "${FIREFOX_LOCATIONS[@]}"; do
    if [ -f "$location" ]; then
        remove_path "$location" "Firefox native messaging host"
    fi
done

# 5. Remove systemd services
echo -e "${WHITE}=== Removing System Services ===${NC}"

if systemctl --user list-unit-files | grep -q ldm; then
    echo -e "${CYAN}Stopping and removing user services...${NC}"
    systemctl --user stop ldm* 2>/dev/null || true
    systemctl --user disable ldm* 2>/dev/null || true
    rm -f "$HOME/.config/systemd/user/ldm*"
    systemctl --user daemon-reload
    echo -e "${GREEN}✓ User services removed${NC}"
fi

if [ -f "/etc/systemd/system/ldm.service" ]; then
    echo -e "${YELLOW}System service detected. Root privileges required.${NC}"
    if command -v sudo >/dev/null 2>&1; then
        ask_confirmation "Remove system service?"
        sudo systemctl stop ldm 2>/dev/null || true
        sudo systemctl disable ldm 2>/dev/null || true
        sudo rm -f /etc/systemd/system/ldm*
        sudo systemctl daemon-reload
        echo -e "${GREEN}✓ System service removed${NC}"
    fi
fi

echo

# 6. Clean package manager traces (if installed via package)
echo -e "${WHITE}=== Checking Package Manager ===${NC}"

# Check for RPM package
if command -v rpm >/dev/null 2>&1 && rpm -q ldm >/dev/null 2>&1; then
    echo -e "${YELLOW}LDM RPM package detected.${NC}"
    ask_confirmation "Remove RPM package?"
    sudo rpm -e ldm
    echo -e "${GREEN}✓ RPM package removed${NC}"
# Check for DEB package
elif command -v dpkg >/dev/null 2>&1 && dpkg -l | grep -q ldm; then
    echo -e "${YELLOW}LDM DEB package detected.${NC}"
    ask_confirmation "Remove DEB package?"
    sudo apt remove --purge ldm
    echo -e "${GREEN}✓ DEB package removed${NC}"
else
    echo -e "${GREEN}✓ No package manager installation found${NC}"
fi

echo

# 7. Remove temporary files and logs
echo -e "${WHITE}=== Cleaning Temporary Files ===${NC}"

remove_path "/tmp/ldm*" "temporary files"
remove_path "/var/log/ldm*" "system log files"
remove_path "$HOME/.local/share/recently-used.xbel.ldm*" "recent files traces"

echo

# 8. Update desktop database
echo -e "${WHITE}=== Updating Desktop Database ===${NC}"

if command -v update-desktop-database >/dev/null 2>&1; then
    echo -e "${CYAN}Updating desktop database...${NC}"
    update-desktop-database "$HOME/.local/share/applications/" 2>/dev/null || true
    echo -e "${GREEN}✓ Desktop database updated${NC}"
fi

if command -v gtk-update-icon-cache >/dev/null 2>&1; then
    echo -e "${CYAN}Updating icon cache...${NC}"
    gtk-update-icon-cache "$HOME/.local/share/icons/" 2>/dev/null || true
    echo -e "${GREEN}✓ Icon cache updated${NC}"
fi

echo

# 9. Summary
echo "=============================================="
echo -e "${GREEN}LDM Uninstallation Complete${NC}"
echo "=============================================="
echo

echo -e "${WHITE}Removed components:${NC}"
echo "✓ Local executables and build files"
echo "✓ User configuration and data files"
echo "✓ Desktop entries and shortcuts"
echo "✓ Browser integration files"
echo "✓ Cache and temporary files"
echo "✓ System services (if present)"
echo "✓ Package manager installation (if present)"

echo
echo -e "${WHITE}What was NOT removed:${NC}"
echo "• Downloaded files in ~/Downloads/"
echo "• User-created download directories"
echo "• System packages (Qt6, build tools, etc.)"
echo "• Browser extensions (remove manually)"

echo
echo -e "${CYAN}Manual cleanup (if needed):${NC}"
echo "• Remove browser extensions manually"
echo "• Clear browser download history"
echo "• Remove any custom download directories"

echo
echo -e "${YELLOW}To completely remove all traces:${NC}"
echo "• Clear browser cache and cookies"
echo "• Check for any remaining files: find /home /opt /usr -name '*ldm*' 2>/dev/null"

echo
echo -e "${BLUE}Thank you for using LDM - Linux Download Manager!${NC}"
echo -e "${BLUE}Developer: Anna-el Gerard RABENANDRASANA (aerab243)${NC}"
echo -e "${BLUE}Project: https://github.com/aerab243/ldm${NC}"
echo

echo -e "${GREEN}Uninstallation completed successfully.${NC}"
