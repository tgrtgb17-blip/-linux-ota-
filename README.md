# Linux OTA 升级系统

一个基于 MQTT 的嵌入式 Linux OTA 升级方案，包含服务端、客户端应用和 Bootloader。

## 功能

- 设备通过 MQTT 连接服务端
- 服务端管理设备、下发升级任务
- 客户端接收升级指令并执行
- 支持 Bootloader 引导升级

## 项目结构

- `ota_server/`：Python 服务端，负责设备管理、任务下发、MQTT 通信
- `ota_app/`：OTA 客户端应用（Keil 工程）
- `ota_b/`：OTA Bootloader（Keil 工程）
### 服务端

cd ota_server
python main.py

### 需要修改的地方

-请把mqtt_client.py上的broker修改为自己的服务器地址（不会问deepseek）
-请修改它devices.json和task.json（分别表示现有的设备，想要更新的设备）请按照格式修改

注：如果想要使用请确保以安装emqx，并且开启了emqx服务器（./bin/emqx start）
