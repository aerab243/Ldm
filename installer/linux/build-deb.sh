#!/bin/bash

# LDM Download Manager - Debian Package Builder
# This script creates a DEB package for Ubuntu/Debian

set -e

# Configuration
PACKAGE_NAME="ldm"
VERSION="1.0.0"
ARCHITECTURE="amd64"
MAINTAINER="aerab243 (Anna-el Gerard RABENANDRASANA) <aerabenandrasana@gmail.com>"
DESCRIPTION="A powerful download manager with browser integration"
SOURCE_DIR="../../desktop/build/release"
OUTPUT_DIR="./output"
DEB_DIR="${OUTPUT_DIR}/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

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
    rm -rf "$DEB_DIR"
}

trap cleanup EXIT

# Check prerequisites
check_prerequisites() {
    log_info "Checking prerequisites..."

    if [ ! -d "$SOURCE_DIR" ]; then
        log_error "Source directory not found: $SOURCE_DIR"
        log_error "Please build the application first"
        exit 1
    fi

    if ! command -v dpkg-deb &> /dev/null; then
        log_error "dpkg-deb not found. Please install dpkg-dev package."
        exit 1
    fi

    if ! command -v fakeroot &> /dev/null; then
        log_error "fakeroot not found. Please install fakeroot package."
        exit 1
    fi
}

# Create DEB directory structure
create_deb_structure() {
    log_info "Creating DEB directory structure..."

    mkdir -p "$DEB_DIR/DEBIAN"
    mkdir -p "$DEB_DIR/usr/bin"
    mkdir -p "$DEB_DIR/usr/share/applications"
    mkdir -p "$DEB_DIR/usr/share/icons/hicolor/256x256/apps"
    mkdir -p "$DEB_DIR/usr/share/ldm"
    mkdir -p "$DEB_DIR/usr/share/doc/ldm"
    mkdir -p "$DEB_DIR/etc/ldm"
}

# Create control file
create_control_file() {
    log_info "Creating control file..."

    cat > "$DEB_DIR/DEBIAN/control" << EOF
Package: $PACKAGE_NAME
Version: $VERSION
Architecture: $ARCHITECTURE
Maintainer: $MAINTAINER
Description: $DESCRIPTION
Depends: libqt6network6 (>= 6.2.0), libqt6widgets6 (>= 6.2.0), libqt6core6 (>= 6.2.0), libssl3 (>= 3.0.0), libc6 (>= 2.34)
Homepage: https://github.com/aerab243/ldm
Section: utils
Priority: optional
Installed-Size: $(du -sk "$SOURCE_DIR" | cut -f1)
EOF
}

# Create postinst script
create_postinst() {
    log_info "Creating postinst script..."

    cat > "$DEB_DIR/DEBIAN/postinst" << 'EOF'
#!/bin/bash

set -e

# Create desktop entry
cat > /usr/share/applications/ldm.desktop << EOL
[Desktop Entry]
Name=LDM Download Manager
Comment=A powerful download manager
Exec=/usr/bin/ldm
Icon=ldm
Terminal=false
Type=Application
Categories=Network;FileTransfer;
EOL

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    update-desktop-database
fi

# Create default config directory
mkdir -p /etc/ldm

echo "LDM installation completed successfully!"
EOF

    chmod 755 "$DEB_DIR/DEBIAN/postinst"
}

# Create prerm script
create_prerm() {
    log_info "Creating prerm script..."

    cat > "$DEB_DIR/DEBIAN/prerm" << 'EOF'
#!/bin/bash

set -e

# Remove desktop entry
rm -f /usr/share/applications/ldm.desktop

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    update-desktop-database
fi

echo "LDM pre-removal completed!"
EOF

    chmod 755 "$DEB_DIR/DEBIAN/prerm"
}

# Copy files
copy_files() {
    log_info "Copying application files..."

    # Copy main executable
    cp "$SOURCE_DIR/ldm" "$DEB_DIR/usr/bin/"

    # Copy libraries and resources
    cp -r "$SOURCE_DIR/lib" "$DEB_DIR/usr/share/ldm/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/plugins" "$DEB_DIR/usr/share/ldm/" 2>/dev/null || true
    cp -r "$SOURCE_DIR/resources" "$DEB_DIR/usr/share/ldm/" 2>/dev/null || true

    # Copy icon
    cp "../../desktop/resources/icons/app-icon.png" "$DEB_DIR/usr/share/icons/hicolor/256x256/apps/ldm.png" 2>/dev/null || true

    # Copy documentation
    if [ -d "../../docs" ]; then
        cp -r "../../docs"/* "$DEB_DIR/usr/share/doc/ldm/" 2>/dev/null || true
    fi

    # Copy copyright and changelog
    cat > "$DEB_DIR/usr/share/doc/ldm/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: LDM Download Manager
Upstream-Contact: $MAINTAINER
Source: https://github.com/aerab243/ldm

Files: *
Copyright: 2025 aerab243 (Anna-el Gerard RABENANDRASANA)
License: GPL-3+
EOF

    # Set permissions
    find "$DEB_DIR" -type f -exec chmod 644 {} \;
    find "$DEB_DIR" -type d -exec chmod 755 {} \;
    chmod 755 "$DEB_DIR/usr/bin/ldm"
}

# Build DEB package
build_deb() {
    log_info "Building DEB package..."

    mkdir -p "$OUTPUT_DIR"

    fakeroot dpkg-deb --build "$DEB_DIR" "$OUTPUT_DIR/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"
}

# Verify package
verify_package() {
    log_info "Verifying package..."

    dpkg-deb --info "$OUTPUT_DIR/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"

    log_info "Package contents:"
    dpkg-deb --contents "$OUTPUT_DIR/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb" | head -20
}

# Main execution
main() {
    log_info "Starting DEB build for $PACKAGE_NAME $VERSION"

    check_prerequisites
    create_deb_structure
    create_control_file
    create_postinst
    create_prerm
    copy_files
    build_deb
    verify_package

    log_info "DEB build completed successfully!"
    log_info "Output: $OUTPUT_DIR/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb"

    # Display package size
    PACKAGE_SIZE=$(du -h "$OUTPUT_DIR/${PACKAGE_NAME}_${VERSION}_${ARCHITECTURE}.deb" | cut -f1)
    log_info "Package size: $PACKAGE_SIZE"
}

# Run main function
main "$@"
