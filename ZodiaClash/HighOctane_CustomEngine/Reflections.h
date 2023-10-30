#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

class TypeInformation {
public:
    virtual ~TypeInformation() = default;
    virtual void Print() const = 0;
};

template<typename T>
class TypeInfo : public TypeInformation {
public:
    void Print() const override {
        std::cout << "Type: " << typeid(T).name() << std::endl;
    }
};

class TypeRegistry {
private:
    std::unordered_map<std::string, std::unique_ptr<TypeInformation>> types;

public:
    template<typename T>
    void RegisterType(const std::string& typeName) {
        types[typeName] = std::make_unique<TypeInfo<T>>();
    }

    const TypeInformation* GetType(const std::string& typeName) const {
        auto it = types.find(typeName);
        if (it != types.end()) {
            return it->second.get();
        }
        return nullptr;
    }
};
//void testFunc();