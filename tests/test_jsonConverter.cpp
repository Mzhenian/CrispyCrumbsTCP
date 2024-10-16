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
        unique_ptr<vector<string>> result = parseJsonStringArray(jsonStr, it);
        assert(result->empty());
        std::cout << "Test case 1 passed.\n";
    }

    // Test case 2: Array with strings
    {
        string jsonStr = "[\"a\", \"b\", \"c\"]";
        auto it = jsonStr.begin();
        unique_ptr<vector<string>> result = parseJsonStringArray(jsonStr, it);
        assert(result->size() == 3);
        assert((*result)[0] == "a");
        assert((*result)[1] == "b");
        assert((*result)[2] == "c");
        std::cout << "Test case 2 passed.\n";
    }
}

int main() {
    testParseJsonArray();
    std::cout << "All test cases passed.\n";
    return 0;
}