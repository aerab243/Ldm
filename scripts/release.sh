#!/bin/bash

# Linux Download Manager Release Script
# This script helps create new releases with proper versioning and tagging
# Developed by: Anna-el Gerard RABENANDRASANA (aerab243)
# Project: https://github.com/aerab243/ldm

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default values
DRY_RUN=false
FORCE=false
PRERELEASE=false
RELEASE_TYPE=""

# Function to print colored output
print_info() {
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

# Function to show usage
show_usage() {
    cat << EOF
Usage: $0 [OPTIONS] <release_type>

Creates a new release for Linux Download Manager with proper versioning and tagging.

RELEASE TYPES:
    major       Increment major version (X.0.0)
    minor       Increment minor version (x.Y.0)
    patch       Increment patch version (x.y.Z)
    custom      Specify custom version (use with --version)

OPTIONS:
    -v, --version VERSION   Specify custom version (e.g., 1.2.3)
    -p, --prerelease        Mark as prerelease (adds -rc.N suffix)
    -d, --dry-run          Show what would be done without making changes
    -f, --force            Force release even if working directory is dirty
    -h, --help             Show this help message

EXAMPLES:
    $0 patch                    # Increment patch version
    $0 minor                    # Increment minor version
    $0 major                    # Increment major version
    $0 custom --version 2.0.0   # Set specific version
    $0 patch --prerelease       # Create prerelease (e.g., 1.0.1-rc.1)
    $0 minor --dry-run          # Preview what would happen

EOF
}

# Function to get current version
get_current_version() {
    if [ -f "$PROJECT_DIR/VERSION" ]; then
        cat "$PROJECT_DIR/VERSION"
    else
        print_error "VERSION file not found"
        exit 1
    fi
}

# Function to validate version format
validate_version() {
    local version="$1"
    if [[ ! $version =~ ^[0-9]+\.[0-9]+\.[0-9]+(-rc\.[0-9]+)?$ ]]; then
        print_error "Invalid version format: $version"
        print_error "Expected format: X.Y.Z or X.Y.Z-rc.N"
        exit 1
    fi
}

# Function to increment version
increment_version() {
    local current_version="$1"
    local release_type="$2"

    # Remove any prerelease suffix for base version calculation
    local base_version=$(echo "$current_version" | sed 's/-rc\.[0-9]*$//')

    IFS='.' read -ra VERSION_PARTS <<< "$base_version"
    local major=${VERSION_PARTS[0]}
    local minor=${VERSION_PARTS[1]}
    local patch=${VERSION_PARTS[2]}

    case $release_type in
        major)
            major=$((major + 1))
            minor=0
            patch=0
            ;;
        minor)
            minor=$((minor + 1))
            patch=0
            ;;
        patch)
            patch=$((patch + 1))
            ;;
        *)
            print_error "Invalid release type: $release_type"
            exit 1
            ;;
    esac

    echo "${major}.${minor}.${patch}"
}

# Function to get next prerelease version
get_prerelease_version() {
    local base_version="$1"
    local current_version="$2"

    if [[ $current_version == *"-rc."* ]]; then
        # Extract current RC number and increment
        local rc_num=$(echo "$current_version" | sed 's/.*-rc\.\([0-9]*\)$/\1/')
        rc_num=$((rc_num + 1))
        echo "${base_version}-rc.${rc_num}"
    else
        # First prerelease
        echo "${base_version}-rc.1"
    fi
}

# Function to check if working directory is clean
check_working_directory() {
    if [ "$FORCE" = "false" ] && [ -n "$(git status --porcelain)" ]; then
        print_error "Working directory is not clean. Commit or stash changes first."
        print_error "Use --force to override this check."
        exit 1
    fi
}

# Function to check if we're on main branch
check_branch() {
    local current_branch=$(git rev-parse --abbrev-ref HEAD)
    if [ "$current_branch" != "main" ] && [ "$current_branch" != "master" ]; then
        print_warning "You are not on the main branch (current: $current_branch)"
        read -p "Continue anyway? [y/N] " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            exit 1
        fi
    fi
}

# Function to update version files
update_version_files() {
    local new_version="$1"

    print_info "Updating version files..."

    # Update VERSION file
    echo "$new_version" > "$PROJECT_DIR/VERSION"

    # Update CMakeLists.txt
    sed -i "s/project(LDM VERSION [0-9.]*/project(LDM VERSION $new_version/" "$PROJECT_DIR/CMakeLists.txt"

    # Update package.json if it exists (for browser extension)
    if [ -f "$PROJECT_DIR/browser-extension/package.json" ]; then
        sed -i "s/\"version\": \"[0-9.]*\"/\"version\": \"$new_version\"/" "$PROJECT_DIR/browser-extension/package.json"
    fi

    # Update manifest.json if it exists
    if [ -f "$PROJECT_DIR/browser-extension/manifest.json" ]; then
        sed -i "s/\"version\": \"[0-9.]*\"/\"version\": \"$new_version\"/" "$PROJECT_DIR/browser-extension/manifest.json"
    fi
}

