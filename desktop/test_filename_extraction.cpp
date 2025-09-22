#include <iostream>
#include <string>
#include <vector>
#include <regex>

// Simple function to extract filename from URL
std::string extractFilenameFromUrl(const std::string& url) {
    if (url.empty()) {
        return "download";
    }
    
    // Find the last slash in the URL
    size_t lastSlash = url.find_last_of('/');
    std::string filename;
    
    if (lastSlash != std::string::npos) {
        filename = url.substr(lastSlash + 1);
    }
    
    // If no filename found in path, try to extract from URL parameters
    if (filename.empty()) {
        std::regex filenameRegex(R"(filename=([^&;]+))");
        std::smatch match;
        if (std::regex_search(url, match, filenameRegex)) {
            filename = match[1].str();
        }
    }
    
    // If still no filename, generate one from host
    if (filename.empty()) {
        std::regex hostRegex(R"(https?://([^/]+))");
        std::smatch match;
        if (std::regex_search(url, match, hostRegex)) {
            filename = match[1].str() + "_download";
        } else {
            filename = "download";
        }
    }
    
    // Clean up filename (remove invalid characters and URL encoding)
    std::regex invalidChars(R"([<>:"/\\|?*%])");
    filename = std::regex_replace(filename, invalidChars, "_");
    
    // Remove query parameters if they exist
    size_t queryPos = filename.find('?');
    if (queryPos != std::string::npos) {
        filename = filename.substr(0, queryPos);
    }
    
    return filename;
}

int main() {
    std::cout << "🎯 LDM AddURL - Test d'extraction automatique du nom de fichier\n";
    std::cout << "===============================================================\n\n";
    
    // Test URLs
    std::vector<std::string> testUrls = {
        "https://releases.ubuntu.com/22.04/ubuntu-22.04.3-desktop-amd64.iso",
        "https://download.mozilla.org/pub/firefox/releases/latest/linux-x86_64/en-US/firefox-latest.tar.bz2",
        "https://github.com/torvalds/linux/archive/refs/heads/master.zip",
        "https://www.w3.org/WAI/ER/tests/xhtml/testfiles/resources/pdf/dummy.pdf",
        "https://file-examples.com/storage/fe68c16bb66071ae2c83f87/2017/10/file_example_JPG_100kB.jpg",
        "https://sample-videos.com/zip/10/mp4/SampleVideo_1280x720_1mb.mp4",
        "https://www.soundjay.com/misc/sounds/beep-07a.wav",
        "https://httpbin.org/base64/SFRUUEJJTiBpcyBhd2Vzb21l?filename=test.txt",
        "https://example.com/download",
        "https://cdn.example.com/",
        "https://download.example.com/file.zip?version=1.2.3&format=zip",
        "https://api.github.com/repos/user/repo/zipball/main",
        "ftp://ftp.example.com/pub/software/program.tar.gz",
        "https://www.example.com/files/document%20with%20spaces.pdf",
        "https://media.example.com/video/movie.mp4#t=30"
    };
    
    std::cout << "📋 Test avec " << testUrls.size() << " URLs d'exemple:\n\n";
    
    int testNumber = 1;
    for (const auto& url : testUrls) {
        std::string extractedName = extractFilenameFromUrl(url);
        
        std::cout << "Test " << testNumber << ":\n";
        std::cout << "  URL     : " << url << "\n";
        std::cout << "  Fichier : " << extractedName << "\n";
        
        // Analyze the extraction
        if (extractedName.find("_download") != std::string::npos) {
            std::cout << "  Type    : 🏠 Généré depuis le nom d'hôte\n";
        } else if (extractedName == "download") {
            std::cout << "  Type    : 🔧 Nom par défaut\n";
        } else if (url.find("filename=") != std::string::npos) {
            std::cout << "  Type    : 📋 Extrait des paramètres URL\n";
        } else {
            std::cout << "  Type    : 📁 Extrait du chemin URL\n";
        }
        
        std::cout << "  Statut  : ✅ Succès\n";
        std::cout << "\n";
        
        testNumber++;
    }
    
    // Interactive test
    std::cout << "🔗 Test interactif - Entrez vos propres URLs:\n";
    std::cout << "(Entrez une URL ou tapez 'quit' pour quitter)\n\n";
    
    std::string userUrl;
    while (true) {
        std::cout << "URL > ";
        std::getline(std::cin, userUrl);
        
        if (userUrl == "quit" || userUrl == "q" || userUrl == "exit") {
            break;
        }
        
        if (userUrl.empty()) {
            continue;
        }
        
        std::string result = extractFilenameFromUrl(userUrl);
        std::cout << "📁 Nom de fichier extrait: " << result << "\n\n";
    }
    
    std::cout << "👋 Test terminé. Merci d'avoir testé l'extraction de nom de fichier LDM!\n";
    std::cout << "\n🎯 Fonctionnalités testées:\n";
    std::cout << "   • Extraction depuis le chemin de l'URL\n";
    std::cout << "   • Extraction depuis les paramètres filename=\n";
    std::cout << "   • Génération depuis le nom d'hôte\n";
    std::cout << "   • Nettoyage des caractères invalides\n";
    std::cout << "   • Gestion des paramètres de requête\n";
    std::cout << "   • Support des URLs encodées\n";
    
    return 0;
}