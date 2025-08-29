    #include<iostream>
    #include"headers/cache.h"
    #include<memory>
    #include<string>
    #include"headers/config.h"
    #include<thread>
    int main(){

        CacheConfig cfg =  Config::with(15,300);

        auto& cache = Cache<int,std::string>::Instance(cfg);
        cache.subscribe([](const int& key,const std::string& value){
            std::cout<<"cache update : "<<key<<" = "<<value<<std::endl;
        });
        cache.put(42,std::string("this is 42"));

        if(!cache.get(42).expired()){
            auto res = cache.get(42).lock();
            std::cout<<*res<<std::flush;
        }else{
            std::cout<<"no value"<<std::flush;
        }
        std::this_thread::sleep_for(std::chrono::seconds(12));
        if(!cache.get(42).expired()){
            auto res = cache.get(42).lock();
            std::cout<<"first in thread"<<*res<<std::flush;
        }else{
            std::cout<<"no value"<<std::flush;
        }

        std::this_thread::sleep_for(std::chrono::seconds(20));
        if(!cache.get(42).expired()){
            auto res = cache.get(42).lock();
            std::cout<<*res<<std::flush;
        }else{
            std::cout<<"no value"<<std::flush;
        }
        
    }