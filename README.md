````markdown
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
6. [Examples](#examples)  
7. [LRU + TTL Behavior](#lru--ttl-behavior)  

---

## Installation

Copy the headers to your project:  
- `cache.h`  
- `config.h`  
- `observer.h`  

Include them in your C++ source:

```cpp
#include "cache.h"
#include "config.h"
#include "observer.h"
````

---

## Usage

### Creating or accessing a cache

```cpp
auto& cache = Cache<int, std::string>::Instance(); // default config
```

### Using a custom configuration

```cpp
CacheConfig cfg = Config::with(20, 100); // TTL 20s, max 100 entries
auto& cache = Cache<int, std::string>::Instance(cfg);
```

---

### Putting and getting values

```cpp
cache.put(1, "Hello World");

auto val = cache.get(1).lock(); // weak_ptr to value
if (val) {
    std::cout << *val << std::endl;
}
```

---

### Checking if a key exists

```cpp
if (cache.contains(1)) {
    std::cout << "Key exists and not expired" << std::endl;
}
```

---

### Clearing the cache

```cpp
cache.clear();
```

---

## Classes and Methods

### `Cache<Key, Value>`

* **Static Methods**

  * `Instance(const CacheConfig &cfg = Config::Default())`
    Returns a singleton cache object for this type. Optional config only affects **first creation**.

* **Member Methods**

  * `void put(const Key &key, const Value &value)`
    Insert or update a value. Moves key to most-recently-used position.
  * `std::weak_ptr<Value> get(const Key &key)`
    Retrieves a value if it exists and not expired; updates LRU position.
  * `bool contains(const Key &key)`
    Returns true if the key exists and TTL has not expired.
  * `void clear()`
    Clears all cache entries.
  * `void subscribe(Callback cb)`
    Subscribe to change events (`put`).

---

### `CacheConfig`

* **Fields**

  * `std::chrono::seconds ttl` — time-to-live per entry
  * `size_t maxSize` — maximum number of entries

* **Helper Methods (Config class)**

  * `static CacheConfig Default()` — default config (TTL 10s, max 300)
  * `static CacheConfig with(unsigned int seconds, unsigned int size)` — custom TTL and max size

---

### `Observer<Key, Value>`

* Allows subscribing to cache changes.
* **Callback signature**: `void(const Key&, const Value&)`

```cpp
cache.subscribe([](const int& k, const std::string& v){
    std::cout << "Key " << k << " updated with value " << v << std::endl;
});
```

---

## Examples

```cpp
#include "cache.h"
#include "config.h"
#include <iostream>

int main() {
    CacheConfig cfg = Config::with(5, 2); // TTL 5s, max 2 entries
    auto& cache = Cache<int, std::string>::Instance(cfg);

    cache.subscribe([](const int& k, const std::string& v){
        std::cout << "Updated key " << k << " with value " << v << std::endl;
    });

    cache.put(1, "Hello");
    cache.put(2, "World");

    auto val = cache.get(1).lock();
    if(val) std::cout << *val << std::endl;

    cache.put(3, "LRU Eviction"); // Evicts key 2 (least recently used)

    return 0;
}
```

---

## LRU + TTL Behavior

* **LRU Eviction**: When `maxSize` is reached, the **least recently used** key is automatically removed.
* **TTL Expiry**: Each entry expires after `ttl` seconds. Expired entries are automatically removed on access (`get` or `contains`).
* **Observers**: Subscribers are notified on `put` events.

---

This README provides a **complete overview** of the API, usage, configuration, and behavior, ready to use as a **GitHub README.md**.

```

---


```
