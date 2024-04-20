//
// Created by Jacob Edwards on 17/04/2024.
//

#ifndef SINGLETON_H
#define SINGLETON_H

#include <memory>
#include <mutex>

template<typename T>
class Singleton {
public:
    static auto GetInstance() -> T & {
        std::call_once(onceFlag, [] {
            instance = std::make_unique<T>(Token{});
        });
        return *instance;
    }

    Singleton(const Singleton &) = delete;

    Singleton(Singleton &&) = delete;

    auto operator=(const Singleton &) -> Singleton & = delete;

    auto operator=(Singleton &&) -> Singleton & = delete;

protected:
    Singleton() = default;

    virtual ~Singleton() = default;

    struct Token {
    };

private:
    static std::unique_ptr<T> instance;
    static std::once_flag onceFlag;
};

template<typename T>
std::unique_ptr<T> Singleton<T>::instance = nullptr;

template<typename T>
std::once_flag Singleton<T>::onceFlag;

#endif //SINGLETON_H
