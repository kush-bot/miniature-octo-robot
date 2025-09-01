#ifndef CONFIG_H
#define CONFIG_H
#include<iostream>
#include<ctype.h>
#include<chrono>

struct CacheConfig{
    std::chrono::seconds ttl{10};
    size_t maxSize = 300;

};

class Config{
public:
    
    static CacheConfig Default(){
        return CacheConfig{};
    }

    static CacheConfig with(unsigned int  seconds, unsigned int size){
        return CacheConfig{
            std::chrono::seconds(seconds),
            static_cast<size_t>(size)
        };
    }



};




#endif