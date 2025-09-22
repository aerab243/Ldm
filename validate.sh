#!/bin/bash
# LDM - Like Download Manager Validation Script
# Comprehensive testing before GitHub release

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

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_PATH="$SCRIPT_DIR/desktop/ldm-complete"
TEST_OUTPUT_DIR="$SCRIPT_DIR/validation_results"
LOG_FILE="$TEST_OUTPUT_DIR/validation.log"
ERRORS_FILE="$TEST_OUTPUT_DIR/errors.log"

# Statistics
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0
WARNING_TESTS=0

# Print functions
print_header() {
    echo -e "\n${CYAN}================================${NC}"
    echo -e "${WHITE}$1${NC}"
    echo -e "${CYAN}================================${NC}\n"
}

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
}

print_success() {
    echo -e "${GREEN}[PASS]${NC} $1"
    PASSED_TESTS=$((PASSED_TESTS + 1))
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
    WARNING_TESTS=$((WARNING_TESTS + 1))
}

print_error() {
    echo -e "${RED}[FAIL]${NC} $1"
    FAILED_TESTS=$((FAILED_TESTS + 1))
    echo "$(date): $1" >> "$ERRORS_FILE"
}

print_info() {
    echo -e "${PURPLE}[INFO]${NC} $1"
}

# Setup validation environment
setup_validation() {
    print_step "Setting up validation environment"

    # Create output directory
    mkdir -p "$TEST_OUTPUT_DIR"
    rm -f "$LOG_FILE" "$ERRORS_FILE"

    # Log system information
    {
        echo "LDM Validation Report"
        echo "===================="
        echo "Date: $(date)"
        echo "System: $(uname -a)"
        echo "User: $(whoami)"
        echo "Directory: $SCRIPT_DIR"
        echo ""
    } > "$LOG_FILE"

    print_success "Validation environment ready"
}

# Test 1: Project structure validation
test_project_structure() {
    print_step "Validating project structure"

    local required_files=(
        "README.md"
        "LICENSE"
        "CONTRIBUTING.md"
        "install.sh"
        "demo.sh"
        "desktop/Makefile.complete"
        "desktop/Makefile.idm"
        "desktop/src/main_complete.cpp"
        "desktop/src/main_idm.cpp"
    )

    local missing_files=0
    for file in "${required_files[@]}"; do
        if [ ! -f "$SCRIPT_DIR/$file" ]; then
            print_error "Missing required file: $file"
            missing_files=$((missing_files + 1))
        fi
    done

    if [ $missing_files -eq 0 ]; then
        print_success "All required files present"
    else
        print_error "$missing_files required files missing"
    fi

    # Check directory structure
    local required_dirs=(
        "desktop/src/core"
        "desktop/src/ui"
        "desktop/src/utils"
        "docs"
        "scripts"
    )

    local missing_dirs=0
    for dir in "${required_dirs[@]}"; do
        if [ ! -d "$SCRIPT_DIR/$dir" ]; then
            print_warning "Missing directory: $dir"
            missing_dirs=$((missing_dirs + 1))
        fi
    done

    if [ $missing_dirs -eq 0 ]; then
        print_success "All required directories present"
    else
        print_warning "$missing_dirs directories missing (non-critical)"
    fi
}

# Test 2: Dependencies validation
test_dependencies() {
    print_step "Validating build dependencies"

    # Check compiler
    if command -v g++ &> /dev/null; then
        local gcc_version=$(g++ --version | head -1)
        print_success "G++ compiler found: $gcc_version"
    else
        print_error "G++ compiler not found"
        return 1
    fi

    # Check make
    if command -v make &> /dev/null; then
        print_success "Make utility found"
    else
        print_error "Make utility not found"
    fi

    # Check Qt6 packages
    local qt_packages=("Qt6Core" "Qt6Widgets" "Qt6Network" "Qt6Gui")
    for package in "${qt_packages[@]}"; do
        if pkg-config --exists "$package"; then
            local version=$(pkg-config --modversion "$package")
            print_success "$package found: $version"
        else
            print_error "$package not found"
        fi
    done

    # Check libraries
    if ldconfig -p | grep libcurl >/dev/null 2>&1; then
        print_success "libcurl found"
    else
        print_error "libcurl not found"
    fi

    if ldconfig -p | grep libssl >/dev/null 2>&1; then
        print_success "OpenSSL found"
    else
        print_error "OpenSSL not found"
    fi

    # Check MOC
    local moc_paths=(
        "/usr/lib64/qt6/libexec/moc"
        "/usr/lib/qt6/libexec/moc"
        "/usr/lib/x86_64-linux-gnu/qt6/libexec/moc"
    )

    local moc_found=false
    for moc_path in "${moc_paths[@]}"; do
        if [ -x "$moc_path" ]; then
            print_success "MOC found: $moc_path"
            moc_found=true
            break
        fi
    done

    if [ "$moc_found" = false ]; then
        if command -v moc &> /dev/null || command -v moc-qt6 &> /dev/null; then
            print_success "MOC found in PATH"
        else
            print_warning "MOC not found (may affect build)"
        fi
    fi
}

