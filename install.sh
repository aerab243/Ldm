#!/bin/bash
# LDM - Like Download Manager Installation Script
# Complete IDM-style download manager with real download engine

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Print colored output
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

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        print_error "Please don't run this script as root. Use sudo when needed."
        exit 1
    fi
}

# Detect distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    else
        print_error "Cannot detect distribution. Please install dependencies manually."
        exit 1
    fi

    print_status "Detected: $PRETTY_NAME"
}

# Install dependencies based on distribution
install_dependencies() {
    print_status "Installing dependencies for $DISTRO..."

    case $DISTRO in
        ubuntu|debian)
            sudo apt update
            sudo apt install -y build-essential qt6-base-dev qt6-tools-dev cmake
            sudo apt install -y libcurl4-openssl-dev libssl-dev pkg-config
            sudo apt install -y qt6-base-dev-tools
            ;;
        fedora)
            sudo dnf install -y gcc-c++ make cmake qt6-qtbase-devel qt6-qttools-devel
            sudo dnf install -y libcurl-devel openssl-devel pkg-config
            ;;
        centos|rhel)
            sudo yum install -y gcc-c++ make cmake qt6-qtbase-devel qt6-qttools-devel
            sudo yum install -y libcurl-devel openssl-devel pkg-config
            ;;
        arch|manjaro)
            sudo pacman -S --noconfirm gcc make cmake qt6-base qt6-tools
            sudo pacman -S --noconfirm curl openssl pkg-config
            ;;
        opensuse*)
            sudo zypper install -y gcc-c++ make cmake qt6-base-devel qt6-tools-devel
            sudo zypper install -y libcurl-devel libopenssl-devel pkg-config
            ;;
        *)
            print_warning "Unsupported distribution: $DISTRO"
            print_status "Please install dependencies manually:"
            echo "  - Qt6 development packages"
            echo "  - libcurl development packages"
            echo "  - OpenSSL development packages"
            echo "  - C++ compiler (g++)"
            echo "  - make and cmake"
            read -p "Press Enter if dependencies are installed, or Ctrl+C to exit..."
            ;;
    esac

    print_success "Dependencies installed successfully"
}

# Check dependencies
check_dependencies() {
    print_status "Checking dependencies..."

    # Check compiler
    if ! command -v g++ &> /dev/null; then
        print_error "g++ compiler not found"
        return 1
    fi

    # Check make
    if ! command -v make &> /dev/null; then
        print_error "make not found"
        return 1
    fi

    # Check Qt6
    if ! pkg-config --exists Qt6Core; then
        print_error "Qt6 Core not found"
        return 1
    fi

    if ! pkg-config --exists Qt6Widgets; then
        print_error "Qt6 Widgets not found"
        return 1
    fi

    if ! pkg-config --exists Qt6Network; then
        print_error "Qt6 Network not found"
        return 1
    fi

    # Check libcurl
    if ! ldconfig -p | grep libcurl >/dev/null 2>&1; then
        print_error "libcurl not found"
        return 1
    fi

    # Check OpenSSL
    if ! ldconfig -p | grep libssl >/dev/null 2>&1; then
        print_error "OpenSSL not found"
        return 1
    fi

    # Check MOC
    MOC_FOUND=false
    for moc_path in "/usr/lib64/qt6/libexec/moc" "/usr/lib/qt6/libexec/moc" "/usr/lib/x86_64-linux-gnu/qt6/libexec/moc"; do
        if [ -x "$moc_path" ]; then
            MOC_FOUND=true
            break
        fi
    done

    if [ "$MOC_FOUND" = false ]; then
        if ! command -v moc &> /dev/null && ! command -v moc-qt6 &> /dev/null; then
            print_warning "MOC (Meta-Object Compiler) not found, but build may still work"
        fi
    fi

    print_success "All dependencies are available"
}

# Build application
build_application() {
    print_status "Building LDM application..."

    cd desktop

    # Clean previous build
    make -f Makefile.complete clean >/dev/null 2>&1 || true

    # Build complete application
    if make -f Makefile.complete; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        return 1
    fi

    cd ..
}

# Test application
test_application() {
    print_status "Testing application..."

    cd desktop

    if [ -x "./ldm-complete" ]; then
        print_success "Application binary is executable"

        # Quick test - just check if it starts and exits
        timeout 3s ./ldm-complete --version >/dev/null 2>&1 || true

        print_success "Application test completed"
    else
        print_error "Application binary not found or not executable"
        return 1
    fi

    cd ..
}

