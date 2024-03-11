#pragma once

#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <ctype.h>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <functional>
#include "log.h"
// 配置系统原则：约定优于配置（创建的优于从文件读取的）
namespace atpdxy
{
class ConfigBase
{
public:
    // 智能指针
    typedef std::shared_ptr<ConfigBase> ptr;

    // 构造函数
    ConfigBase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description)
    {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    // 将析构声明为虚函数用以释放内存
    virtual ~ConfigBase() {}

    // 返回私有数据成员
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    // 将配置转换成字符串
    virtual std::string toString() = 0;

    // 从字符串中解析配置
    virtual bool fromString(const std::string& val) = 0;

    // 返回配置值的属性
    virtual std::string getTypeName() const = 0;
protected:
    // 配置名
    std::string m_name;

    // 配置的描述
    std::string m_description;
};

// 针对基础类型的模板偏特化，将F类型转换成T类型
template <class F, class T>
class LexicalCast
{
public:
    T operator()(const F& v) const
    {
        return boost::lexical_cast<T>(v);
    }
};

// 针对vector从string到vector<T>偏特化
template <class T>
class LexicalCast <std::string, std::vector<T>>
{
public:
    std::vector<T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 针对vector从vector<T>到string偏特化
template <class T>
class LexicalCast <std::vector<T>, std::string>
{
public:
    std::string operator() (const std::vector<T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 针对list从string到list<T>偏特化
template <class T>
class LexicalCast <std::string, std::list<T>>
{
public:
    std::list<T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 针对list从list<T>到string偏特化
template <class T>
class LexicalCast <std::list<T>, std::string>
{
public:
    std::string operator() (const std::list<T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 针对set从string到set<T>偏特化
template <class T>
class LexicalCast <std::string, std::set<T>>
{
public:
    std::set<T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 针对set从set<T>到string偏特化
template <class T>
class LexicalCast <std::set<T>, std::string>
{
public:
    std::string operator() (const std::set<T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 针对unordered_set从string到unordered_set<T>偏特化
template <class T>
class LexicalCast <std::string, std::unordered_set<T>>
{
public:
    std::unordered_set<T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i){
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 针对unordered_set从unordered_set<T>到string偏特化
template <class T>
class LexicalCast <std::unordered_set<T>, std::string>
{
public:
    std::string operator() (const std::unordered_set<T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 针对map从string到map<std::string, T>偏特化
template <class T>
class LexicalCast <std::string, std::map<std::string, T>>
{
public:
    std::map<std::string, T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 针对map从map<std::string, T>到string偏特化
template <class T>
class LexicalCast <std::map<std::string, T>, std::string>
{
public:
    std::string operator() (const std::map<std::string, T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 针对unordered_map从string到unordered_map<std::string, T>偏特化
template <class T>
class LexicalCast <std::string, std::unordered_map<std::string, T>>
{
public:
    std::unordered_map<std::string, T> operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin(); it != node.end(); ++it){
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 针对map从map<std::string, T>到string偏特化
template <class T>
class LexicalCast <std::unordered_map<std::string, T>, std::string>
{
public:
    std::string operator() (const std::unordered_map<std::string, T>& v)
    {
        YAML::Node node;
        for(auto& i : v)
        {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 配置实现类
template <class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class Config : public ConfigBase
{
public:
    // 配置类的智能指针
    typedef std::shared_ptr<Config> ptr;

    // 配置变更回调函数
    typedef std::function<void (const T& old_value, const T& new_value)> onConfigChanged;

    // 构造函数
    Config(const std::string& name, const T& default_value, const std::string& description = "")
        :ConfigBase(name, description)
        ,m_val(default_value)
    {

    }

    // 将m_val从T转换成字符串
    std::string toString() override
    {
        try
        {   
            // return boost::lexical_cast<std::string>(m_val);
            // 创建临时对象，并调用临时对象的重载()
            return ToStr()(m_val);
        }
        catch(std::exception& e)
        {
            LOG_ERROR(LOG_ROOT()) << "Config::toString exception "
                << e.what() << " convert: " << typeid(m_val).name() << " to string.";
        }
        return "";
    }

    // 将m_val的值进行修改
    bool fromString(const std::string& val) override
    {
        try
        {   
            // m_val = boost::lexical_cast<T>(val);
            // 创建临时对象，并调用临时对象的重载()
            setValue(FromStr()(val));
            return true;
        }
        catch(std::exception& e)
        {
            LOG_ERROR(LOG_ROOT()) << "Config::fromString exception "
                << e.what() << " convert string to: " << typeid(m_val).name()
                << " - " << val;
        }
        return false;
    }

    // 设置和获得配置值的函数
    const T getValue() const { return m_val; }

    void setValue(const T& val) 
    { 
        // 原值和新值有没有变化
        if(val == m_val)
        {
            return;
        }
        for(auto& i : m_cbs)
        {
            i.second(m_val, val);
        }
        m_val = val;
    } 

    // 获得配置的类型
    std::string getTypeName() const override { return typeid(T).name(); }

    void addListener(uint64_t key, onConfigChanged cb)
    {   
        m_cbs[key] = cb;
    }

    void delListener(uint64_t key)
    {
        m_cbs.erase(key);
    }

    onConfigChanged getListener(uint64_t key)
    {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener()
    {
        m_cbs.clear();
    }
private:
    // 配置的值
    T m_val;

    // 回调变更函数组，key要求唯一，一般用哈希值
    std::map<uint64_t, onConfigChanged> m_cbs;
};

// 配置管理类
class ConfigManager
{
public:
    // 
    typedef std::map<std::string, ConfigBase::ptr> ConfigMap;

    // 从静态成员中查找配置，有则返回，没有则创建
    template <class T>
    static typename Config<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description = "")
    {
        auto it = GetDatas().find(name);
        if(it != GetDatas().end())
        {
            auto tmp = std::dynamic_pointer_cast<Config<T>>(it->second);
            if(tmp)
            {
                LOG_INFO(LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            }
            else
            {
                LOG_WARN(LOG_ROOT()) << "Lookup name = " 
                    << name << " exists but type not " << typeid(T).name()
                    << " real_type = " << it->second->getTypeName()
                    << " " << it->second->toString();
                return nullptr;
            }
        }

        // 配置项命名不合法
        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._012345678") != std::string::npos)
        {
            LOG_ERROR(LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        // 新建一个配置项并存入静态map中
        typename Config<T>::ptr v(new Config<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }

    // 查询静态map中是否有配置，返回子类指针
    template <class T>
    static typename Config<T>::ptr Lookup(const std::string& name)
    {
        auto it = GetDatas().find(name);
        if(it == GetDatas().end())
        {
            return nullptr;
        }
        return std::dynamic_pointer_cast<Config<T>> (it->second);
    }

    // 从YAML文件导入配置
    static void LoadFromYaml(const YAML::Node& root);

    // 查询静态map中是否有配置项，返回基类指针
    static ConfigBase::ptr LookupBase(const std::string& name);
private:
    // 配置名-配置的智能指针
    static ConfigMap& GetDatas()
    {
        static ConfigMap s_datas;
        return s_datas;
    }
};
}