# Test 3: Build validation
test_build_process() {
    print_step "Validating build process"

    cd "$SCRIPT_DIR/desktop"

    # Clean previous builds
    print_info "Cleaning previous builds..."
    make -f Makefile.complete clean >/dev/null 2>&1 || true
    make -f Makefile.idm clean >/dev/null 2>&1 || true

    # Test complete build
    print_info "Building complete application..."
    if make -f Makefile.complete >>"$LOG_FILE" 2>&1; then
        print_success "Complete application build successful"

        # Check binary exists and is executable
        if [ -x "./ldm-complete" ]; then
            print_success "Complete application binary is executable"

            # Check file size (should be reasonable)
            local file_size=$(stat -c%s "./ldm-complete" 2>/dev/null || echo 0)
            local file_size_mb=$((file_size / 1024 / 1024))
            if [ $file_size_mb -gt 0 ] && [ $file_size_mb -lt 100 ]; then
                print_success "Binary size reasonable: ${file_size_mb}MB"
            else
                print_warning "Binary size unusual: ${file_size_mb}MB"
            fi
        else
            print_error "Complete application binary not executable"
        fi
    else
        print_error "Complete application build failed"
        echo "Build output:" >> "$ERRORS_FILE"
        tail -20 "$LOG_FILE" >> "$ERRORS_FILE"
    fi

    # Test IDM interface build
    print_info "Building IDM interface..."
    if make -f Makefile.idm >>"$LOG_FILE" 2>&1; then
        print_success "IDM interface build successful"

        if [ -x "./ldm-idm" ]; then
            print_success "IDM interface binary is executable"
        else
            print_error "IDM interface binary not executable"
        fi
    else
        print_error "IDM interface build failed"
    fi

    cd "$SCRIPT_DIR"
}

# Test 4: Application startup validation
test_application_startup() {
    print_step "Validating application startup"

    if [ ! -x "$APP_PATH" ]; then
        print_error "Application binary not found or not executable"
        return 1
    fi

    cd "$SCRIPT_DIR/desktop"

    # Test version command
    print_info "Testing version command..."
    if timeout 5s ./ldm-complete --version >/dev/null 2>&1; then
        print_success "Version command works"
    else
        print_warning "Version command failed or timed out"
    fi

    # Test help command
    print_info "Testing help command..."
    if timeout 5s ./ldm-complete --help >/dev/null 2>&1; then
        print_success "Help command works"
    else
        print_warning "Help command failed or timed out"
    fi

    # Test basic startup (with timeout)
    print_info "Testing basic application startup..."
    ./ldm-complete &
    local app_pid=$!
    sleep 3

    if ps -p $app_pid > /dev/null; then
        print_success "Application started successfully"

        # Check memory usage
        local memory=$(ps -o rss= -p $app_pid 2>/dev/null || echo 0)
        local memory_mb=$((memory / 1024))
        if [ $memory_mb -lt 200 ]; then
            print_success "Memory usage reasonable: ${memory_mb}MB"
        else
            print_warning "Memory usage high: ${memory_mb}MB"
        fi

        # Terminate gracefully
        kill $app_pid 2>/dev/null || true
        sleep 2
        kill -9 $app_pid 2>/dev/null || true
        wait $app_pid 2>/dev/null || true
    else
        print_error "Application failed to start or crashed"
    fi

    cd "$SCRIPT_DIR"
}

