#include "menu.h"
#include <iostream>
#include <limits>

MenuSystem::MenuSystem(StationManager& stationManager, MetroGraph& graph, PathFinder& pathFinder)
    : stationManager_(stationManager), graph_(graph), pathFinder_(pathFinder)
{
}

void MenuSystem::showMainMenu() const
{
    std::cout << "\n===== 地铁路径规划系统 =====\n";
    std::cout << "1. 线路站点信息/运营状态管理\n";
    std::cout << "2. 所需时间最短路径规划\n";
    std::cout << "3. 所需换乘次数最少路径规划\n";
    std::cout << "4. 退出系统\n";
    std::cout << "请输入选项编号：";
}

void MenuSystem::showStationMenu() const
{
    std::cout << "\n-- 线路站点信息/运营状态管理 --\n";
    std::cout << "1. 从 CSV 文件批量更新站点开启/关闭状态\n";
    std::cout << "2. 手工更新站点开启/关闭状态\n";
    std::cout << "3. 显示当前关闭站点\n";
    std::cout << "4. 恢复所有站点初始状态\n";
    std::cout << "5. 显示线路站点信息\n";
    std::cout << "6. 受关闭站点影响站点分析\n";
    std::cout << "7. 站点查询\n";
    std::cout << "8. 返回上级菜单\n";
    std::cout << "请输入选项编号：";
}

void MenuSystem::showTimePathMenu() const
{
    std::cout << "\n-- 所需时间最短路径规划 --\n";
    std::cout << "1. 单条所需时间最短路径\n";
    std::cout << "2. 3条所需时间最短路径\n";
    std::cout << "3. 返回上级菜单\n";
    std::cout << "请输入选项编号：";
}

void MenuSystem::showTransferPathMenu() const
{
    std::cout << "\n-- 所需换乘次数最少路径规划 --\n";
    std::cout << "1. 单条所需换乘次数最少路径\n";
    std::cout << "2. 3条所需换乘次数最少路径\n";
    std::cout << "3. 返回上级菜单\n";
    std::cout << "请输入选项编号：";
}

void MenuSystem::run()
{
    int choice;

    while (true) {
        showMainMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入。\n";
            continue;
        }

        switch (choice) {
        case 1:
            handleStationMenu();
            break;
        case 2:
            handleTimePathMenu();
            break;
        case 3:
            handleTransferPathMenu();
            break;
        case 4:
            std::cout << "系统已退出。\n";
            return;
        default:
            std::cout << "选项无效，请重新输入。\n";
            break;
        }
    }
}

void MenuSystem::handleStationMenu()
{
    int choice;

    while (true) {
        showStationMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入。\n";
            continue;
        }

        switch (choice) {
        case 1:
            batchUpdateStationStatus();
            break;
        case 2:
            manualUpdateStationStatus();
            break;
        case 3:
            showClosedStations();
            break;
        case 4:
            resetStationStatus();
            break;
        case 5:
            showLineInfo();
            break;
        case 6:
            analyzeAffectedStations();
            break;
        case 7:
            searchStationInfo();
            break;
        case 8:
            return;
        default:
            std::cout << "选项无效，请重新输入。\n";
            break;
        }
    }
}

void MenuSystem::handleTimePathMenu()
{
    int choice;

    while (true) {
        showTimePathMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入。\n";
            continue;
        }

        switch (choice) {
        case 1:
            queryShortestTimePath();
            break;
        case 2:
            queryKShortestTimePaths();
            break;
        case 3:
            return;
        default:
            std::cout << "选项无效，请重新输入。\n";
            break;
        }
    }
}

void MenuSystem::handleTransferPathMenu()
{
    int choice;

    while (true) {
        showTransferPathMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "输入无效，请重新输入。\n";
            continue;
        }

        switch (choice) {
        case 1:
            queryMinTransferPath();
            break;
        case 2:
            queryKMinTransferPaths();
            break;
        case 3:
            return;
        default:
            std::cout << "选项无效，请重新输入。\n";
            break;
        }
    }
}

