#ifndef RECOMMENDATION_ENGINE_HPP
#define RECOMMENDATION_ENGINE_HPP

#include <math.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <thread>
#include <future>

using namespace std;

#define RECOMMENDATION_COUNT 10

/// Manage a Union Find data structure to generate video recommendations by precised watch history
class RecommendationEngine {
   public:
    RecommendationEngine();
    ~RecommendationEngine();

    // find
    vector<string> getRecommendations(const vector<string>& userWatchHistory, const string& userId, const string& videoId);

    // union
    void loadUser(const vector<string>& userWatchHistory, const string& userId);

    // make
    void loadVideo(const string& videoId, int views);

    // void removeUser(vector<string>& userWatchHistory);

    // void removeVideo(const string& videoId);

   private:
    int usersCount = 0;
    int criticalMass();
};

#endif  // RECOMMENDATION_ENGINE_HPP