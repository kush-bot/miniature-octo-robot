#ifndef CACHE_H
#define CACHE_H
#include <unordered_map>
#include <string>
#include "observer.h"
#include <memory>
#include "config.h"
#include<chrono>


template <typename k, typename v>
class Cache
{
public:
    using Callback = typename Observer<k, v>::Callback;
    static Cache& Instance(const CacheConfig &cfg = Config::Default()){
        static Cache cache(cfg);
        return cache;
    }

    void subscribe(Callback cb){
        obs.subscribe(cb);
    }

    void put(const k &key, const v &value){

        if (storage.size() < config.maxSize){
            storage[key] = CacheEntry{std::make_shared<v>(value),std::chrono::steady_clock::now()};
            obs.notify(key, value);
        }else{
            //do evict (lru ,etc)
        }
    }

    std::weak_ptr<v> get(const k &key){
        auto it = storage.find(key);
        if(it!=storage.end()){
            auto now = std::chrono::steady_clock::now();
            auto time_took = std::chrono::duration_cast<std::chrono::seconds>(now - it->second.createdAt);
            if(time_took < config.ttl){
                return it->second.value;
            }else{
                storage.erase(it);
            }
        }

        return {};
        
        
    }

private:
    
    Observer<k, v> obs;
    CacheConfig config;

    struct CacheEntry{
        std::shared_ptr<v> value;
        std::chrono::steady_clock::time_point createdAt;
    };
    
    std::unordered_map<k,CacheEntry> storage;
    explicit Cache(const CacheConfig& cfg):config(cfg){}
};

#endif