void MenuSystem::batchUpdateStationStatus()
{
    bool ok = stationManager_.batchUpdateStatusFromCsv(resolveDataPath("update_station_status.csv"));
    if (ok) {
        ok = stationManager_.saveToCsv(resolveDataPath("Station.csv"));
    }

    if (ok) {
        std::cout << "\n批量更新成功。\n";
    }
    else {
        std::cout << "\n批量更新失败。\n";
    }
    waitForEnter();
}

void MenuSystem::manualUpdateStationStatus()
{
    int id;
    int statusChoice;

    std::cout << "\n请输入站点 id：";
    std::cin >> id;
    std::cout << "请输入状态（1-开启，0-关闭）：";
    std::cin >> statusChoice;

    bool ok = stationManager_.setStationOpen(id, statusChoice == 1);
    if (ok) {
        stationManager_.saveToCsv(resolveDataPath("Station.csv"));
        std::cout << "站点状态更新成功。\n";
    }
    else {
        std::cout << "指定站点未找到。\n";
    }
    waitForEnter();
}

void MenuSystem::resetStationStatus()
{
    bool ok = stationManager_.resetFromInitCsv(resolveDataPath("Station_init.csv"), resolveDataPath("Station.csv"));
    if (ok) {
        std::cout << "\n已恢复所有站点初始状态。\n";
    }
    else {
        std::cout << "\n恢复失败。\n";
    }
    waitForEnter();
}

void MenuSystem::showClosedStations()
{
    std::vector<Station> closedStations = stationManager_.getClosedStations();

    std::cout << "\n==============================\n";
    if (closedStations.empty()) {
        std::cout << "当前无关闭站点。\n";
    }
    else {
        std::cout << "当前关闭站点如下：\n";
        for (std::size_t i = 0; i < closedStations.size(); ++i) {
            std::cout << i + 1 << ". "
                << closedStations[i].name << "（"
                << closedStations[i].line << "）\n";
        }
    }
    std::cout << "==============================\n";
    waitForEnter();
}

std::string MenuSystem::lineNumberToName(int lineNumber) const
{
    if (lineNumber >= 1 && lineNumber <= 18) {
        return std::to_string(lineNumber) + "号线";
    }
    if (lineNumber == 19) {
        return "浦江线";
    }
    if (lineNumber == 20) {
        return "市域机场线";
    }
    return "";
}

void MenuSystem::showLineInfo()
{
    std::cout << "\n线路编号列表（1 - 20）\n";
    std::cout << "1 - 1号线\t2 - 2号线\n";
    std::cout << "3 - 3号线\t4 - 4号线\n";
    std::cout << "5 - 5号线\t6 - 6号线\n";
    std::cout << "7 - 7号线\t8 - 8号线\n";
    std::cout << "9 - 9号线\t10 - 10号线\n";
    std::cout << "11 - 11号线\t12 - 12号线\n";
    std::cout << "13 - 13号线\t14 - 14号线\n";
    std::cout << "15 - 15号线\t16 - 16号线\n";
    std::cout << "17 - 17号线\t18 - 18号线\n";
    std::cout << "19 - 浦江线\t20 - 市域机场线\n";

    int lineNumber;
    std::cout << "\n请输入要查询的线路编号（1 - 20）：";
    std::cin >> lineNumber;

    std::string lineName = lineNumberToName(lineNumber);
    if (lineName.empty()) {
        std::cout << "线路编号无效。\n";
        waitForEnter();
        return;
    }

    std::vector<Station> stations = stationManager_.getStationsByLine(lineName);
    if (stations.empty()) {
        std::cout << "未找到该线路站点信息。\n";
        waitForEnter();
        return;
    }

    std::cout << lineName << " 共有 " << stations.size() << " 个站点\n\n";

    const std::vector<Station>& allStations = stationManager_.getAllStations();
    for (std::size_t i = 0; i < stations.size(); ++i) {
        std::vector<std::string> transferLines;
        for (const auto& station : allStations) {
            if (station.name == stations[i].name && station.line != lineName) {
                bool exists = false;
                for (const auto& transferLine : transferLines) {
                    if (transferLine == station.line) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    transferLines.push_back(station.line);
                }
            }
        }

        std::cout << "○ " << stations[i].name;
        if (!transferLines.empty()) {
            std::cout << "（换乘：";
            for (std::size_t j = 0; j < transferLines.size(); ++j) {
                if (j > 0) {
                    std::cout << "，";
                }
                std::cout << transferLines[j];
            }
            std::cout << "）";
        }
        if (stations[i].status == StationStatus::Closed) {
            std::cout << "  关闭";
        }
        std::cout << "\n";
        if (i + 1 < stations.size()) {
            std::cout << "|\n";
        }
    }

    waitForEnter();
}

