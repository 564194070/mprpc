#pragma once

#include<unordered_map>
#include<string>

//框架读取配置文件夹
//rpcserviceip   rpcserverport  zookeeperip zookeeperport
class MprpcConfig
{
public:
    //负责解析加载配置文件
    void LoadConfigFile(const char* config_file);
    //查询配置项信息
    std::string Load(std::string key);
    void Trim(std::string& buf);
private:
    std::unordered_map<std::string,std::string> m_configMap;
};
