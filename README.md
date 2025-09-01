# C++ LRU Cache Library with TTL and Observer

A **template-based, in-memory LRU cache** for C++ with TTL (time-to-live) support and observer callbacks. Each cache instance is **singleton per type**.

---

## Features

- Type-safe cache (`Cache<Key, Value>`)  
- Least Recently Used (LRU) eviction when `maxSize` is reached  
- TTL per entry for automatic expiry  
- Observer pattern for change notifications (`put`)  
- Singleton per `<Key, Value>` type combination  

---

## Table of Contents

1. [Installation](#installation)  
2. [Usage](#usage)  
3. [Classes and Methods](#classes-and-methods)  
4. [Configuration](#configuration)  
5. [Observer Callbacks](#observer-callbacks)  

---

## Installation

1. Copy the headers to your project:  
   - `cache.h`  
   - `config.h`  
   - `observer.h`  
2. Include in your C++ source:

```cpp
#include "cache.h"
#include "config.h"
#include "observer.h"


##Usage

auto& cache = Cache<int, std::string>::Instance(); // default config

##Using a custom configuration

CacheConfig cfg = Config::with(20, 100); // TTL 20s, max 100 entries
auto& cache = Cache<int, std::string>::Instance(cfg);


---
##Putting and getting values

cache.put(1, "Hello World");

auto val = cache.get(1).lock(); // weak_ptr to value
if (val) {
    std::cout << *val << std::endl;
}

Checking if a key exists

if (cache.contains(1)) {
    std::cout << "Key exists and not expired" << std::endl;
}


Clearing the cache

cache.clear();


Classes and Methods
Cache<Key, Value>

Static Methods

Instance(const CacheConfig &cfg = Config::Default())
Returns a singleton cache object for this type. Optional config only affects first creation.

Member Methods

void put(const Key &key, const Value &value)
Insert or update a value. Moves key to most-recently-used position.

std::weak_ptr<Value> get(const Key &key)
Retrieves a value if it exists and not expired; updates LRU position.

bool contains(const Key &key)
Returns true if the key exists and TTL has not expired.

void clear()
Clears all cache entries.

void subscribe(Callback cb)
Subscribe to change events (put).

CacheConfig

Fields

std::chrono::seconds ttl — time-to-live per entry

size_t maxSize — maximum number of entries

Helper Methods (Config class)

static CacheConfig Default() — default config (TTL 10s, max 300)

static CacheConfig with(unsigned int seconds, unsigned int size) — custom TTL and max size

Observer<Key, Value>

Allows subscribing to cache changes.

Callback signature: void(const Key&, const Value&)
