// test_jsonConverter.cpp
#include "../jsonConverter.hpp"
#include <cassert>
#include <iostream>

using namespace JsonConverter;

void testParseJsonArray() {
    // Test case 1: Empty array
    {
        string jsonStr = "[]";
        auto it = jsonStr.begin();
        unique_ptr<vector<jsonValue>> result = parseJsonArray(jsonStr, it);
        assert(result->empty());
        std::cout << "Test case 1 passed.\n";
    }

    // Test case 2: Array with integers
    {
        string jsonStr = "[1, 2, 3]";
        auto it = jsonStr.begin();
        unique_ptr<vector<jsonValue>> result = parseJsonArray(jsonStr, it);
        assert(result->size() == 3);
        assert(std::get<int>((*result)[0].value) == 1);
        assert(std::get<int>((*result)[1].value) == 2);
        assert(std::get<int>((*result)[2].value) == 3);
        std::cout << "Test case 2 passed.\n";
    }

    // Test case 3: Array with strings
    {
        string jsonStr = "[\"a\", \"b\", \"c\"]";
        auto it = jsonStr.begin();
        unique_ptr<vector<jsonValue>> result = parseJsonArray(jsonStr, it);
        assert(result->size() == 3);
        assert(std::get<string>((*result)[0].value) == "a");
        assert(std::get<string>((*result)[1].value) == "b");
        assert(std::get<string>((*result)[2].value) == "c");
        std::cout << "Test case 3 passed.\n";
    }

    // Test case 4: Nested arrays
    {
        string jsonStr = "[[1, 2], [3, 4]]";
        auto it = jsonStr.begin();
        unique_ptr<vector<jsonValue>> result = parseJsonArray(jsonStr, it);
        assert(result->size() == 2);
        assert(std::get<vector<jsonValue>>((*result)[0].value).size() == 2);
        assert(std::get<vector<jsonValue>>((*result)[1].value).size() == 2);
        assert(std::get<int>(std::get<vector<jsonValue>>((*result)[0].value)[0].value) == 1);
        assert(std::get<int>(std::get<vector<jsonValue>>((*result)[0].value)[1].value) == 2);
        assert(std::get<int>(std::get<vector<jsonValue>>((*result)[1].value)[0].value) == 3);
        assert(std::get<int>(std::get<vector<jsonValue>>((*result)[1].value)[1].value) == 4);
        std::cout << "Test case 4 passed.\n";
    }

    // Test case 5: Mixed types
    {
        string jsonStr = "[1, \"a\", [2, \"b\"]]";
        auto it = jsonStr.begin();
        unique_ptr<vector<jsonValue>> result = parseJsonArray(jsonStr, it);
        assert(result->size() == 3);
        assert(std::get<int>((*result)[0].value) == 1);
        assert(std::get<string>((*result)[1].value) == "a");
        assert(std::get<vector<jsonValue>>((*result)[2].value).size() == 2);
        assert(std::get<int>(std::get<vector<jsonValue>>((*result)[2].value)[0].value) == 2);
        assert(std::get<string>(std::get<vector<jsonValue>>((*result)[2].value)[1].value) == "b");
        std::cout << "Test case 5 passed.\n";
    }
}

int main() {
    testParseJsonArray();
    std::cout << "All test cases passed.\n";
    return 0;
}