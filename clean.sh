#!/bin/bash

# LDM Cleanup Script
# This script removes all build artifacts and generated files

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

# Function to remove build directories
remove_build_dirs() {
    print_status "Removing build directories..."

    local dirs_removed=0

    # Find and remove all directories named "build"
    while IFS= read -r -d '' dir; do
        rm -rf "$dir"
        ((dirs_removed++))
    done < <(find . -type d -name "build" -print0)

    # Also remove .qt directories (Qt build artifacts)
    while IFS= read -r -d '' dir; do
        rm -rf "$dir"
        ((dirs_removed++))
    done < <(find . -type d -name ".qt" -print0)

    if [ $dirs_removed -gt 0 ]; then
        print_success "Removed $dirs_removed build directories"
    else
        print_status "No build directories found"
    fi
}

# Function to remove CMake temporary files
remove_cmake_files() {
    print_status "Removing CMake temporary files..."

    local files_removed=0

    # Find and remove CMake cache and temporary files
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "CMakeCache.txt" -o -name "cmake_install.cmake" -o -name "Makefile" -o -name "install_manifest.txt" -o -name "CPack*.cmake" -o -name "CTestTestfile.cmake" -o -name "compile_commands.json" -o -name "_deps" -type f -print0)

    # Also remove CMake build directories
    while IFS= read -r -d '' dir; do
        rm -rf "$dir"
        ((files_removed++))
    done < <(find . -type d -name "CMakeFiles" -print0)

    if [ $files_removed -gt 0 ]; then
        print_success "Removed $files_removed CMake temporary files and directories"
    else
        print_status "No CMake temporary files found"
    fi
}

# Function to remove Qt MOC and autogen files
remove_qt_generated_files() {
    print_status "Removing Qt generated files (MOC, autogen)..."

    local files_removed=0

    # Find and remove Qt generated files and directories
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*.moc" -o -name "moc_*.cpp" -o -name "qrc_*.cpp" -o -name "ui_*.h" -type f -print0)

    # Remove autogen directories
    while IFS= read -r -d '' dir; do
        rm -rf "$dir"
        ((files_removed++))
    done < <(find . -type d -name "*_autogen*" -print0)

    if [ $files_removed -gt 0 ]; then
        print_success "Removed $files_removed Qt generated files and directories"
    else
        print_status "No Qt generated files found"
    fi
}

# Function to remove other build artifacts
remove_other_artifacts() {
    print_status "Removing other build artifacts..."

    local files_removed=0

    # Remove common build artifacts
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*.o" -o -name "*.so" -o -name "*.a" -o -name "*.exe" -o -name "*.dll" -o -name "*.dylib" -o -name "*.obj" -o -name "*.lib" -o -name "*.pdb" -o -name "*.ilk" -o -name "*.exp" -o -name "*.manifest" -type f -print0)

    # Remove backup files and temporary files
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*~" -o -name "*.bak" -o -name "*.tmp" -o -name "*.swp" -o -name ".DS_Store" -type f -print0)

    if [ $files_removed -gt 0 ]; then
        print_success "Removed $files_removed other build artifacts and temporary files"
    else
        print_status "No other build artifacts found"
    fi
}

# Function to clean test artifacts
clean_test_artifacts() {
    print_status "Cleaning test artifacts..."

    local files_removed=0

    # Remove test executables and artifacts
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*test*" -type f -executable -print0)

    # Remove coverage files
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*.gcda" -o -name "*.gcno" -o -name "*.gcov" -o -name "coverage.info" -o -name "lcov.info" -o -name "*.profraw" -type f -print0)

    # Remove test result files
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "CTestTestfile.cmake" -o -name "LastTest.log" -o -name "Test.xml" -type f -print0)

    if [ $files_removed -gt 0 ]; then
        print_success "Removed $files_removed test artifacts"
    else
        print_status "No test artifacts found"
    fi
}

# Function to remove IDE and editor files
remove_ide_files() {
    print_status "Removing IDE and editor files..."

    local files_removed=0

    # Remove common IDE directories
    while IFS= read -r -d '' dir; do
        rm -rf "$dir"
        ((files_removed++))
    done < <(find . -type d \( -name ".vscode" -o -name ".idea" -o -name ".vs" -o -name "build" -o -name "bin" -o -name "obj" \) -print0)

    # Remove editor backup files
    while IFS= read -r -d '' file; do
        rm -f "$file"
        ((files_removed++))
    done < <(find . -name "*.swp" -o -name "*.swo" -o -name "*~" -o -name ".#*" -type f -print0)

    if [ $files_removed -gt 0 ]; then
        print_success "Removed $files_removed IDE and editor files"
    else
        print_status "No IDE files found"
    fi
}

# Function to show disk space recovered
show_cleanup_summary() {
    print_status "Cleanup completed!"
    echo ""
    echo "The following types of files were removed:"
    echo "  - Build directories (all 'build' and '.qt' directories)"
    echo "  - CMake temporary files and CMakeFiles directories"
    echo "  - Qt generated files (MOC, autogen, qrc, ui files)"
    echo "  - Object files, libraries, and executables"
    echo "  - Test executables, coverage files, and test results"
    echo "  - IDE files (.vscode, .idea, etc.) and editor backups"
    echo "  - Temporary and backup files"
    echo ""
    echo "You can now rebuild the project cleanly with:"
    echo "  ./install.sh"
}

# Main cleanup process
main() {
    print_status "Starting LDM cleanup..."

    # Check if we're in the correct directory
    if [ ! -f "CMakeLists.txt" ]; then
        print_error "Please run this script from the LDM root directory"
        exit 1
    fi

    # Remove build directories
    remove_build_dirs

    # Remove CMake temporary files
    remove_cmake_files

    # Remove Qt generated files
    remove_qt_generated_files

    # Remove other build artifacts
    remove_other_artifacts

    # Clean test artifacts
    clean_test_artifacts

    # Remove IDE and editor files
    remove_ide_files

    # Show summary
    show_cleanup_summary
}

# Run main function
main "$@"
