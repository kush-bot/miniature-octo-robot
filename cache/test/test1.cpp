#include <iostream>
#include <string>
#include "../headers/cache.h"
#include <thread>
#include <chrono>
#include<vector>

class TestRunner
{
private:
    int passed = 0;
    int failed = 0;
    std::string current_test = "";

public:
    void startTest(const std::string &name)
    {
        current_test = name;
        std::cout << "\n--------Testing : " << name << "--------" << std::endl;
        std::cout << std::endl;
    }

    void assert_true(bool condition, const std::string &message)
    {
        if (condition)
        {
            std::cout << "✅ " << message << std::endl;
            passed++;
        }
        else
        {
            std::cout << "❌ " << message << "(FAILED)" << std::endl;
            failed++;
        }
    }

    void assert_false(bool condition, const std::string &message)
    {
        assert_true(!condition, message);
    }

    void getResults()
    {
        std::cout << std::endl;
        std::cout << "Test Result" << std::endl;
        std::cout << "Passed : " << passed << std::endl;
        std::cout << "Failed : " << failed << std::endl;
        std::cout << "Total : " << (passed + failed) << std::endl;

        if (failed == 0)
        {
            std::cout << "All Test cases Passed" << std::endl;
        }
        else
        {
            std::cout << "Few Test Cases Failed" << std::endl;
        }
    }
};

template <typename K, typename V>
void resetCache(Cache<K, V> &cache)
{
    cache.clear();
}

void testBasicOperation()
{
    TestRunner t_runner;
    t_runner.startTest("Basic Cache Opearations");

    auto &cache = Cache<int, std::string>::Instance();
    resetCache(cache);

    cache.put(1, "value1");
    auto result = cache.get(1);

    t_runner.assert_false(result.expired(), "Value should exist after put");

    if (!result.expired())
    {
        auto locked = result.lock();
        t_runner.assert_true(locked != nullptr, "lock should not exist");
        if (locked)
        {
            t_runner.assert_true(*locked == "value1", "the value should match");
        }
    }

    auto missing = cache.get(999);
    t_runner.assert_true(missing.expired(), "the key should not exist");

    t_runner.getResults();
}

void TestTtl()
{
    TestRunner t_runner;
    t_runner.startTest("ttl tests");
    CacheConfig config = Config::with(1, 100);

    auto cache = Cache<std::string, int>::Instance(config);
    resetCache(cache);

    cache.put("one", 1);
    auto result = cache.get("one");
    t_runner.assert_false(result.expired(), "the value should exist immediately ");
    std::this_thread::sleep_for(std::chrono::milliseconds(1150));

    // the value should be expired

    auto result2 = cache.get("one");

    t_runner.assert_true(result2.expired(), "the value should be expired now");

    t_runner.getResults();
}

void testLru()
{
    TestRunner t_runner;
    t_runner.startTest("LRU evivtion test");
    CacheConfig config = Config::with(100, 3);
    auto &cache = Cache<int, std::string>::Instance(config);

    resetCache(cache);

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    t_runner.assert_true(cache.contains(1), "the value should exist");
    t_runner.assert_true(cache.contains(2), "the value should exist");
    t_runner.assert_true(cache.contains(3), "the value should exist");
    t_runner.assert_false(cache.contains(4), "the value shouldn't exist");

    // auto res = cache.get(1);

    // if(!res.expired()){
    //     std::cout<< "the recent get" << *res.lock()<<std::endl;
    // }

    // Access 1 so it becomes MRU
    cache.get(1);

    cache.put(4, "four");

    t_runner.assert_true(cache.contains(1), "Key 1 should still exist (recently accessed)");
    t_runner.assert_false(cache.contains(2), "Key 2 should be evicted (LRU)");
    t_runner.assert_true(cache.contains(3), "Key 3 should still exist");
    t_runner.assert_true(cache.contains(4), "Key 4 should exist (newly added)");

    t_runner.getResults();
}

void testCacheConfiguration(){
    TestRunner t_runner;
    t_runner.startTest("Cache Configuartion test");
    CacheConfig config = Config::with(100,2);
    auto cache = Cache<std::string,int>::Instance(config);
    resetCache(cache);

    cache.put("a",1);
    cache.put("b",2);
    cache.put("c",3);

    t_runner.assert_false(cache.contains("a"),"the value should not exist");
    t_runner.assert_true(cache.contains("b"),"the value should exist");
    t_runner.assert_true(cache.contains("c"),"the value should exist");
}


void testObsersers(){
        TestRunner t_runner;
        t_runner.startTest("Observer pattren Test");


        auto cache = Cache<std::string,std::string>::Instance();
        resetCache(cache);
        std::vector<std::string> notifications;
        cache.subscribe([&notifications](const std::string& key,const std::string& value){
            notifications.push_back(key+":"+value);
        });

        cache.put("key1","value1");
        cache.put("key2","value2");

        t_runner.assert_true(notifications.size()==2,"the value should be equal");
        if(notifications.size()>=2){
            t_runner.assert_true(notifications[0] == "key1:value1","the value shoudl be equal");
            t_runner.assert_true(notifications[1] == "key2:value2","the value shoudl be equal");
        }

        t_runner.getResults();
}

void runPerformencrTest(){
    std::cout<<"\n Performence Testing"<<std::endl;
    CacheConfig config = Config::with(100,10000);
    auto& cache = Cache<int,std::string>::Instance(config);
    resetCache(cache);
    
    auto start = std::chrono::high_resolution_clock::now();

    for(int i=0; i<1000; i++){
        cache.put(i,"value_"+ std::to_string(i));
    }

    auto insert_end = std::chrono::high_resolution_clock::now();

    int hits = 0; 
    for(int i=0; i<1000; i++){
        auto result = cache.get(i);
        if(!result.expired()){
            hits++;
        }
    }

    auto read_end = std::chrono::high_resolution_clock::now();

    auto insert_time = std::chrono::duration_cast<std::chrono::microseconds>(insert_end-start);
    auto read_time = std::chrono::duration_cast<std::chrono::microseconds>(read_end-insert_end);

    std::cout << "📈 Results:" << std::endl;
    std::cout << "  Insert 1000 items: " << insert_time.count() << " μs" << std::endl;
    std::cout << "  Read 1000 items: " << read_time.count() << " μs" << std::endl;
    std::cout << "  Cache hits: " << hits << "/1000" << std::endl;
    std::cout << "  Avg insert: " << (insert_time.count() / 1000.0) << " μs per item" << std::endl;
    std::cout << "  Avg read: " << (read_time.count() / 1000.0) << " μs per item" << std::endl;
}

int main()
{
    testBasicOperation();
    TestTtl();
    testLru();
    testCacheConfiguration();
    testObsersers();
    runPerformencrTest();

}