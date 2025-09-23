#!/bin/bash

# LDM Installation Script for Linux
# This script detects the Linux distribution and installs dependencies,
# then builds and installs LDM (Linux Download Manager)

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

# Function to detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        DISTRO=$ID
        VERSION=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        DISTRO="rhel"
        VERSION=$(cat /etc/redhat-release | sed 's/.*release \([0-9]\+\).*/\1/')
    elif [ -f /etc/debian_version ]; then
        DISTRO="debian"
        VERSION=$(cat /etc/debian_version)
    else
        print_error "Unable to detect Linux distribution"
        exit 1
    fi

    print_status "Detected distribution: $DISTRO $VERSION"
}

# Function to check if dependencies are installed
check_dependencies() {
    print_status "Checking if dependencies are already installed..."

    local missing_deps=false

    # Check for basic tools
    if ! command -v cmake &> /dev/null; then
        print_warning "CMake not found"
        missing_deps=true
    fi

    if ! command -v g++ &> /dev/null && ! command -v gcc &> /dev/null; then
        print_warning "C++ compiler not found"
        missing_deps=true
    fi

    # Check for Qt6
    if ! pkg-config --exists Qt6Core Qt6Widgets Qt6Network Qt6Sql; then
        print_warning "Qt6 development libraries not found"
        missing_deps=true
    fi

    # Check for other libraries
    if ! pkg-config --exists libcurl; then
        print_warning "libcurl development library not found"
        missing_deps=true
    fi

    if [ "$missing_deps" = true ]; then
        print_status "Some dependencies are missing. Installing them..."
        install_dependencies
    else
        print_success "All dependencies are already installed"
    fi
}

# Function to install dependencies
install_dependencies() {
    print_status "Installing dependencies for $DISTRO..."

    case $DISTRO in
        ubuntu|debian|linuxmint|pop|elementary)
            sudo apt update
            sudo apt install -y build-essential cmake qt6-base-dev qt6-tools-dev \
                libqt6network6 libqt6sql6 libqt6multimedia6 \
                libcurl4-openssl-dev libssl-dev libsqlite3-dev libprotobuf-dev \
                pkg-config
            ;;
        fedora|rhel|centos|rocky|almalinux)
            if command -v dnf &> /dev/null; then
                sudo dnf install -y gcc-c++ make cmake qt6-qtbase-devel qt6-qttools-devel \
                    qt6-qtnetwork qt6-qtsql qt6-qtmultimedia \
                    libcurl-devel openssl-devel sqlite-devel protobuf-devel \
                    pkgconfig
            elif command -v yum &> /dev/null; then
                sudo yum install -y gcc-c++ make cmake qt6-qtbase-devel qt6-qttools-devel \
                    qt6-qtnetwork qt6-qtsql qt6-qtmultimedia \
                    libcurl-devel openssl-devel sqlite-devel protobuf-devel \
                    pkgconfig
            else
                print_error "Neither dnf nor yum found"
                exit 1
            fi
            ;;
        arch|manjaro|endeavouros)
            sudo pacman -Sy --noconfirm base-devel cmake qt6-base qt6-tools \
                qt6-network qt6-sql qt6-multimedia \
                curl openssl sqlite protobuf pkgconf
            ;;
        opensuse|sles)
            sudo zypper install -y gcc-c++ make cmake libqt6-qtbase-devel libqt6-qttools-devel \
                libqt6-qtnetwork6 libqt6-qtsql6 libqt6-qtmultimedia6 \
                libcurl-devel libopenssl-devel sqlite3-devel protobuf-devel \
                pkg-config
            ;;
        gentoo)
            sudo emerge --ask=n dev-qt/qtbase:6 dev-qt/qttools:6 dev-qt/qtnetwork:6 \
                dev-qt/qtsql:6 dev-qt/qtmultimedia:6 \
                net-misc/curl dev-libs/openssl dev-db/sqlite dev-libs/protobuf \
                dev-util/cmake dev-util/pkgconfig
            ;;
        *)
            print_error "Unsupported distribution: $DISTRO"
            print_warning "Please install dependencies manually:"
            echo "  - C++ compiler (gcc/g++)"
            echo "  - CMake"
            echo "  - Qt6 (base, tools, network, sql, multimedia)"
            echo "  - libcurl, openssl, sqlite, protobuf development packages"
            exit 1
            ;;
    esac

    print_success "Dependencies installed"
}

# Function to build LDM
build_ldm() {
    print_status "Building LDM..."

    # Create build directory
    mkdir -p build
    cd build

    # Configure with CMake
    cmake -DCMAKE_BUILD_TYPE=Release ..

    # Build
    make -j$(nproc)

    print_success "LDM built successfully"
}

# Function to install LDM
install_ldm() {
    local project_dir="$1"
    print_status "Installing LDM system-wide..."

    cd "$project_dir/build"
    sudo make install

    print_success "LDM installed successfully"
    print_status "You can now run 'ldm-complete' from the command line"
}

# Main installation process
main() {
    print_status "Starting LDM installation..."

    # Check if we're in the correct directory
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "Please run this script from the LDM root directory"
        exit 1
    fi

    # Store project directory
    PROJECT_DIR=$(pwd)

    # Detect distribution
    detect_distro

    # Check and install dependencies if needed
    check_dependencies

    # Build LDM
    build_ldm

    # Install LDM
    install_ldm "$PROJECT_DIR"

    print_success "LDM installation completed!"
    echo ""
    echo "To run LDM:"
    echo "  ldm-complete"
    echo ""
    echo "For more information, see the README.md file"
}

# Run main function
main "$@"
