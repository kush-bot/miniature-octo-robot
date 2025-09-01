#ifndef CACHE_H
#define CACHE_H
#include <unordered_map>
#include <string>
#include "observer.h"
#include <memory>
#include "config.h"
#include <chrono>
#include <list>

template <typename k, typename v>
class Cache
{
public:
    using Callback = typename Observer<k, v>::Callback;
    static Cache &Instance(const CacheConfig &cfg = Config::Default())
    {
        static Cache cache(cfg);
        cache.config = cfg;
        return cache;
    }

    void subscribe(Callback cb)
    {
        obs.subscribe(cb);
    }

    void put(const k &key, const v &value)
    {
        auto it = storage.find(key);
        if (it != storage.end())
        {
            it->second.value = std::make_shared<v>(value);
            it->second.createdAt = std::chrono::steady_clock::now();
            lruList.erase(it->second.iter);
            lruList.push_front(key);
            it->second.iter = lruList.begin();
            obs.notify(key,value);
            return;
        }

        if (storage.size() >=config.maxSize) {
            
            k lruKey = lruList.back();
            lruList.pop_back();
            storage.erase(lruKey);

        }

        lruList.push_front(key);
        storage[key] = CacheEntry
        {
            std::make_shared<v>(value),
                std::chrono::steady_clock::now(),
                lruList.begin()
        };

        obs.notify(key,value);
   
    }

    std::weak_ptr<v> get(const k &key)
    {
        auto it  = storage.find(key);
        if (it != storage.end())
        {
            auto age = std::chrono::steady_clock::now() - it->second.createdAt;
            if (std::chrono::duration<double>(age).count() > config.ttl.count())
            {
                lruList.erase(it->second.iter);
                storage.erase(key);
                return {};
            }

            lruList.erase(it->second.iter);
            lruList.push_front(key);
            it->second.iter = lruList.begin();      
            return it->second.value;
        }

        return {};
    }

    bool contains(const k& key) {
        auto it = storage.find(key);
        if (it == storage.end()) return false;
    
        auto age = std::chrono::steady_clock::now() - it->second.createdAt;
        if (std::chrono::duration_cast<std::chrono::seconds>(age) > config.ttl) {
            // expired → cleanup
            lruList.erase(it->second.iter);
            storage.erase(it);
            return false;
        }
    
        return true;
    }

    void clear(){
        storage.clear();
        lruList.clear();
    }

private:
    Observer<k, v> obs;
    CacheConfig config;
    struct CacheEntry
    {
        std::shared_ptr<v> value;
        std::chrono::steady_clock::time_point createdAt;
        typename std::list<k>::iterator iter;
    };

    // void print_table() {
    //     for (auto& pair : storage) {
    //         auto& entry = pair.second; // assuming storage is like std::unordered_map<k, CacheEntry>
    
    //         if (entry.value) { // check if shared_ptr is not null
    //             std::cout << *(entry.value) << std::endl; // dereference shared_ptr
    //         } else {
    //             std::cout << "null" << std::endl;
    //         }
    //     }
        
    //     std::cout<<std::endl;
    //     std::cout<<std::endl;
    // }

    std::unordered_map<k, CacheEntry> storage;
    std::list<k> lruList;
    explicit Cache(const CacheConfig &cfg) : config(cfg) {};
};

#endif