#ifndef CACHE_H
#define CACHE_H
#include<unordered_map>
#include<string>
#include "observer.h"
#include<memory>

template<typename k,typename v>
class Cache{
public:

    using Callback = typename Observer<k,v>::Callback;
    static Cache& Instance(){
        static Cache cache;
        return cache;
    }

    void subscribe(Callback cb){
        obs.subscribe(cb);
    }

    void put(const k& key, const v& value){
        storage[key]=std::make_shared<v>(value);
        obs.notify(key,value);
    }


    std::weak_ptr<v> get(const k& key){
        if(storage.find(key)!=storage.end()){
            return storage[key];
        }
        return {};
    }

    


private:
    std::unordered_map<k,std::shared_ptr<v>> storage; 
    Observer<k,v> obs;
};



#endif