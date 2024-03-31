#pragma once

namespace atpdxy {

// 无法拷贝基类
class Noncopyable {
public:
    Noncopyable() = default;

    ~Noncopyable() = default;

    // 禁用拷贝构造函数
    Noncopyable(const Noncopyable&) = delete;

    // 禁用赋值构造函数
    Noncopyable& operator=(const Noncopyable&) = delete;

    Noncopyable( Noncopyable&&) = delete;
    Noncopyable& operator=(Noncopyable&&) = delete;
};

}
