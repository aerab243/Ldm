#!/bin/bash

# LDM Download Manager - macOS DMG Builder
# This script creates a DMG installer for macOS

set -e

# Configuration
APP_NAME="LDM Download Manager"
APP_VERSION="1.0.0"
DMG_NAME="${APP_NAME}-${APP_VERSION}"
SOURCE_DIR="../../desktop/build/release/LDM.app"
OUTPUT_DIR="./output"
DMG_PATH="${OUTPUT_DIR}/${DMG_NAME}.dmg"
TEMP_DMG="${OUTPUT_DIR}/temp.dmg"
MOUNT_POINT="/Volumes/${APP_NAME}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

cleanup() {
    log_info "Cleaning up..."
    if [ -d "$MOUNT_POINT" ]; then
        hdiutil detach "$MOUNT_POINT" 2>/dev/null || true
    fi
    rm -f "$TEMP_DMG"
}

trap cleanup EXIT

# Check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."

    if [ ! -d "$SOURCE_DIR" ]; then
        log_error "Source app not found at: $SOURCE_DIR"
        log_error "Please build the application first"
        exit 1
    fi

    if ! command -v hdiutil &> /dev/null; then
        log_error "hdiutil not found. This script requires macOS."
        exit 1
    fi

    if ! command -v create-dmg &> /dev/null; then
        log_warn "create-dmg not found. Installing..."
        if command -v brew &> /dev/null; then
            brew install create-dmg
        else
            log_error "Homebrew not found. Please install create-dmg manually."
            exit 1
        fi
    fi
}

# Create output directory
create_output_dir() {
    log_info "Creating output directory..."
    mkdir -p "$OUTPUT_DIR"
}

# Create temporary DMG
create_temp_dmg() {
    log_info "Creating temporary DMG..."

    # Calculate size (app size + 100MB for overhead)
    APP_SIZE=$(du -sm "$SOURCE_DIR" | cut -f1)
    DMG_SIZE=$((APP_SIZE + 100))

    hdiutil create -size ${DMG_SIZE}m -fs HFS+ -volname "$APP_NAME" "$TEMP_DMG"
}

# Mount temporary DMG
mount_temp_dmg() {
    log_info "Mounting temporary DMG..."
    hdiutil attach "$TEMP_DMG" -mountpoint "$MOUNT_POINT"
}

# Copy application and create symlinks
populate_dmg() {
    log_info "Populating DMG..."

    # Copy application
    cp -R "$SOURCE_DIR" "$MOUNT_POINT/"

    # Create Applications symlink
    ln -s /Applications "$MOUNT_POINT/Applications"

    # Copy documentation
    if [ -d "../../docs" ]; then
        cp -R "../../docs" "$MOUNT_POINT/Documentation"
    fi

    # Set permissions
    chmod -R 755 "$MOUNT_POINT/LDM.app"
}

# Create final DMG with create-dmg
create_final_dmg() {
    log_info "Creating final DMG..."

    # Unmount temp DMG
    hdiutil detach "$MOUNT_POINT"

    # Use create-dmg for professional appearance
    create-dmg \
        --volname "$APP_NAME" \
        --volicon "../../desktop/resources/icons/app-icon.icns" \
        --window-pos 200 120 \
        --window-size 800 400 \
        --icon-size 100 \
        --icon "LDM.app" 200 190 \
        --hide-extension "LDM.app" \
        --app-drop-link 600 185 \
        --no-internet-enable \
        "$DMG_PATH" \
        "$SOURCE_DIR"
}

# Sign the DMG (optional)
sign_dmg() {
    if [ -n "$CODE_SIGN_IDENTITY" ]; then
        log_info "Signing DMG..."
        codesign --sign "$CODE_SIGN_IDENTITY" --verbose "$DMG_PATH"
    else
        log_warn "Code signing identity not set. Skipping DMG signing."
    fi
}

# Verify DMG
verify_dmg() {
    log_info "Verifying DMG..."

    # Mount DMG
    hdiutil attach "$DMG_PATH" -mountpoint "$MOUNT_POINT" -readonly

    # Check if app exists
    if [ ! -d "$MOUNT_POINT/LDM.app" ]; then
        log_error "LDM.app not found in DMG"
        exit 1
    fi

    # Check Applications symlink
    if [ ! -L "$MOUNT_POINT/Applications" ]; then
        log_error "Applications symlink not found in DMG"
        exit 1
    fi

    # Unmount
    hdiutil detach "$MOUNT_POINT"

    log_info "DMG verification successful"
}

# Main execution
main() {
    log_info "Starting DMG build for $APP_NAME $APP_VERSION"

    check_prerequisites
    create_output_dir
    create_temp_dmg
    mount_temp_dmg
    populate_dmg
    create_final_dmg
    sign_dmg
    verify_dmg

    log_info "DMG build completed successfully!"
    log_info "Output: $DMG_PATH"

    # Display file size
    DMG_SIZE=$(du -h "$DMG_PATH" | cut -f1)
    log_info "DMG size: $DMG_SIZE"
}

# Run main function
main "$@"
