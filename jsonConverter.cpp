#include "jsonConverter.hpp"

using namespace JsonConverter;

const unique_ptr<jsonObject> JsonConverter::parseJsonObject(const string& jsonStr, strIt& it) {
    while (it != jsonStr.end() && (*it == ' ' || *it == '\n')) it++;  // Skip whitespace
    if (it == jsonStr.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing object");
    }
    if (*it != '{') {
        throw invalid_argument("Expected '{' at the beginning of JSON object");
    }
    it++;  // Move past '{'

    unique_ptr<jsonObject> json_map(new jsonObject());

    while (it != jsonStr.end() && *it != '}') {
        while (it != jsonStr.end() && (*it == ' ' || *it == '\n' || *it == ',')) it++;  // Skip comma
        json_map->insert(parseKeyValuePair(jsonStr, it));
    }
    if (it == jsonStr.end()) {
        throw invalid_argument("JSON object is not properly closed");
    }
    it++;  // Move past '}'

    return json_map;
}

const unique_ptr<jsonValue> JsonConverter::parseJsonValue(const string& jsonStr, strIt& it) {
    while (it != jsonStr.end() && (*it == ' ' || *it == '\n')) it++;  // Skip whitespace
    if (it == jsonStr.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing value");
    }
    if (*it == '\0') {
        throw invalid_argument("Unexpected null character in JSON string");
    }

    if (*it == '{') {
        unique_ptr<jsonObject> obj = parseJsonObject(jsonStr, it);
        return unique_ptr<jsonValue>(new jsonValue(obj.release()));
    } else if (*it == '[') {
        unique_ptr<vector<jsonValue>> arr = parseJsonArray(jsonStr, it);
        return unique_ptr<jsonValue>(new jsonValue(arr.release()));
    } else if (*it == '\"') {
        return unique_ptr<jsonValue>(new jsonValue(parseString(jsonStr, it)));
    } else {
        return unique_ptr<jsonValue>(new jsonValue(parseInt(jsonStr, it)));
    }
}

const unique_ptr<vector<jsonValue>> JsonConverter::parseJsonArray(const string& jsonArrStr, strIt& it) {
    while (it != jsonArrStr.end() && (*it == ' ' || *it == '\n')) it++;  // Skip whitespace
    if (it == jsonArrStr.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing array");
    }
    if (*it != '[') {
        throw invalid_argument("Expected '[' at the beginning of JSON array");
    }
    it++;  // Move past '['

    unique_ptr<vector<jsonValue>> jsonArray(new vector<jsonValue>());

    while (it != jsonArrStr.end() && *it != ']') {
        while (it != jsonArrStr.end() && (*it == ' ' || *it == '\n' || *it == ',')) it++;  // Skip comma
        jsonArray->emplace_back(*JsonConverter::parseJsonValue(jsonArrStr, it));
    }
    if (it == jsonArrStr.end()) {
        throw invalid_argument("JSON array is not properly closed");
    }
    it++;  // Move past ']'

    return jsonArray;
}

pair<string, jsonValue> JsonConverter::parseKeyValuePair(const string& s, strIt& it) {
    while (it != s.end() && (*it == ' ' || *it == '\n')) it++;  // Skip whitespace
    if (it == s.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing key-value pair");
    }
    if (*it != '\"') {
        throw invalid_argument("Expected '\"' at the beginning of JSON key");
    }

    string key = parseString(s, it);

    while (it != s.end() && (*it == ' ' || *it == '\n')) it++;  // Skip whitespace
    if (it == s.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing key-value pair");
    }
    if (*it != ':') {
        throw invalid_argument("Expected ':' after JSON key");
    }
    it++;  // Move past ':'

    jsonValue value = *parseJsonValue(s, it);

    return make_pair(key, value);
}

string JsonConverter::parseString(const string& s, strIt& it) {
    while (it != s.end() && (*it == ' ' || *it == '\n')) it++;
    if (it == s.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing string");
    }
    if (*it != '\"') {
        throw invalid_argument("Expected '\"' at the beginning of JSON string");
    }
    it++;

    strIt start = it;
    while (it != s.end() && *it != '\"') it++;
    if (it == s.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing string");
    }

    string str = s.substr(start - s.begin(), it - start);
    it++;  // Move past closing quote
    return str;
}

int JsonConverter::parseInt(const string& jsonInt, strIt& it) {
    while (it != jsonInt.end() && (*it == ' ' || *it == '\n')) it++;
    if (it == jsonInt.end()) {
        throw invalid_argument("Unexpected end of JSON string while parsing integer");
    }
    if (*it == '\0') {
        throw invalid_argument("Unexpected null character in JSON string");
    }

    strIt start = it;
    while (it != jsonInt.end() && isdigit(*it)) it++;
    if (start == it) {
        throw invalid_argument("Expected digit while parsing integer");
    }
    int number = stoi(jsonInt.substr(start - jsonInt.begin(), it - start));

    return number;
}