void MenuSystem::analyzeAffectedStations()
{
    int stationId = selectStationByKeyword("请输入要分析的站点名称关键字：");
    if (stationId == -1) {
        std::cout << "未选择有效站点。\n";
        waitForEnter();
        return;
    }

    std::cout << "\n==============================\n";
    const Station* station = stationManager_.getStationById(stationId);
    if (station != nullptr) {
        std::cout << "分析站点：" << station->name << "（" << station->line << "，按关闭处理）\n\n";
    }

    std::vector<int> affectedStationIds = pathFinder_.analyzeAffectedArea({ stationId });
    if (affectedStationIds.empty()) {
        std::cout << "未发现直接受影响站点。\n";
    }
    else {
        std::cout << "直接受影响站点共 " << affectedStationIds.size() << " 个：\n";
        for (std::size_t i = 0; i < affectedStationIds.size(); ++i) {
            const Station* affectedStation = stationManager_.getStationById(affectedStationIds[i]);
            if (affectedStation == nullptr) {
                continue;
            }

            std::cout << i + 1 << ". "
                << affectedStation->name << "（"
                << affectedStation->line << "，"
                << (affectedStation->status == StationStatus::Open ? "开启" : "关闭")
                << "）\n";
        }
    }
    std::cout << "==============================\n";

    waitForEnter();
}

void MenuSystem::searchStationInfo()
{
    int stationId = selectStationByKeyword("请输入站点名称：");
    if (stationId == -1) {
        std::cout << "未选择有效站点。\n";
        waitForEnter();
        return;
    }

    const Station* station = stationManager_.getStationById(stationId);
    if (station == nullptr) {
        std::cout << "未找到站点信息。\n";
        waitForEnter();
        return;
    }

    std::cout << "\n匹配结果：\n";
    std::cout << "站点ID：" << station->id << "\n";
    std::cout << "站点名称：" << station->name << "\n";
    std::cout << "所属线路：" << station->line << "\n";
    std::cout << "当前状态："
        << (station->status == StationStatus::Open ? "开启" : "关闭") << "\n";

    waitForEnter();
}

int MenuSystem::selectStationByKeyword(const std::string& prompt) const
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string keyword;
    std::cout << prompt;
    std::getline(std::cin, keyword);

    std::vector<int> ids = stationManager_.searchStationByName(keyword);
    if (ids.empty()) {
        std::cout << "没有匹配到站点。\n";
        return -1;
    }

    std::cout << "匹配的站点如下：\n";
    for (std::size_t i = 0; i < ids.size(); ++i) {
        const Station* station = stationManager_.getStationById(ids[i]);
        if (station != nullptr) {
            std::cout << i + 1 << ". "
                << station->name << "（"
                << station->line << "）\n";
        }
    }

    int choice;
    std::cout << "请输入对应编号选择站点：";
    std::cin >> choice;

    if (choice < 1 || (std::size_t)choice > ids.size()) {
        return -1;
    }
    return ids[choice - 1];
}

bool MenuSystem::readPathEndpoints(int& startId, int& endId) const
{
    startId = selectStationByKeyword("请输入起点站关键字：");
    if (startId == -1) {
        std::cout << "起点选择失败。\n";
        return false;
    }

    endId = selectStationByKeyword("请输入终点站关键字：");
    if (endId == -1) {
        std::cout << "终点选择失败。\n";
        return false;
    }

    return validatePathEndpoints(startId, endId);
}

