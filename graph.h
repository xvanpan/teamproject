#pragma once
#include "common.h"
#include "station.h"
#include <string>
#include <vector>

class MetroGraph {
public:
    bool buildFromCsv(const std::string& fileName, const StationManager& stationManager);

    bool hasStation(int stationId) const;
    int vertexCount() const;
    const std::vector<Edge>& neighbors(int stationId) const;

private:
    std::vector<std::vector<Edge>> adj_;
};