# Function to update changelog
update_changelog() {
    local new_version="$1"
    local current_date=$(date +%Y-%m-%d)

    if [ -f "$PROJECT_DIR/CHANGELOG.md" ]; then
        print_info "Updating CHANGELOG.md..."

        # Create temporary file with new entry
        cat > /tmp/changelog_entry << EOF
## [$new_version] - $current_date

### Added
- New features and improvements

### Changed
- Updated dependencies and performance improvements

### Fixed
- Bug fixes and stability improvements

EOF

        # Insert new entry after the first line (usually # Changelog)
        sed -i '1r /tmp/changelog_entry' "$PROJECT_DIR/CHANGELOG.md"
        rm /tmp/changelog_entry

        print_info "Please edit CHANGELOG.md to add specific changes for this release"
    else
        print_warning "CHANGELOG.md not found. Creating one..."
        cat > "$PROJECT_DIR/CHANGELOG.md" << EOF
# Changelog - Linux Download Manager

All notable changes to Linux Download Manager will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

**Developer:** Anna-el Gerard RABENANDRASANA
**Project:** https://github.com/aerab243/ldm

## [$new_version] - $current_date

### Added
- Initial release of Linux Download Manager

EOF
    fi
}

# Function to create git tag and commit
create_release_commit() {
    local new_version="$1"
    local tag_name="v$new_version"

    print_info "Creating release commit and tag..."

    # Add changed files
    git add VERSION CMakeLists.txt CHANGELOG.md

    # Add browser extension files if they exist
    [ -f "$PROJECT_DIR/browser-extension/package.json" ] && git add browser-extension/package.json
    [ -f "$PROJECT_DIR/browser-extension/manifest.json" ] && git add browser-extension/manifest.json

    # Commit changes
    git commit -m "Release Linux Download Manager $new_version

- Updated version files
- Updated changelog
- Prepared for release $new_version

Developed by: Anna-el Gerard RABENANDRASANA
Project: https://github.com/aerab243/ldm"

    # Create tag
    if [ "$PRERELEASE" = "true" ]; then
        git tag -a "$tag_name" -m "Prerelease $new_version"
    else
        git tag -a "$tag_name" -m "Release $new_version"
    fi

    print_success "Created commit and tag $tag_name"
}

# Function to push changes
push_changes() {
    local new_version="$1"
    local tag_name="v$new_version"

    print_info "Pushing changes to remote repository..."

    # Push commit
    git push origin $(git rev-parse --abbrev-ref HEAD)

    # Push tag
    git push origin "$tag_name"

    print_success "Pushed changes and tag to remote repository"
}

# Main function
main() {
    cd "$PROJECT_DIR"

    # Check if git repository
    if [ ! -d ".git" ]; then
        print_error "Not a git repository"
        exit 1
    fi

    # Get current version
    local current_version=$(get_current_version)
    print_info "Current version: $current_version"

    # Calculate new version
    local new_version
    if [ "$RELEASE_TYPE" = "custom" ]; then
        if [ -z "$CUSTOM_VERSION" ]; then
            print_error "Custom version not specified. Use --version option."
            exit 1
        fi
        new_version="$CUSTOM_VERSION"
    else
        new_version=$(increment_version "$current_version" "$RELEASE_TYPE")
    fi

    # Handle prerelease
    if [ "$PRERELEASE" = "true" ]; then
        new_version=$(get_prerelease_version "$new_version" "$current_version")
    fi

    # Validate new version
    validate_version "$new_version"

    print_info "New version will be: $new_version"

    if [ "$DRY_RUN" = "true" ]; then
        print_info "DRY RUN - No changes will be made"
        print_info "Would update:"
        print_info "  - VERSION file: $current_version → $new_version"
        print_info "  - CMakeLists.txt project version"
        print_info "  - CHANGELOG.md with new entry"
        print_info "  - Git commit and tag: v$new_version"
        exit 0
    fi

    # Confirm release
    print_warning "This will create release $new_version"
    read -p "Continue? [y/N] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "Release cancelled"
        exit 0
    fi

    # Pre-flight checks
    check_working_directory
    check_branch

    # Update files
    update_version_files "$new_version"
    update_changelog "$new_version"

    # Create commit and tag
    create_release_commit "$new_version"

    # Push changes
    push_changes "$new_version"

    print_success "Linux Download Manager release $new_version created successfully!"
    print_info "GitHub Actions will automatically build and publish the release."
    print_info "Release URL: https://github.com/aerab243/ldm/releases/tag/v$new_version"
    print_info ""
    print_info "Developed by: Anna-el Gerard RABENANDRASANA (aerab243)"
    print_info "Contact: aerabenandrasana@gmail.com"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -v|--version)
            CUSTOM_VERSION="$2"
            shift 2
            ;;
        -p|--prerelease)
            PRERELEASE=true
            shift
            ;;
        -d|--dry-run)
            DRY_RUN=true
            shift
            ;;
        -f|--force)
            FORCE=true
            shift
            ;;
        -h|--help)
            show_usage
            exit 0
            ;;
        major|minor|patch|custom)
            RELEASE_TYPE="$1"
            shift
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Check if release type is specified
if [ -z "$RELEASE_TYPE" ]; then
    print_error "Release type not specified"
    show_usage
    exit 1
fi

# Run main function
main
