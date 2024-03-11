#include "../atpdxy/config.h"
#include "../atpdxy/log.h"
#include <yaml-cpp/yaml.h>
#if 1
// 创建了两个配置system.port和system.value，查询时通过fromString将m_val修改成读取到的值
atpdxy::Config<int>::ptr g_int_value_config = atpdxy::ConfigManager::Lookup("system.port", (int)8080, "system port");
atpdxy::Config<float>::ptr g_int_valuex_config = atpdxy::ConfigManager::Lookup("system.port", (float)8080, "system port");
atpdxy::Config<float>::ptr g_float_value_config = atpdxy::ConfigManager::Lookup("system.value", (float)8.8, "system value");
atpdxy::Config<std::vector<int>>::ptr g_int_vec_config = atpdxy::ConfigManager::Lookup("system.int_vec", std::vector<int>{1, 2, 3}, "system int vec");
atpdxy::Config<std::list<int>>::ptr g_int_list_config = atpdxy::ConfigManager::Lookup("system.int_list", std::list<int>{4, 5, 6}, "system int list");
atpdxy::Config<std::set<int>>::ptr g_int_set_config = atpdxy::ConfigManager::Lookup("system.int_set", std::set<int>{7, 8, 9}, "system int set");
atpdxy::Config<std::unordered_set<int>>::ptr g_int_uset_config = atpdxy::ConfigManager::Lookup("system.int_uset", std::unordered_set<int>{10, 11, 12}, "system int uset");
atpdxy::Config<std::map<std::string, int>>::ptr g_int_map_config = atpdxy::ConfigManager::Lookup("system.int_map", std::map<std::string, int>{{"三上悠亚", 1}, {"天使萌", 2}}, "system int map");
atpdxy::Config<std::unordered_map<std::string, int>>::ptr g_int_umap_config = atpdxy::ConfigManager::Lookup("system.int_umap", std::unordered_map<std::string, int>{{"桃乃木香奈", 1}, {"河北菜花", 2}}, "system int umap");

