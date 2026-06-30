#include "station.h"
#include "graph.h"
#include "pathfinder.h"
#include "menu.h"
#include <direct.h>
#include <iostream>

int main() {
    char cwd[FILENAME_MAX];
    if (_getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "当前工作目录: " << cwd << std::endl;
    }

    const std::string stationFile = resolveDataPath("Station.csv");
    const std::string edgeFile = resolveDataPath("Edge.csv");

    StationManager stationManager;
    if (!stationManager.loadFromCsv(stationFile)) {
        std::cerr << "站点数据加载失败，请检查 data/Station.csv 是否存在。\n";
        return 1;
    }

    MetroGraph graph;
    if (!graph.buildFromCsv(edgeFile, stationManager)) {
        std::cerr << "边数据加载失败，请检查 data/Edge.csv 是否存在。\n";
        return 1;
    }

    PathFinder pathFinder(graph, stationManager);

    MenuSystem menu(stationManager, graph, pathFinder);
    menu.run();

    return 0;
}
