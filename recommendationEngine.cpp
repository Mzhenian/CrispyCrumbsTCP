#include "recommendationEngine.hpp"

// using namespace RecommendationEngine;

int RecommendationEngine::criticalMass() {
    return log(usersCount);
}

RecommendationEngine::RecommendationEngine() {
}

RecommendationEngine::~RecommendationEngine() {
}

vector<string> RecommendationEngine::getRecommendations(const vector<string>& userWatchHistory, const string& UserId) {
    // todo implement
    cout << "A logged-in user watched a video" << endl;
    return vector<string>({"6679d78c94c2dcb530d27adb", "6679d76c94c2dcb530d27ad5"});
}

// union
void RecommendationEngine::loadUser(vector<string>& userWatchHistory, const string& userId) {
    // todo implement
    cout << "loaded user" << endl;
}

// make
void RecommendationEngine::loadVideo(const string& videoId, int views) {
    // todo implement
    cout << "A video being viewed" << endl;
}