Perfect! I can integrate your **test code and results** into the same GitHub-style `README.md`. This will include:

* Overview of the cache library
* API usage
* Examples
* Test suite demonstration
* Sample output

Here’s the complete `.md` file including your tests and outputs:

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
7. [Testing](#testing)  
8. [LRU + TTL Behavior](#lru--ttl-behavior)  

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

## Testing

The library comes with a **basic test suite** using a `TestRunner` helper class.

### TestRunner Usage

```cpp
TestRunner t;
t.startTest("Test Name");
t.assert_true(condition, "message");
t.assert_false(condition, "message");
t.getResults();
```

---

### Included Tests

1. **Basic Cache Operations**
2. **TTL Expiry**
3. **LRU Eviction**
4. **Cache Configuration**
5. **Observer Pattern**
6. **Performance Test**

### Example Test Code

```cpp
void testBasicOperation() {
    TestRunner t_runner;
    t_runner.startTest("Basic Cache Operations");

    auto &cache = Cache<int, std::string>::Instance();
    cache.clear();

    cache.put(1, "value1");
    auto result = cache.get(1);
    t_runner.assert_false(result.expired(), "Value should exist after put");
    
    auto missing = cache.get(999);
    t_runner.assert_true(missing.expired(), "the key should not exist");

    t_runner.getResults();
}
```

---

### Sample Test Output

```
--------Testing : Basic Cache Opearations--------

✅ Value should exist after put
✅ lock should not exist
✅ the value should match
✅ the key should not exist

Test Result
Passed : 4
Failed : 0
Total : 4
All Test cases Passed

--------Testing : ttl tests--------

✅ the value should exist immediately 
✅ the value should be expired now

Test Result
Passed : 2
Failed : 0
Total : 2
All Test cases Passed

--------Testing : LRU evivtion test--------

✅ the value should exist
✅ the value should exist
✅ the value should exist
✅ the value shouldn't exist
✅ Key 1 should still exist (recently accessed)
✅ Key 2 should be evicted (LRU)
✅ Key 3 should still exist
✅ Key 4 should exist (newly added)

Test Result
Passed : 8
Failed : 0
Total : 8
All Test cases Passed

--------Testing : Cache Configuartion test--------

✅ the value should not exist
✅ the value should exist
✅ the value should exist

--------Testing : Observer pattren Test--------

✅ the value should be equal
✅ the value shoudl be equal
✅ the value shoudl be equal

Performence Testing
📈 Results:
  Insert 1000 items: 3803 μs
  Read 1000 items: 1146 μs
  Cache hits: 1000/1000
  Avg insert: 3.803 μs per item
  Avg read: 1.146 μs per item
```

---

## LRU + TTL Behavior

* **LRU Eviction**: When `maxSize` is reached, the **least recently used** key is automatically removed.
* **TTL Expiry**: Each entry expires after `ttl` seconds. Expired entries are automatically removed on access (`get` or `contains`).
* **Observers**: Subscribers are notified on `put` events.

---

This README provides a **complete overview** of the cache library, usage, configuration, tests, and sample output.

```

---

