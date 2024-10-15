#ifndef RECOMMENDATION_ENGINE_HPP
#define RECOMMENDATION_ENGINE_HPP

#include <math.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define RECOMMENDATION_COUNT 10

/// Manage a Union Find data structure to generate video recommendations by precised watch history
class RecommendationEngine {
   public:
    RecommendationEngine();
    ~RecommendationEngine();

    // find
    vector<string> getRecommendations(const vector<string>& userWatchHistory, const string& userId);

    // union
    void loadUser(vector<string>& userWatchHistory, const string& userId);

    // make
    void loadVideo(const string& videoId, int views);

    // void removeUser(vector<string>& userWatchHistory);

    // void removeVideo(const string& videoId);

   private:
    int usersCount = 0;
    int criticalMass();
};

#endif  // RECOMMENDATION_ENGINE_HPP