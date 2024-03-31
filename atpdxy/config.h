#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "thread.h"
#include "log.h"
#include "util.h"

namespace atpdxy {

// 配置变量的基类，包含配置名和配置描述
class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    
    // 构造函数
    ConfigVarBase(const std::string& name, const std::string& description = ""):
        m_name(name),
        m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    // 析构函数
    virtual ~ConfigVarBase() {}

    // 返回配置名称
    const std::string& getName() const { return m_name;}

    // 返回配置的描述
    const std::string& getDescription() const { return m_description;}

    // 将配置转换成字符串
    virtual std::string toString() = 0;

    // 将字符串转换成配置
    virtual bool fromString(const std::string& val) = 0;

    // 返回配置参数值的类型
    virtual std::string getTypeName() const = 0;
protected:
    // 配置参数的名称
    std::string m_name;
    // 配置参数的描述
    std::string m_description;
};

// 类型转换模板类，从F类型转换成T类型
template<class F, class T>
class LexicalCast {
public:
    // 重载()
    T operator()(const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

// 类型转换模板类片特化从yaml的string类型转换成vector<T>类型
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 类型转换模板类片特化从std::vector<T>类型转换成 yaml的string类型
template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 类型转换模板类片特化从yaml的string类型转换成std::list<T>
template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 类型转换模板类片特化从std::list<T>类型转换成yaml的string类型
template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 类型转换模板类片特化yaml的string类型转换成std::set<T>类型
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 类型转换模板类片特化从std::set<T>类型转换成yaml的string类型
template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 类型转换模板类片特化从yaml的string类型转换成std::unordered_set<T>类型
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for(size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

// 类型转换模板类片特化从std::unordered_set<T>类型转换成yaml的string类型
template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for(auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 类型转换模板类片特化从yaml的string类型转换成std::map<std::string, T>类型
template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 类型转换模板类片特化std::map<std::string, T>类型转换成yaml的string类型
template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 类型转换模板类片特化从yaml的string类型转换成std::unordered_map<std::string, T>类型
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream ss;
        for(auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

// 类型转换模板类片特化std::unordered_map<std::string, T>类型转换成yaml的string类型
template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for(auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// 配置子类，增添配置的值
// FromStr:从std::string转换成T类型的仿函数
// ToStr:从T转换成std::string的仿函数
template<class T, class FromStr = LexicalCast<std::string, T>
                , class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase {
public:
    typedef RWMutex RWMutexType;
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

    // 构造函数
    ConfigVar(const std::string& name ,const T& default_value ,const std::string& description = ""):
        ConfigVarBase(name, description),
        m_val(default_value) {
    }

    // 将参数转换成yaml的string类型
    std::string toString() override {
        try {
            RWMutexType::ReadLock lock(m_mutex);
            return ToStr()(m_val);
        } catch (std::exception& e) {
            ERROR(GET_ROOT_LOGGER()) << "ConfigVar::toString exception "
                << e.what() << " convert: " << TypeToName<T>() << " to string"
                << " name=" << m_name;
        }
        return "";
    }

    // 从yaml的string类型提取出配置
    bool fromString(const std::string& val) override {
        try {
            setValue(FromStr()(val));
        } catch (std::exception& e) {
            ERROR(GET_ROOT_LOGGER()) << "ConfigVar::fromString exception "
                << e.what() << " convert: string to " << TypeToName<T>()
                << " name=" << m_name
                << " - " << val;
        }
        return false;
    }

    // 返回当前配置的值
    const T getValue() {
        RWMutexType::ReadLock lock(m_mutex);
        return m_val;
    }

    // 设置当前配置的值
    void setValue(const T& v) {
        {
            RWMutexType::ReadLock lock(m_mutex);
            if(v == m_val) {
                return;
            }
            for(auto& i : m_cbs) {
                i.second(m_val, v);
            }
        }
        RWMutexType::WriteLock lock(m_mutex);
        m_val = v;
    }

    // 返回配置参数值的类型
    std::string getTypeName() const override { return TypeToName<T>();}

    // 添加变化回调函数
    uint64_t addListener(on_change_cb cb) {
        static uint64_t s_fun_id = 0;
        RWMutexType::WriteLock lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = cb;
        return s_fun_id;
    }

    // 删除回调函数
    void delListener(uint64_t key) {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.erase(key);
    }

    // 获取对应的回调函数
    on_change_cb getListener(uint64_t key) {
        RWMutexType::ReadLock lock(m_mutex);
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    // 清理所有的回调函数
    void clearListener() {
        RWMutexType::WriteLock lock(m_mutex);
        m_cbs.clear();
    }
private:
    // 互斥锁
    RWMutexType m_mutex;
    // 配置的值
    T m_val;
    // 变更回调函数组, uint64_t key,要求唯一，一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

// 配置管理器
class Config {
public:
    typedef std::unordered_map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef RWMutex RWMutexType;

    // 获取/创建配置
    // 有对应配置则返回；没有对应配置则创建并设置默认值
    // 参数名存在而类型不匹配则返回nullptr
    // 如果参数名包含非法字符[^0-9a-z_.] 抛出异常 std::invalid_argument
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
        const T& default_value, const std::string& description = "") {
        RWMutexType::WriteLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
            if(tmp) {
                INFO(GET_ROOT_LOGGER()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {
                ERROR(GET_ROOT_LOGGER()) << "Lookup name=" << name << " exists but type not "
                        << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                        << " " << it->second->toString();
                return nullptr;
            }
        }

        if(name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678")
                != std::string::npos) {
            ERROR(GET_ROOT_LOGGER()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = v;
        return v;
    }

    // 返回名为name的配置参数
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        RWMutexType::ReadLock lock(GetMutex());
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
    }

    // 从yaml文件读取内容并初始化配置
    static void LoadFromYaml(const YAML::Node& root);

    // 滴滴滴滴滴滴滴滴
    // 加载path文件夹里面的配置文件
    // static void LoadFromConfDir(const std::string& path, bool force = false);

    // 查找配置参数，返回基类智能指针
    static ConfigVarBase::ptr LookupBase(const std::string& name);

    // 遍历配置模块里面所有配置项，可以纠错
    static void Visit(std::function<void(ConfigVarBase::ptr)> cb);
private:
    // 返回所有配置项
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    // 配置项的锁，以静态函数是为了避免初始化顺序未知的问题
    static RWMutexType& GetMutex() {
        static RWMutexType s_mutex;
        return s_mutex;
    }
};

}
