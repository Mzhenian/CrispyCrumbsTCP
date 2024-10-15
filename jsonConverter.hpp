#ifndef JSON_CONVERTER_HPP
#define JSON_CONVERTER_HPP

#include <algorithm>
#include <cassert>
#include <cctype>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace JsonConverter {
using namespace std;
using strIt = string::iterator;

struct jsonValue;
// object means a pair, excluding array
using jsonObject = map<string, jsonValue>;

/**
 * recursive data structure to represent a json object
 */
struct jsonValue {
    variant<int, string, jsonObject, vector<jsonValue>> value;

    // Constructors for the different types
    jsonValue(int num) : value(num) {}
    jsonValue(const string& s) : value(s) {}

    jsonValue(jsonObject* obj) : value(*obj) { delete obj; }
    jsonValue(vector<jsonValue>* arr) : value(*arr) { delete arr; }
};

const unique_ptr<jsonValue> parseJsonValue(const string& jsonStr, strIt& it);
const unique_ptr<jsonObject> parseJsonObject(const string& jsonObjStr, strIt& it);
const unique_ptr<vector<jsonValue>> parseJsonArray(const string& jsonArrStr, strIt& it);
string parseString(const string& jsonStr, strIt& it);
int parseInt(const string& jsonInt, strIt& it);
pair<string, jsonValue> parseKeyValuePair(const string& jsonValueStr, strIt& it);
}  // namespace JsonConverter

#endif