# Test 5: Code quality validation
test_code_quality() {
    print_step "Validating code quality"

    local cpp_files=$(find "$SCRIPT_DIR/desktop/src" -name "*.cpp" -o -name "*.h" 2>/dev/null | wc -l)
    if [ $cpp_files -gt 0 ]; then
        print_success "Found $cpp_files C++ source files"
    else
        print_error "No C++ source files found"
    fi

    # Check for common issues in main files
    local main_files=(
        "desktop/src/main_complete.cpp"
        "desktop/src/main_idm.cpp"
    )

    for file in "${main_files[@]}"; do
        if [ -f "$SCRIPT_DIR/$file" ]; then
            # Check file size
            local file_size=$(wc -l < "$SCRIPT_DIR/$file")
            if [ $file_size -gt 100 ]; then
                print_success "$file has substantial content ($file_size lines)"
            else
                print_warning "$file is quite small ($file_size lines)"
            fi

            # Check for Qt includes
            if grep -q "#include.*Qt" "$SCRIPT_DIR/$file"; then
                print_success "$file includes Qt headers"
            else
                print_warning "$file missing Qt includes"
            fi

            # Check for main function
            if grep -q "int main" "$SCRIPT_DIR/$file"; then
                print_success "$file contains main function"
            else
                print_error "$file missing main function"
            fi
        fi
    done

    # Check for TODO/FIXME comments
    local todo_count=$(find "$SCRIPT_DIR/desktop/src" -name "*.cpp" -o -name "*.h" | xargs grep -i "TODO\|FIXME" 2>/dev/null | wc -l)
    if [ $todo_count -eq 0 ]; then
        print_success "No TODO/FIXME comments found"
    else
        print_info "Found $todo_count TODO/FIXME comments"
    fi
}

# Test 6: Documentation validation
test_documentation() {
    print_step "Validating documentation"

    # Check README.md
    if [ -f "$SCRIPT_DIR/README.md" ]; then
        local readme_size=$(wc -l < "$SCRIPT_DIR/README.md")
        if [ $readme_size -gt 50 ]; then
            print_success "README.md is comprehensive ($readme_size lines)"
        else
            print_warning "README.md might be too brief ($readme_size lines)"
        fi

        # Check for key sections
        local sections=("Installation" "Features" "Usage" "Building" "Contributing")
        for section in "${sections[@]}"; do
            if grep -qi "$section" "$SCRIPT_DIR/README.md"; then
                print_success "README contains $section section"
            else
                print_warning "README missing $section section"
            fi
        done
    else
        print_error "README.md not found"
    fi

    # Check CONTRIBUTING.md
    if [ -f "$SCRIPT_DIR/CONTRIBUTING.md" ]; then
        local contrib_size=$(wc -l < "$SCRIPT_DIR/CONTRIBUTING.md")
        if [ $contrib_size -gt 20 ]; then
            print_success "CONTRIBUTING.md exists ($contrib_size lines)"
        else
            print_warning "CONTRIBUTING.md might be incomplete"
        fi
    else
        print_warning "CONTRIBUTING.md not found"
    fi

    # Check LICENSE
    if [ -f "$SCRIPT_DIR/LICENSE" ]; then
        print_success "LICENSE file exists"
        if grep -qi "MIT\|GPL\|Apache\|BSD" "$SCRIPT_DIR/LICENSE"; then
            print_success "LICENSE appears to be a standard license"
        else
            print_warning "LICENSE might be custom or unclear"
        fi
    else
        print_error "LICENSE file not found"
    fi
}

# Test 7: Installation script validation
test_installation_scripts() {
    print_step "Validating installation scripts"

    # Check install.sh
    if [ -f "$SCRIPT_DIR/install.sh" ]; then
        if [ -x "$SCRIPT_DIR/install.sh" ]; then
            print_success "install.sh is executable"

            # Check for help option
            if "$SCRIPT_DIR/install.sh" --help >/dev/null 2>&1; then
                print_success "install.sh supports --help"
            else
                print_warning "install.sh might not support --help"
            fi
        else
            print_error "install.sh is not executable"
        fi
    else
        print_error "install.sh not found"
    fi

    # Check demo.sh
    if [ -f "$SCRIPT_DIR/demo.sh" ]; then
        if [ -x "$SCRIPT_DIR/demo.sh" ]; then
            print_success "demo.sh is executable"
        else
            print_error "demo.sh is not executable"
        fi
    else
        print_warning "demo.sh not found"
    fi

    # Check Makefiles
    local makefiles=(
        "desktop/Makefile.complete"
        "desktop/Makefile.idm"
    )

    for makefile in "${makefiles[@]}"; do
        if [ -f "$SCRIPT_DIR/$makefile" ]; then
            print_success "$makefile exists"

            # Check for key targets
            if grep -q "clean:" "$SCRIPT_DIR/$makefile"; then
                print_success "$makefile has clean target"
            else
                print_warning "$makefile missing clean target"
            fi
        else
            print_error "$makefile not found"
        fi
    done
}

