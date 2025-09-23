#!/bin/bash

# LDM Simple Startup Script
# Easy launcher for Linux Download Manager

echo "🚀 LDM - Linux Download Manager"
echo "================================"

cd "$(dirname "$0")"

# Check for working GUI version first
if [ -f "desktop/ldm-working" ] && [ -x "desktop/ldm-working" ]; then
    echo "Starting LDM GUI..."
    echo "📱 Interface: Full download manager with modern Qt6 GUI"
    echo "✨ Features: Categories, progress tracking, download management"
    echo
    exec ./desktop/ldm-working "$@"

# Fallback to CLI version
elif [ -f "desktop/ldm-cli" ] && [ -x "desktop/ldm-cli" ]; then
    echo "Starting LDM CLI..."
    echo "💻 Interface: Command line interface"
    echo "⚡ Features: URL management, status reporting, system info"
    echo

    # If no arguments, show help
    if [ $# -eq 0 ]; then
        ./desktop/ldm-cli --help
    else
        exec ./desktop/ldm-cli "$@"
    fi

# Need to build first
else
    echo "❌ LDM not found. Building now..."
    echo

    # Try to build working version
    if [ -f "build_working.sh" ] && [ -x "build_working.sh" ]; then
        echo "Building working GUI version..."
        ./build_working.sh

        if [ -f "desktop/ldm-working" ]; then
            echo "✅ Build successful! Starting LDM..."
            exec ./desktop/ldm-working "$@"
        fi
    fi

    # Try to build CLI version
    if [ -f "build_cli.sh" ] && [ -x "build_cli.sh" ]; then
        echo "Building CLI version..."
        ./build_cli.sh

        if [ -f "desktop/ldm-cli" ]; then
            echo "✅ Build successful! Starting LDM CLI..."
            if [ $# -eq 0 ]; then
                ./desktop/ldm-cli --help
            else
                exec ./desktop/ldm-cli "$@"
            fi
        fi
    fi

    # If nothing works
    echo "❌ Unable to build or find LDM executables."
    echo
    echo "Please install dependencies first:"
    echo "  Fedora: sudo dnf install qt6-qtbase-devel qt6-qttools-devel"
    echo "  Ubuntu: sudo apt install qt6-base-dev qt6-tools-dev"
    echo
    echo "Then run: ./build_working.sh"
    exit 1
fi
