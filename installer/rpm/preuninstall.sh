#!/bin/bash
# Pre-uninstall script for Linux Download Manager (LDM) - RPM package
# This script is executed before the package is removed
# Developed by: Anna-el Gerard RABENANDRASANA (aerab243)
# Project: https://github.com/aerab243/ldm

set -e

# Package name
PACKAGE_NAME="ldm"

echo "Preparing to remove Linux Download Manager..."

# Stop any running LDM processes
if command -v killall >/dev/null 2>&1; then
    killall ldm 2>/dev/null || true
    killall ldm-native-host 2>/dev/null || true
fi

# Give processes time to shutdown gracefully
sleep 2

# Force kill if still running
if command -v pkill >/dev/null 2>&1; then
    pkill -f "ldm" 2>/dev/null || true
    pkill -f "ldm-native-host" 2>/dev/null || true
fi

# Stop systemd user service if running
if command -v systemctl >/dev/null 2>&1; then
    # Try to stop user service for all users
    for user_home in /home/*; do
        if [ -d "$user_home" ]; then
            username=$(basename "$user_home")
            if id "$username" >/dev/null 2>&1; then
                # Stop service for this user
                sudo -u "$username" systemctl --user stop ldm.service 2>/dev/null || true
                sudo -u "$username" systemctl --user disable ldm.service 2>/dev/null || true
            fi
        fi
    done

    # Remove systemd service file
    if [ -f "/usr/lib/systemd/user/ldm.service" ]; then
        rm -f /usr/lib/systemd/user/ldm.service || true
        systemctl daemon-reload || true
    fi
fi

# Remove native messaging hosts
if [ -f "/etc/opt/chrome/native-messaging-hosts/ldm-chrome.json" ]; then
    rm -f /etc/opt/chrome/native-messaging-hosts/ldm-chrome.json || true
fi

if [ -f "/usr/lib64/mozilla/native-messaging-hosts/ldm-firefox.json" ]; then
    rm -f /usr/lib64/mozilla/native-messaging-hosts/ldm-firefox.json || true
fi

if [ -f "/usr/lib/mozilla/native-messaging-hosts/ldm-firefox.json" ]; then
    rm -f /usr/lib/mozilla/native-messaging-hosts/ldm-firefox.json || true
fi

# Unregister URL handlers
if command -v xdg-mime >/dev/null 2>&1; then
    # Note: We can't easily restore the previous default handler
    # so we just remove our association
    echo "Note: URL handler associations will be reset"
fi

# Remove symlink
if [ -L "/usr/local/bin/ldm" ]; then
    rm -f /usr/local/bin/ldm || true
fi

# Remove autostart entries for all users
for user_home in /home/*; do
    if [ -d "$user_home" ]; then
        autostart_file="$user_home/.config/autostart/ldm.desktop"
        if [ -f "$autostart_file" ]; then
            rm -f "$autostart_file" || true
        fi
    fi
done

# Clean up temporary files
if [ -d "/tmp/ldm" ]; then
    rm -rf /tmp/ldm || true
fi

# Clean up any lock files
if [ -f "/var/lock/ldm.lock" ]; then
    rm -f /var/lock/ldm.lock || true
fi

# Note: We don't remove user configuration files and data
# as users might want to keep their settings and download history
echo "Note: User configuration and data files will be preserved"
echo "To completely remove all LDM data, users should manually delete:"
echo "  - ~/.config/ldm/"
echo "  - ~/.local/share/ldm/"

exit 0
