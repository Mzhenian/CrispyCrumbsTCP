#include "recommendationEngine.hpp"

// using namespace RecommendationEngine;
// Data structures
unordered_map<string, unordered_set<string>> videoRecommendationsMap;  // videoID -> set of

mutex dataMutex;  // Mutex for thread-safe access to data structures

int RecommendationEngine::criticalMass() {
    return log(usersCount);
}

RecommendationEngine::RecommendationEngine() {
}

RecommendationEngine::~RecommendationEngine() {
}

vector<string> RecommendationEngine::getRecommendations(const vector<string>& userWatchHistory, const string& UserId, const string& videoId) {
    cout << "A logged-in user watched a video" << endl;
    loadUser(userWatchHistory, UserId);
    unordered_map<string, unordered_set<string>> mapCopy = videoRecommendationsMap;
    vector<string> recommendations;

    for (auto i : mapCopy[videoId]) {
        if (find(userWatchHistory.begin(), userWatchHistory.end(), i) == userWatchHistory.end()) {
            recommendations.push_back(i);
        }
    }
    return recommendations;
}

// union
void RecommendationEngine::loadUser(const vector<string>& userWatchHistory, const string& userId) {
    cout << "loading user" << endl;
    thread([userWatchHistory, userId]() {
        lock_guard<mutex> lock(dataMutex);

        for (const auto& i : userWatchHistory) {
            for (const auto& j : userWatchHistory) {
                videoRecommendationsMap[i].insert(j);
            }
        }
    }).detach();
}
// make
void RecommendationEngine::loadVideo(const string& videoId, int views) {
    cout << "A video being viewed" << endl;
    // todo currently unused
}