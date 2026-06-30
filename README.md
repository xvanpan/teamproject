# teamproject

地铁路径规划系统，基于 CSV 数据加载站点与线路边信息，提供站点状态管理、线路查询、最短时间路径规划、最少换乘路径规划和关闭站点影响分析。

## 功能

- 从 `data/Station.csv` 加载站点信息。
- 从 `data/Edge.csv` 加载线路连接关系。
- 批量或手工更新站点开启/关闭状态。
- 显示当前关闭站点，并可从 `data/Station_init.csv` 恢复初始状态。
- 按线路显示站点及换乘信息。
- 查询单条或 3 条所需时间最短路径。
- 查询单条或 3 条换乘次数最少路径。
- 分析指定站点关闭后的直接受影响站点。

## 构建与运行

使用 Visual Studio 2022 打开 `teamproject/teamproject.sln`，选择 `Debug|x64` 或 `Release|x64` 后生成并运行。

也可以使用 MSBuild：

```powershell
MSBuild.exe .\teamproject\teamproject.sln /p:Configuration=Debug /p:Platform=x64
.\teamproject\x64\Debug\teamproject.exe
```

## 数据文件

- `data/Station.csv`：当前站点数据，包含 `id,name,line,status`。
- `data/Station_init.csv`：初始站点状态，用于恢复。
- `data/Edge.csv`：线路边数据，包含 `from,to,line,direction,time`。
- `data/update_station_status.csv`：批量更新站点状态的数据。

程序会自动从当前目录及上级目录查找 `data` 文件夹。