bool MenuSystem::validatePathEndpoints(int startId, int endId) const
{
    const Station* start = stationManager_.getStationById(startId);
    const Station* end = stationManager_.getStationById(endId);
    if (start == nullptr || end == nullptr) {
        std::cout << "未找到可行路径。\n";
        return false;
    }

    if (start->status == StationStatus::Closed) {
        printClosedEndpointMessage("起点", startId);
        return false;
    }

    if (end->status == StationStatus::Closed) {
        printClosedEndpointMessage("终点", endId);
        return false;
    }

    if (isSamePhysicalStation(startId, endId)) {
        std::cout << "起点和终点相同，无需进行路径规划。\n";
        return false;
    }

    return true;
}

void MenuSystem::printClosedEndpointMessage(const std::string& endpointName, int stationId) const
{
    const Station* station = stationManager_.getStationById(stationId);
    if (station == nullptr) {
        std::cout << endpointName << "站点不存在，无法进行路径规划。\n";
        return;
    }

    std::cout << endpointName << "：" << station->name << "（" << station->line
        << "）已关闭，无法进行路径规划。\n";
}

bool MenuSystem::isSamePhysicalStation(int leftId, int rightId) const
{
    const Station* left = stationManager_.getStationById(leftId);
    const Station* right = stationManager_.getStationById(rightId);
    return left != nullptr && right != nullptr && left->name == right->name;
}

void MenuSystem::queryShortestTimePath()
{
    int startId;
    int endId;
    if (!readPathEndpoints(startId, endId)) {
        waitForEnter();
        return;
    }

    PathResult result = pathFinder_.shortestTimePath(startId, endId);
    printPathResult(result);
    waitForEnter();
}

void MenuSystem::queryKShortestTimePaths()
{
    int startId;
    int endId;
    if (!readPathEndpoints(startId, endId)) {
        waitForEnter();
        return;
    }

    std::vector<PathResult> results = pathFinder_.kShortestTimePaths(startId, endId, 3);
    if (results.empty()) {
        std::cout << "未找到可行路径。\n";
    }
    else {
        for (std::size_t i = 0; i < results.size(); ++i) {
            std::cout << "\n第 " << i + 1 << " 条路径：\n";
            printPathResult(results[i]);
        }
    }
    waitForEnter();
}

void MenuSystem::queryMinTransferPath()
{
    int startId;
    int endId;
    if (!readPathEndpoints(startId, endId)) {
        waitForEnter();
        return;
    }

    PathResult result = pathFinder_.minTransferPath(startId, endId);
    printPathResult(result);
    waitForEnter();
}

void MenuSystem::queryKMinTransferPaths()
{
    int startId;
    int endId;
    if (!readPathEndpoints(startId, endId)) {
        waitForEnter();
        return;
    }

    std::vector<PathResult> results = pathFinder_.kMinTransferPaths(startId, endId, 3);
    if (results.empty()) {
        std::cout << "未找到可行路径。\n";
    }
    else {
        for (std::size_t i = 0; i < results.size(); ++i) {
            std::cout << "\n第 " << i + 1 << " 条路径：\n";
            printPathResult(results[i]);
        }
    }
    waitForEnter();
}

void MenuSystem::printPathResult(const PathResult& result) const
{
    if (!result.valid) {
        std::cout << "未找到可行路径。\n";
        return;
    }

    std::cout << "\n====================================\n";
    std::cout << "预计车程：" << result.totalTime << " 分钟\n";
    std::cout << "途经（含起点）共 " << result.passedStationCount << " 站\n";
    std::cout << "换乘次数：" << result.transferCount << " 次\n";

    if (!result.lines.empty()) {
        std::cout << "路径线路：";
        for (std::size_t i = 0; i < result.lines.size(); ++i) {
            std::cout << result.lines[i];
            if (i != result.lines.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << "\n";
    }

    if (!result.stationNames.empty()) {
        std::cout << "站点路径：";
        for (std::size_t i = 0; i < result.stationNames.size(); ++i) {
            std::cout << result.stationNames[i];
            if (i != result.stationNames.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << "\n";
    }

    if (!result.transferStations.empty()) {
        std::cout << "换乘站：";
        for (std::size_t i = 0; i < result.transferStations.size(); ++i) {
            std::cout << result.transferStations[i];
            if (i != result.transferStations.size() - 1) {
                std::cout << "、";
            }
        }
        std::cout << "\n";
    }

    std::cout << "====================================\n";
}

void MenuSystem::waitForEnter() const
{
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