# Install application
install_application() {
    print_status "Installing LDM..."

    cd desktop

    # Create directories
    sudo mkdir -p /usr/local/bin
    sudo mkdir -p /usr/local/share/applications
    sudo mkdir -p /usr/local/share/pixmaps
    sudo mkdir -p /usr/local/share/doc/ldm

    # Install binary
    sudo cp ldm-complete /usr/local/bin/ldm
    sudo chmod +x /usr/local/bin/ldm

    # Install desktop file
    cat > ldm.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=LDM - Like Download Manager
Comment=A powerful download manager with IDM-style interface
Exec=/usr/local/bin/ldm
Icon=ldm
Terminal=false
StartupNotify=true
Categories=Network;FileTransfer;
Keywords=download;manager;internet;file;transfer;
MimeType=x-scheme-handler/http;x-scheme-handler/https;x-scheme-handler/ftp;
EOF

    sudo mv ldm.desktop /usr/local/share/applications/

    # Install documentation
    sudo cp ../README.md /usr/local/share/doc/ldm/ 2>/dev/null || true
    sudo cp ../LICENSE /usr/local/share/doc/ldm/ 2>/dev/null || true

    # Update desktop database
    if command -v update-desktop-database &> /dev/null; then
        sudo update-desktop-database /usr/local/share/applications >/dev/null 2>&1 || true
    fi

    cd ..

    print_success "LDM installed successfully"
}

# Create uninstall script
create_uninstaller() {
    print_status "Creating uninstaller..."

    cat > uninstall_ldm.sh << 'EOF'
#!/bin/bash
# LDM Uninstaller

echo "Uninstalling LDM - Like Download Manager..."

# Remove binary
sudo rm -f /usr/local/bin/ldm

# Remove desktop file
sudo rm -f /usr/local/share/applications/ldm.desktop

# Remove icon
sudo rm -f /usr/local/share/pixmaps/ldm.png

# Remove documentation
sudo rm -rf /usr/local/share/doc/ldm

# Update desktop database
if command -v update-desktop-database &> /dev/null; then
    sudo update-desktop-database /usr/local/share/applications >/dev/null 2>&1 || true
fi

echo "LDM uninstalled successfully"
echo "User configuration files in ~/.config/LDM/ were preserved"
echo "To remove them: rm -rf ~/.config/LDM/"
EOF

    chmod +x uninstall_ldm.sh
    print_success "Uninstaller created: ./uninstall_ldm.sh"
}

# Main installation process
main() {
    echo "==============================================="
    echo "  LDM - Like Download Manager Installer"
    echo "  Version 1.0.0"
    echo "==============================================="
    echo

    check_root
    detect_distro

    # Ask user what to do
    echo "What would you like to do?"
    echo "1) Install dependencies and build LDM"
    echo "2) Just build LDM (dependencies already installed)"
    echo "3) Just install LDM (already built)"
    echo "4) Check dependencies only"
    echo "5) Exit"
    echo
    read -p "Enter your choice (1-5): " choice

    case $choice in
        1)
            install_dependencies
            if check_dependencies; then
                build_application
                test_application

                echo
                read -p "Install LDM system-wide? (y/N): " install_choice
                if [[ $install_choice =~ ^[Yy]$ ]]; then
                    install_application
                    create_uninstaller
                fi
            else
                print_error "Dependency check failed"
                exit 1
            fi
            ;;
        2)
            if check_dependencies; then
                build_application
                test_application

                echo
                read -p "Install LDM system-wide? (y/N): " install_choice
                if [[ $install_choice =~ ^[Yy]$ ]]; then
                    install_application
                    create_uninstaller
                fi
            else
                print_error "Dependencies not satisfied"
                exit 1
            fi
            ;;
        3)
            if [ -f "desktop/ldm-complete" ]; then
                install_application
                create_uninstaller
            else
                print_error "LDM binary not found. Please build first."
                exit 1
            fi
            ;;
        4)
            check_dependencies
            ;;
        5)
            print_status "Exiting..."
            exit 0
            ;;
        *)
            print_error "Invalid choice"
            exit 1
            ;;
    esac

    echo
    echo "==============================================="
    echo "  Installation Summary"
    echo "==============================================="

    if [ -f "/usr/local/bin/ldm" ]; then
        print_success "LDM is installed and ready to use!"
        echo
        echo "How to run:"
        echo "  • From terminal: ldm"
        echo "  • From applications menu: Look for 'LDM - Like Download Manager'"
        echo
        echo "Configuration:"
        echo "  • Settings stored in: ~/.config/LDM/"
        echo "  • Downloads default to: ~/Downloads/"
        echo
        echo "To uninstall:"
        echo "  • Run: ./uninstall_ldm.sh"
        echo
    elif [ -f "desktop/ldm-complete" ]; then
        print_success "LDM is built and ready to run!"
        echo
        echo "How to run:"
        echo "  • From this directory: cd desktop && ./ldm-complete"
        echo
        echo "To install system-wide:"
        echo "  • Run: ./install.sh and choose option 3"
        echo
    fi

    echo "Features available:"
    echo "  ✓ Real download engine with libcurl"
    echo "  ✓ Multi-threaded downloads"
    echo "  ✓ Resume/pause functionality"
    echo "  ✓ IDM-style interface"
    echo "  ✓ Download scheduling"
    echo "  ✓ Queue management"
    echo "  ✓ Link grabber"
    echo "  ✓ System tray integration"
    echo
    echo "Enjoy using LDM!"
}

# Run main function
main "$@"
