#pragma once
#include <cctype>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>

const int INF = 1000000000;
const int TRANSFER_TIME = 5;

enum class StationStatus {
    Open,
    Closed
};

enum class EdgeType {
    Normal,
    Transfer
};

struct Station {
    int id;
    std::string name;
    std::string line;
    StationStatus status = StationStatus::Open;
};

struct Edge {
    int from;
    int to;
    std::string line;
    std::string direction;
    int timeCost = 0;
    EdgeType type = EdgeType::Normal;
};

struct PathResult {
    std::vector<int> stationIds;
    std::vector<std::string> stationNames;
    std::vector<std::string> lines;
    std::vector<std::string> transferStations;
    int totalTime = 0;
    int transferCount = 0;
    int passedStationCount = 0;
    bool valid = false;
};

inline std::string trimCsvField(std::string text)
{
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
        text.erase(text.begin());
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
        text.pop_back();
    }
    return text;
}

inline std::string resolveDataPath(const std::string& fileName)
{
    auto isAbsolutePath = [](const std::string& path) {
        return path.size() >= 2 && path[1] == ':'
            || path.size() >= 2 && path[0] == '\\' && path[1] == '\\'
            || !path.empty() && (path[0] == '/' || path[0] == '\\');
    };

    auto hasDirectoryPart = [](const std::string& path) {
        return path.find('/') != std::string::npos || path.find('\\') != std::string::npos;
    };

    auto joinPath = [](const std::string& dir, const std::string& name) {
        if (dir.empty() || dir.back() == '\\' || dir.back() == '/') {
            return dir + name;
        }
        return dir + "\\" + name;
    };

    auto parentPath = [](const std::string& path) {
        size_t pos = path.find_last_of("\\/");
        if (pos == std::string::npos) {
            return std::string();
        }
        if (pos == 2 && path.size() >= 3 && path[1] == ':') {
            return path.substr(0, 3);
        }
        return path.substr(0, pos);
    };

    auto fileExists = [](const std::string& path) {
        std::ifstream file(path);
        return file.good();
    };

    if (isAbsolutePath(fileName)) {
        return fileName;
    }

    char buffer[FILENAME_MAX];
    std::string current = (_getcwd(buffer, sizeof(buffer)) != nullptr) ? buffer : ".";
    std::vector<std::string> candidates;

    for (int i = 0; i < 6 && !current.empty(); ++i) {
        candidates.push_back(joinPath(current, fileName));
        if (!hasDirectoryPart(fileName)) {
            candidates.push_back(joinPath(joinPath(current, "data"), fileName));
        }

        std::string parent = parentPath(current);
        if (parent == current) {
            break;
        }
        current = parent;
    }

    for (const auto& candidate : candidates) {
        if (fileExists(candidate)) {
            return candidate;
        }
    }

    return joinPath(current, fileName);
}
