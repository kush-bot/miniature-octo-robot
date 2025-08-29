#ifndef OBSERVER_H
#define OBSERVER_H
#include<vector>
#include<string>
#include<functional>

template <typename k,typename v>
class Observer{

public:
    using Callback = std::function<void(const k&, const v&)>; 
    
    void subscribe(Callback cb){
        observers.push_back(cb);
    }
    void notify(k key,v value){
        for(Callback cb:observers){
            cb(key,value);
        }
    }
private:
    std::vector<Callback> observers;
};

#endif