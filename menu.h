#pragma once
#include "station.h"
#include "graph.h"
#include "pathfinder.h"

class MenuSystem {
public:
    MenuSystem(StationManager& stationManager, MetroGraph& graph, PathFinder& pathFinder);
    void run();

private:
    void showMainMenu() const;
    void showStationMenu() const;
    void showTimePathMenu() const;
    void showTransferPathMenu() const;

    void handleStationMenu();
    void handleTimePathMenu();
    void handleTransferPathMenu();

    void queryShortestTimePath();
    void queryKShortestTimePaths();
    void queryMinTransferPath();
    void queryKMinTransferPaths();

    void batchUpdateStationStatus();
    void manualUpdateStationStatus();
    void resetStationStatus();
    void showClosedStations();
    void showLineInfo();
    void analyzeAffectedStations();
    void searchStationInfo();

    void printPathResult(const PathResult& result) const;
    int selectStationByKeyword(const std::string& prompt) const;
    bool readPathEndpoints(int& startId, int& endId) const;
    bool validatePathEndpoints(int startId, int endId) const;
    void printClosedEndpointMessage(const std::string& endpointName, int stationId) const;
    bool isSamePhysicalStation(int leftId, int rightId) const;
    std::string lineNumberToName(int lineNumber) const;
    void waitForEnter() const;

private:
    StationManager& stationManager_;
    MetroGraph& graph_;
    PathFinder& pathFinder_;
};
