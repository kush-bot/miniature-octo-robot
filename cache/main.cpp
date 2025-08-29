#include<iostream>
#include"headers/cache.h"
#include<memory>
#include<string>


int main(){

    
    auto& cache = Cache<int,std::string>::Instance();
    cache.subscribe([](const int& key,const std::string& value){
        std::cout<<"cache update : "<<key<<" = "<<value<<std::endl;
    });
    cache.put(42,std::string("this is 42"));

    auto cached = cache.get(42);
    
    if(!cached.expired()){
        auto res = cached.lock();
        std::cout<<*res<<std::endl;
    }

    cache.put(50,"key is now 50");
    auto cached2 = cache.get(100);
    if(!cached2.expired()){
        auto res = cached2.lock();
        std::cout<<"chached two is"<<*res;
    }else{
        std::cout<<"no cache value";
    }

    

}