void print_yaml(const YAML::Node node, int level)
{
    if(node.IsScalar()) 
    {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
    }
    else if(node.IsNull())
    {
        LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level;
    }
    else if(node.IsMap())
    {
        for(auto it = node.begin(); it != node.end(); it++)
        {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ') << it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);
        }
    }
    else if(node.IsSequence())
    {
        for(size_t i = 0; i < node.size(); ++i)
        {
            LOG_INFO(LOG_ROOT()) << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/home/atpdxy/GitHub/Server/bin/conf/test.yml");
    print_yaml(root, 0);
    LOG_INFO(LOG_ROOT()) << root.Scalar();
}

void test_config()
{
    LOG_INFO(LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    LOG_INFO(LOG_ROOT()) << "before: " << g_float_value_config->toString();
    #define XX(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) \
        { \
            LOG_INFO(LOG_ROOT()) << #prefix " " #name ": " << i; \
        } \
        LOG_INFO(LOG_ROOT()) << #prefix " " #name ": " " yaml: " << g_var->toString(); \
    }

    #define XX_M(g_var, name, prefix) \
    { \
        auto& v = g_var->getValue(); \
        for(auto& i : v) \
        { \
            LOG_INFO(LOG_ROOT()) << #prefix " " #name ": {" << i.first << " - " << i.second << "}"; \
        } \
        LOG_INFO(LOG_ROOT()) << #prefix " " #name ": " " yaml: " << g_var->toString(); \
    }
    XX(g_int_vec_config, int_vec, before);
    XX(g_int_list_config, int_list, before);
    XX(g_int_set_config, int_set, before);
    XX(g_int_uset_config, int_uset, before);
    XX_M(g_int_map_config, int_map, before);
    XX_M(g_int_umap_config, int_umap, before);

    YAML::Node root = YAML::LoadFile("/home/atpdxy/GitHub/Server/bin/conf/test.yml");
    atpdxy::ConfigManager::LoadFromYaml(root);

    LOG_INFO(LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    LOG_INFO(LOG_ROOT()) << "after: " << g_float_value_config->toString();

    XX(g_int_vec_config, int_vec, after);
    XX(g_int_list_config, int_list, after);
    XX(g_int_set_config, int_set, after);
    XX(g_int_uset_config, int_uset, after);
    XX_M(g_int_map_config, int_map, after);
    XX_M(g_int_umap_config, int_umap, after);
}
#endif 

class Person
{
public:
    Person() {}

    std::string toString() const 
    {
        std::stringstream ss;
        ss << "[Person name=" << m_name
            << " age=" << m_age
            << " sex=" << m_sex
            << "]";
        return ss.str();
    }

    bool operator==(const Person& p) const
    {
        return m_name == p.m_name && m_age == p.m_age && m_sex == p.m_sex;
    }

    std::string m_name;
    int m_age = 0;
    bool m_sex = false;
};
namespace atpdxy
{

template <>
class LexicalCast <std::string, Person>
{
public:
    Person operator() (const std::string& v)
    {
        YAML::Node node = YAML::Load(v);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template <>
class LexicalCast <Person, std::string>
{
public:
    std::string operator() (const Person& v)
    {
        YAML::Node node;
        node["name"] = v.m_name;
        node["age"] = v.m_age;
        node["sex"] = v.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}
atpdxy::Config<Person>::ptr g_person = atpdxy::ConfigManager::Lookup("class.person", Person(), "system person");
atpdxy::Config<std::map<std::string, Person>>::ptr g_person_map = atpdxy::ConfigManager::Lookup("class.map", std::map<std::string, Person>(), "system person");
atpdxy::Config<std::map<std::string, std::vector<Person>>>::ptr g_person_map_vec = atpdxy::ConfigManager::Lookup("class.map_vec", std::map<std::string, std::vector<Person>>(), "system person");

void test_class()
{
    LOG_INFO(LOG_ROOT()) << "before" << g_person->getValue().toString() << " - " << g_person->toString();
    #define XX_PM(g_var, prefix) \
    { \
        auto m = g_person_map->getValue(); \
        for(auto& i : m) \
        { \
            LOG_INFO(LOG_ROOT()) << prefix << ": " << i.first << " - " << i.second.toString(); \
        } \
        LOG_INFO(LOG_ROOT()) << prefix << ": " << ": size=" <<m.size(); \
    }

    g_person->addListener(10, [](const Person& old_value, const Person& new_value){
        LOG_INFO(LOG_ROOT()) << "old_value: " << old_value.toString() << " new_value: " << new_value.toString();
    });

    XX_PM(g_person_map, "class.map before");
    LOG_INFO(LOG_ROOT()) << "before:" << g_person_map_vec->toString();
    YAML::Node root = YAML::LoadFile("/home/atpdxy/GitHub/Server/bin/conf/log.yml");
    atpdxy::ConfigManager::LoadFromYaml(root);
    LOG_INFO(LOG_ROOT()) << "after" << g_person->getValue().toString() << " - " << g_person->toString();
    XX_PM(g_person_map, "class.map after");
    LOG_INFO(LOG_ROOT()) << "after:" << g_person_map_vec->toString();
}

void test_log()
{
    static atpdxy::Logger::ptr system_log = LOG_NAME("system");
    LOG_INFO(system_log) << "hello system" << std::endl;
    std::cout << atpdxy::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    YAML::Node root = YAML::LoadFile("/home/atpdxy/GitHub/Server/bin/conf/logs.yml");
    atpdxy::ConfigManager::LoadFromYaml(root);
    std::cout << "======================================" << std::endl;
    std::cout << atpdxy::LoggerMgr::GetInstance()->toYamlString() << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << root << std::endl;
    LOG_INFO(system_log) << "hello system" << std::endl;

    system_log->setFormatter("%d - %m%n");
    LOG_INFO(system_log) << "hello system" << std::endl;
}

int main()
{
    test_log();
    // test_class();
    // test_config();
    // test_yaml();
    return 0;
}