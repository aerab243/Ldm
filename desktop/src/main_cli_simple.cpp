#include <iostream>
#include <string>
#include <vector>
#include <cstring>

class LDMCommandLine {
public:
    static void showVersion() {
        std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
        std::cout << "Developer: Anna-el Gerard RABENANDRASANA (aerab243)" << std::endl;
        std::cout << "Project: https://github.com/aerab243/ldm" << std::endl;
        std::cout << "Contact: aerabenandrasana@gmail.com" << std::endl;
        std::cout << "Description: Modern, feature-rich download manager for Linux" << std::endl;
    }
    
    static void showHelp(const char* progName) {
        std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
        std::cout << "Usage: " << progName << " [options] [URL]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  --version, -v        Show version information" << std::endl;
        std::cout << "  --help, -h           Show this help message" << std::endl;
        std::cout << "  --add-url URL        Add URL to download queue" << std::endl;
        std::cout << "  --output DIR         Set output directory" << std::endl;
        std::cout << "  --status             Show download status" << std::endl;
        std::cout << "  --list               List all downloads" << std::endl;
        std::cout << "  --info               Show system information" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << progName << " --version" << std::endl;
        std::cout << "  " << progName << " --add-url \"https://example.com/file.zip\"" << std::endl;
        std::cout << "  " << progName << " --add-url \"https://example.com/file.zip\" --output \"/home/user/Downloads\"" << std::endl;
        std::cout << std::endl;
        std::cout << "For GUI mode, use the desktop application." << std::endl;
    }
    
    static void showStatus() {
        std::cout << "=== LDM Status ===" << std::endl;
        std::cout << "Application: Running" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Mode: Command Line Interface" << std::endl;
        std::cout << "Downloads: 0 active, 0 completed" << std::endl;
        std::cout << "Queue: Empty" << std::endl;
        std::cout << "==================" << std::endl;
    }
    
    static void showInfo() {
        std::cout << "=== LDM System Information ===" << std::endl;
        std::cout << "Application: LDM - Like Download Manager" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << "Build Type: Command Line Interface" << std::endl;
        std::cout << "Developer: Anna-el Gerard RABENANDRASANA (aerab243)" << std::endl;
        std::cout << "Email: aerabenandrasana@gmail.com" << std::endl;
        std::cout << "Project URL: https://github.com/aerab243/ldm" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Features:" << std::endl;
        std::cout << "  ✓ Multi-platform support (Linux primary)" << std::endl;
        std::cout << "  ✓ Command-line interface" << std::endl;
        std::cout << "  ✓ URL download management" << std::endl;
        std::cout << "  ✓ Browser integration ready" << std::endl;
        std::cout << "  ✓ Modern C++20 implementation" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Technologies:" << std::endl;
        std::cout << "  • C++20" << std::endl;
        std::cout << "  • Qt 6.5+" << std::endl;
        std::cout << "  • libcurl" << std::endl;
        std::cout << "  • SQLite" << std::endl;
        std::cout << "  • CMake build system" << std::endl;
        std::cout << "==============================" << std::endl;
    }
    
    static void listDownloads() {
        std::cout << "=== Download List ===" << std::endl;
        std::cout << "No downloads in queue." << std::endl;
        std::cout << std::endl;
        std::cout << "To add a download, use:" << std::endl;
        std::cout << "  ldm --add-url \"https://example.com/file.zip\"" << std::endl;
        std::cout << "===================" << std::endl;
    }
    
    static void addUrl(const std::string& url, const std::string& output = "") {
        std::cout << "=== Adding Download ===" << std::endl;
        std::cout << "URL: " << url << std::endl;
        if (!output.empty()) {
            std::cout << "Output Directory: " << output << std::endl;
        } else {
            std::cout << "Output Directory: Default (~/Downloads)" << std::endl;
        }
        std::cout << "Status: Added to queue" << std::endl;
        std::cout << std::endl;
        std::cout << "Note: This is a demonstration. In the full version," << std::endl;
        std::cout << "the download would be processed by the download engine." << std::endl;
        std::cout << "=======================" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "LDM - Like Download Manager v1.0.0" << std::endl;
        std::cout << "Command-line interface mode" << std::endl;
        std::cout << "Use --help for usage information." << std::endl;
        return 0;
    }
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(std::string(argv[i]));
    }
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg == "--version" || arg == "-v") {
            LDMCommandLine::showVersion();
            return 0;
        }
        else if (arg == "--help" || arg == "-h") {
            LDMCommandLine::showHelp(argv[0]);
            return 0;
        }
        else if (arg == "--status") {
            LDMCommandLine::showStatus();
            return 0;
        }
        else if (arg == "--info") {
            LDMCommandLine::showInfo();
            return 0;
        }
        else if (arg == "--list") {
            LDMCommandLine::listDownloads();
            return 0;
        }
        else if (arg == "--add-url") {
            if (i + 1 < args.size()) {
                std::string url = args[i + 1];
                std::string output;
                
                // Check for --output option
                if (i + 3 < args.size() && args[i + 2] == "--output") {
                    output = args[i + 3];
                }
                
                LDMCommandLine::addUrl(url, output);
                return 0;
            } else {
                std::cerr << "Error: --add-url requires a URL argument" << std::endl;
                return 1;
            }
        }
        else if (arg == "--output") {
            // Skip this, it's handled with --add-url
            ++i; // Skip the next argument too
            continue;
        }
        else {
            std::cerr << "Error: Unknown option '" << arg << "'" << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return 1;
        }
    }
    
    return 0;
}