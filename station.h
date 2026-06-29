#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>

class StationManager {
public:
    bool loadFromCsv(const std::string& fileName);
    bool saveToCsv(const std::string& fileName) const;

    bool resetFromInitCsv(const std::string& initFile, const std::string& targetFile);
    bool batchUpdateStatusFromCsv(const std::string& updateFile);

    const Station* getStationById(int id) const;
    Station* getStationById(int id);

    bool setStationStatus(int id, bool isOpen);
    bool isStationOpen(int id) const;

    std::vector<Station> getClosedStations() const;
    std::vector<Station> getStationsByLine(const std::string& line) const;
    std::vector<int> searchStationByName(const std::string& name) const;

    const std::vector<Station>& getAllStations() const;

private:
    std::vector<Station> stations_;
    std::unordered_map<int, int> idToIndex_;
};