# Test 8: Security validation
test_security() {
    print_step "Validating security aspects"

    # Check for hardcoded credentials
    local security_patterns=(
        "password.*="
        "secret.*="
        "key.*="
        "token.*="
    )

    local security_issues=0
    for pattern in "${security_patterns[@]}"; do
        local matches=$(find "$SCRIPT_DIR" -name "*.cpp" -o -name "*.h" | xargs grep -i "$pattern" 2>/dev/null | wc -l)
        if [ $matches -gt 0 ]; then
            print_warning "Found $matches potential security issues with pattern: $pattern"
            security_issues=$((security_issues + matches))
        fi
    done

    if [ $security_issues -eq 0 ]; then
        print_success "No obvious security issues found"
    else
        print_warning "Found $security_issues potential security issues"
    fi

    # Check file permissions
    if [ -f "$APP_PATH" ]; then
        local perms=$(stat -c "%a" "$APP_PATH" 2>/dev/null || echo "000")
        if [[ "$perms" =~ ^[74][0-7][0-7]$ ]]; then
            print_success "Application binary has appropriate permissions: $perms"
        else
            print_warning "Application binary permissions unusual: $perms"
        fi
    fi

    # Check for setuid/setgid
    local setuid_files=$(find "$SCRIPT_DIR" -perm /6000 2>/dev/null | wc -l)
    if [ $setuid_files -eq 0 ]; then
        print_success "No setuid/setgid files found"
    else
        print_warning "Found $setuid_files setuid/setgid files (review needed)"
    fi
}

# Test 9: Performance validation
test_performance() {
    print_step "Validating performance characteristics"

    cd "$SCRIPT_DIR/desktop"

    if [ -x "./ldm-complete" ]; then
        # Test startup time
        print_info "Measuring startup time..."
        local start_time=$(date +%s%N)
        timeout 10s ./ldm-complete --version >/dev/null 2>&1 || true
        local end_time=$(date +%s%N)
        local startup_time=$(( (end_time - start_time) / 1000000 ))

        if [ $startup_time -lt 5000 ]; then
            print_success "Fast startup time: ${startup_time}ms"
        elif [ $startup_time -lt 10000 ]; then
            print_success "Reasonable startup time: ${startup_time}ms"
        else
            print_warning "Slow startup time: ${startup_time}ms"
        fi

        # Test binary size
        local binary_size=$(stat -c%s "./ldm-complete" 2>/dev/null || echo 0)
        local binary_size_mb=$((binary_size / 1024 / 1024))

        if [ $binary_size_mb -lt 20 ]; then
            print_success "Compact binary size: ${binary_size_mb}MB"
        elif [ $binary_size_mb -lt 50 ]; then
            print_success "Reasonable binary size: ${binary_size_mb}MB"
        else
            print_warning "Large binary size: ${binary_size_mb}MB"
        fi

        # Test memory usage during startup
        print_info "Testing memory usage..."
        ./ldm-complete &
        local app_pid=$!
        sleep 2

        if ps -p $app_pid > /dev/null; then
            local memory=$(ps -o rss= -p $app_pid 2>/dev/null || echo 0)
            local memory_mb=$((memory / 1024))

            if [ $memory_mb -lt 50 ]; then
                print_success "Low memory usage: ${memory_mb}MB"
            elif [ $memory_mb -lt 100 ]; then
                print_success "Reasonable memory usage: ${memory_mb}MB"
            else
                print_warning "High memory usage: ${memory_mb}MB"
            fi

            kill $app_pid 2>/dev/null || true
            wait $app_pid 2>/dev/null || true
        fi
    fi

    cd "$SCRIPT_DIR"
}

# Test 10: Integration validation
test_integration() {
    print_step "Validating integration aspects"

    # Test that scripts work together
    print_info "Testing script integration..."

    # Test demo script
    if [ -x "$SCRIPT_DIR/demo.sh" ]; then
        if "$SCRIPT_DIR/demo.sh" --help >/dev/null 2>&1; then
            print_success "Demo script integration works"
        else
            print_warning "Demo script integration issues"
        fi
    fi

    # Test makefile dependencies
    cd "$SCRIPT_DIR/desktop"
    if make -f Makefile.complete check-deps >/dev/null 2>&1; then
        print_success "Makefile dependency checking works"
    else
        print_warning "Makefile dependency checking issues"
    fi

    # Test clean and rebuild
    print_info "Testing clean and rebuild cycle..."
    if make -f Makefile.complete clean >/dev/null 2>&1 && \
       make -f Makefile.complete >/dev/null 2>&1; then
        print_success "Clean and rebuild cycle works"
    else
        print_error "Clean and rebuild cycle failed"
    fi

    cd "$SCRIPT_DIR"
}

