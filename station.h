#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>

class StationManager {
public:
    bool loadFromCsv(const std::string& fileName);//从文件读取信息到stations_
    bool saveToCsv(const std::string& fileName) const;//保存修改后的station

    bool resetFromInitCsv(const std::string& initFile, const std::string& targetFile);//从Station_init.csv恢复站点信息
    bool batchUpdateStatusFromCsv(const std::string& updateFile);//读取update_station_status.csv更新站点信息

    const Station* getStationById(int id) const;
    Station* getStationById(int id);//通过id返回station

    bool setStationStatus(int id, StationStatus status);//调整站点状态
    bool setStationOpen(int id, bool isOpen);//实际对外修改站点状态接口
    bool isStationOpen(int id) const;//判断station状态

    std::vector<Station> getClosedStations() const;//获得关闭站点列表
    std::vector<Station> getStationsByLine(const std::string& line) const;//通过line获得station
    std::vector<int> searchStationByName(const std::string& name) const;//获得同名站点所有的id用于遍历换乘

    const std::vector<Station>& getAllStations() const;//公共调用接口

private:
    StationStatus parseStatusText(const std::string& text) const;//从开启到状态
    std::string toStatusText(StationStatus status) const;//从状态到字符串
    void rebuildIndex();//初始化idToIndex_

private:
    std::vector<Station> stations_;
    std::unordered_map<int, int> idToIndex_;
};