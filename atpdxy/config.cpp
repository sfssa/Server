#include "config.h"

namespace atpdxy
{
ConfigBase::ptr ConfigManager::LookupBase(const std::string& name)
{
    RWMutexType::ReadLock lock(GetMutex());
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

// 将所有属性值存储到list中，格式转换如下：
// A: 
//     B    ---->       A.B
static void ListAllMember(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node>>& outputs)
{
    // 非法
    if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
    {
        LOG_ERROR(LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    outputs.push_back(std::make_pair(prefix, node));
    if(node.IsMap())
    {
        for(auto it = node.begin(); it != node.end(); ++it)
        {
            ListAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, outputs);
        }
    }
}

void ConfigManager::LoadFromYaml(const YAML::Node& root)
{
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    ListAllMember("", root, all_nodes);
    for(auto& i : all_nodes)
    {
        std::string key = i.first;
        if(key.empty())
        {
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigBase::ptr var = LookupBase(key);
        if(var)
        {
            if(i.second.IsScalar())
            {
                var->fromString(i.second.Scalar());
            }
            else
            {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

void ConfigManager::Visit(std::function<void(ConfigBase::ptr)> cb)
{
    RWMutexType::ReadLock lock(GetMutex());
    ConfigMap& m = GetDatas();
    for(auto it = m.begin(); it != m.end(); ++it)
    {
        cb(it->second);
    }
}

}