# Generate validation report
generate_report() {
    print_header "Validation Report"

    local report_file="$TEST_OUTPUT_DIR/validation_report.md"

    {
        echo "# LDM Validation Report"
        echo ""
        echo "**Date:** $(date)"
        echo "**System:** $(uname -s) $(uname -r)"
        echo "**Architecture:** $(uname -m)"
        echo ""
        echo "## Summary"
        echo ""
        echo "- **Total Tests:** $TOTAL_TESTS"
        echo "- **Passed:** $PASSED_TESTS"
        echo "- **Failed:** $FAILED_TESTS"
        echo "- **Warnings:** $WARNING_TESTS"
        echo ""

        local success_rate=0
        if [ $TOTAL_TESTS -gt 0 ]; then
            success_rate=$(( (PASSED_TESTS * 100) / TOTAL_TESTS ))
        fi

        echo "**Success Rate:** ${success_rate}%"
        echo ""

        if [ $FAILED_TESTS -eq 0 ]; then
            echo "## ✅ Status: READY FOR RELEASE"
            echo ""
            echo "All critical tests passed. The project is ready for GitHub publication."
        elif [ $FAILED_TESTS -le 2 ]; then
            echo "## ⚠️ Status: MOSTLY READY"
            echo ""
            echo "Minor issues detected. Review and fix before release."
        else
            echo "## ❌ Status: NOT READY"
            echo ""
            echo "Significant issues detected. Major fixes required before release."
        fi

        echo ""
        echo "## Test Details"
        echo ""
        echo "Detailed logs available in:"
        echo "- \`validation_results/validation.log\`"
        echo "- \`validation_results/errors.log\`"
        echo ""

        if [ -f "$ERRORS_FILE" ] && [ -s "$ERRORS_FILE" ]; then
            echo "## Errors Found"
            echo ""
            echo "\`\`\`"
            cat "$ERRORS_FILE"
            echo "\`\`\`"
        fi

    } > "$report_file"

    echo "📊 Validation Statistics:"
    echo "   Total Tests: $TOTAL_TESTS"
    echo "   Passed: $PASSED_TESTS"
    echo "   Failed: $FAILED_TESTS"
    echo "   Warnings: $WARNING_TESTS"
    echo "   Success Rate: ${success_rate}%"
    echo ""

    if [ $FAILED_TESTS -eq 0 ]; then
        print_success "✅ ALL TESTS PASSED - READY FOR GITHUB RELEASE!"
    elif [ $FAILED_TESTS -le 2 ]; then
        print_warning "⚠️ Minor issues found - Review before release"
    else
        print_error "❌ Significant issues found - Fixes required"
    fi

    echo ""
    print_info "📋 Full report saved to: $report_file"
    print_info "📝 Logs available in: $TEST_OUTPUT_DIR/"
}

# Main validation function
main_validation() {
    print_header "LDM - Like Download Manager Validation"
    print_info "Comprehensive testing for GitHub release readiness"

    setup_validation

    # Run all validation tests
    test_project_structure
    test_dependencies
    test_build_process
    test_application_startup
    test_code_quality
    test_documentation
    test_installation_scripts
    test_security
    test_performance
    test_integration

    generate_report

    print_header "Validation Complete"

    # Exit with appropriate code
    if [ $FAILED_TESTS -eq 0 ]; then
        exit 0
    else
        exit 1
    fi
}

# Handle command line arguments
case "${1:-validate}" in
    validate|--validate)
        main_validation
        ;;
    quick|--quick)
        setup_validation
        test_dependencies
        test_build_process
        test_application_startup
        generate_report
        ;;
    build|--build)
        setup_validation
        test_dependencies
        test_build_process
        generate_report
        ;;
    security|--security)
        setup_validation
        test_security
        generate_report
        ;;
    help|--help|-h)
        echo "LDM Validation Script"
        echo ""
        echo "Usage: $0 [OPTION]"
        echo ""
        echo "Options:"
        echo "  validate   Full validation (default)"
        echo "  quick      Quick validation"
        echo "  build      Build validation only"
        echo "  security   Security validation only"
        echo "  help       Show this help"
        echo ""
        echo "The script validates:"
        echo "  • Project structure"
        echo "  • Dependencies"
        echo "  • Build process"
        echo "  • Application startup"
        echo "  • Code quality"
        echo "  • Documentation"
        echo "  • Installation scripts"
        echo "  • Security aspects"
        echo "  • Performance"
        echo "  • Integration"
        ;;
    *)
        print_error "Unknown option: $1"
        echo "Use --help for usage information"
        exit 1
        ;;
esac
