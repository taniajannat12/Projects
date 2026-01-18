#include <bits/stdc++.h>
using namespace std;

/*------------------------------
   Structures
-------------------------------*/
struct Address {
    int id;
    string name;
    double x, y;
};

struct Depot {
    string name;
    double x, y;
};

struct DeliveryBoy {
    int id;
    string name;
    double speed;
    int capacity;
};

/*------------------------------
   Distance Function
-------------------------------*/
double distance(const Address &a, const Address &b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx*dx + dy*dy);
}

/*------------------------------
   1. REAL K-Means Clustering
-------------------------------*/
vector<vector<Address>> kMeansClustering(vector<Address> addresses, int k) {
    if (addresses.empty() || k <= 0) return vector<vector<Address>>(k);

    if (addresses.size() <= k) {
        vector<vector<Address>> clusters(k);
        for (int i = 0; i < addresses.size(); i++) {
            clusters[i].push_back(addresses[i]);
        }
        return clusters;
    }

    vector<pair<double, double>> centroids(k);
    srand(time(0));
    for (int i = 0; i < k; i++) {
        int idx = rand() % addresses.size();
        centroids[i] = {addresses[idx].x, addresses[idx].y};
    }

    vector<vector<Address>> clusters(k);
    bool changed = true;
    int maxIter = 100;

    while (changed && maxIter-- > 0) {
        for (auto &cluster : clusters) cluster.clear();

        for (auto &addr : addresses) {
            int bestCluster = 0;
            double minDist = 1e18;
            for (int i = 0; i < k; i++) {
                double dx = addr.x - centroids[i].first;
                double dy = addr.y - centroids[i].second;
                double dist = dx*dx + dy*dy;
                if (dist < minDist) {
                    minDist = dist;
                    bestCluster = i;
                }
            }
            clusters[bestCluster].push_back(addr);
        }

        changed = false;
        for (int i = 0; i < k; i++) {
            if (clusters[i].empty()) continue;

            double sumX = 0, sumY = 0;
            for (auto &addr : clusters[i]) {
                sumX += addr.x;
                sumY += addr.y;
            }

            double newX = sumX / clusters[i].size();
            double newY = sumY / clusters[i].size();

            if (abs(newX - centroids[i].first) > 0.001 ||
                abs(newY - centroids[i].second) > 0.001) {
                changed = true;
                centroids[i] = {newX, newY};
            }
        }
    }

    return clusters;
}

/*------------------------------
   2. Greedy Route (Nearest Neighbor) - FIXED
-------------------------------*/
vector<Address> greedyRoute(vector<Address> addresses, Address start) {
    if (addresses.empty()) {
        vector<Address> route;
        route.push_back(start);
        route.push_back(start);
        return route;
    }

    vector<Address> route;
    route.push_back(start);

    vector<bool> visited(addresses.size(), false);
    Address current = start;

    for (int cnt = 0; cnt < addresses.size(); cnt++) {
        int nearestIdx = -1;
        double minDist = 1e18;

        for (int i = 0; i < addresses.size(); i++) {
            if (!visited[i]) {
                double d = distance(current, addresses[i]);
                if (d < minDist) {
                    minDist = d;
                    nearestIdx = i;
                }
            }
        }

        if (nearestIdx != -1) {
            visited[nearestIdx] = true;
            current = addresses[nearestIdx];
            route.push_back(current);
        }
    }

    // Return to depot - MUST BE INCLUDED
    route.push_back(start);
    return route;
}

/*------------------------------
   3. TSP DP - FIXED
-------------------------------*/
vector<int> tspDP(const vector<vector<double>> &distMatrix, int start) {
    int n = distMatrix.size();
    if (n <= 1) return {start};

    int states = 1 << n;
    vector<vector<double>> dp(states, vector<double>(n, 1e18));
    vector<vector<int>> parent(states, vector<int>(n, -1));

    dp[1 << start][start] = 0;

    for (int mask = 0; mask < states; mask++) {
        for (int last = 0; last < n; last++) {
            if (dp[mask][last] < 1e18) {
                for (int next = 0; next < n; next++) {
                    if (!(mask & (1 << next))) {
                        int newMask = mask | (1 << next);
                        double newDist = dp[mask][last] + distMatrix[last][next];
                        if (newDist < dp[newMask][next]) {
                            dp[newMask][next] = newDist;
                            parent[newMask][next] = last;
                        }
                    }
                }
            }
        }
    }

    // Find optimal path
    int finalMask = states - 1;
    int lastNode = start;
    double minCost = dp[finalMask][start];

    for (int i = 0; i < n; i++) {
        if (i != start && dp[finalMask][i] + distMatrix[i][start] < minCost) {
            minCost = dp[finalMask][i] + distMatrix[i][start];
            lastNode = i;
        }
    }

    // Reconstruct
    vector<int> path;
    int mask = finalMask;
    int current = lastNode;

    while (current != -1) {
        path.push_back(current);
        int prev = parent[mask][current];
        mask ^= (1 << current);
        current = prev;
    }

    reverse(path.begin(), path.end());
    if (path[0] != start) {
        path.insert(path.begin(), start);
    }

    // Return to start
    path.push_back(start);
    return path;
}

/*------------------------------
   4. Divide & Conquer - FIXED
-------------------------------*/
vector<Address> divideAndConquerRoute(vector<Address> addresses, Address start) {
    if (addresses.empty()) {
        vector<Address> route;
        route.push_back(start);
        route.push_back(start);
        return route;
    }

    if (addresses.size() <= 3) {
        vector<Address> all = addresses;
        all.push_back(start);

        int n = all.size();
        vector<vector<double>> dist(n, vector<double>(n));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                dist[i][j] = distance(all[i], all[j]);
            }
        }

        vector<int> pathIndices = tspDP(dist, n-1);

        vector<Address> route;
        for (int idx : pathIndices) {
            route.push_back(all[idx]);
        }
        return route;
    }

    // Split
    sort(addresses.begin(), addresses.end(),
         [](const Address &a, const Address &b) { return a.x < b.x; });

    int mid = addresses.size() / 2;
    vector<Address> left(addresses.begin(), addresses.begin() + mid);
    vector<Address> right(addresses.begin() + mid, addresses.end());

    vector<Address> leftRoute = divideAndConquerRoute(left, start);
    vector<Address> rightRoute = divideAndConquerRoute(right, start);

    // Merge
    leftRoute.pop_back();
    leftRoute.insert(leftRoute.end(), rightRoute.begin(), rightRoute.end());
    return leftRoute;
}

/*------------------------------
   Improved Visualization Function
-------------------------------*/
void visualizeRoute(const vector<Address>& route, const string& boyName, int routeNumber) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "     ROUTE VISUALIZATION - " << boyName << " (Route #" << routeNumber << ")\n";
    cout << string(60, '=') << "\n";

    // Check if all points are same
    bool allSame = true;
    for (int i = 1; i < route.size(); i++) {
        if (route[i].x != route[0].x || route[i].y != route[0].y) {
            allSame = false;
            break;
        }
    }

    if (allSame) {
        cout << "\n NOTICE: All delivery points are at the depot location!\n";
        cout << "   Location: (" << route[0].x << ", " << route[0].y << ")\n";
        cout << "   Status: No travel required!\n";
        cout << string(60, '=') << "\n";
        return;
    }

    // Simple coordinate plot
    const int GRID_SIZE = 12;

    // Find min and max coordinates
    double minX = route[0].x, maxX = route[0].x;
    double minY = route[0].y, maxY = route[0].y;

    for (const auto& addr : route) {
        minX = min(minX, addr.x);
        maxX = max(maxX, addr.x);
        minY = min(minY, addr.y);
        maxY = max(maxY, addr.y);
    }

    // Ensure minimum range for better visualization
    if (maxX - minX < 2.0) {
        double centerX = (minX + maxX) / 2;
        minX = centerX - 3.0;
        maxX = centerX + 3.0;
    }
    if (maxY - minY < 2.0) {
        double centerY = (minY + maxY) / 2;
        minY = centerY - 3.0;
        maxY = centerY + 3.0;
    }

    // Add margins
    double rangeX = maxX - minX;
    double rangeY = maxY - minY;
    minX -= rangeX * 0.15;
    maxX += rangeX * 0.15;
    minY -= rangeY * 0.15;
    maxY += rangeY * 0.15;
    rangeX = maxX - minX;
    rangeY = maxY - minY;

    // Create grid
    vector<vector<char>> grid(GRID_SIZE, vector<char>(GRID_SIZE, '.'));

    // Place points on grid
    for (int i = 0; i < route.size(); i++) {
        int col = (int)((route[i].x - minX) / rangeX * (GRID_SIZE - 1));
        int row = (int)((route[i].y - minY) / rangeY * (GRID_SIZE - 1));
        row = GRID_SIZE - 1 - row; // Invert Y-axis

        if (col >= 0 && col < GRID_SIZE && row >= 0 && row < GRID_SIZE) {
            if (i == 0 || i == route.size()-1) {
                grid[row][col] = 'D'; // Depot
            } else {
                if (i < 10) grid[row][col] = '0' + i;
                else grid[row][col] = 'A' + (i - 10);
            }
        }
    }

    // Print the map
    cout << "\n          SIMPLE ROUTE MAP\n";
    cout << "          (Y increases upwards)\n\n";

    for (int i = 0; i < GRID_SIZE; i++) {
        cout << "    ";
        for (int j = 0; j < GRID_SIZE; j++) {
            cout << grid[i][j] << " ";
        }
        cout << "\n";
    }

    // Legend
    cout << "\n    MAP LEGEND:\n";
    cout << "    D = Depot (Start & End Point)\n";
    cout << "    1,2,3... = Delivery Stops in sequence\n";
    cout << "    . = Empty Area\n";

    // Route information
    cout << "\n    ROUTE INFORMATION:\n";
    cout << "    " << string(40, '-') << "\n";

    double totalDist = 0;
    for (int i = 0; i < route.size()-1; i++) {
        double dist = distance(route[i], route[i+1]);
        totalDist += dist;

        cout << "    " << setw(10) << left << route[i].name
             << " -> " << setw(10) << left << route[i+1].name
             << " : " << fixed << setprecision(2) << setw(6) << dist << " km\n";
    }

    cout << "    " << string(40, '-') << "\n";
    cout << "    TOTAL DISTANCE: " << fixed << setprecision(2) << totalDist << " km\n";

    cout << "\n" << string(60, '=') << "\n";
}

/*------------------------------
   Improved Competition Analysis
-------------------------------*/
void analyzeCompetition(const vector<vector<Address>>& routes,
                       const vector<DeliveryBoy>& boys) {
    cout << "\n" << string(70, '=') << "\n";
    cout << "                     COMPETITION RESULTS\n";
    cout << string(70, '=') << "\n";

    vector<pair<double, string>> finishTimes; // (time, boyName)

    cout << "\nINDIVIDUAL PERFORMANCE:\n";
    cout << string(50, '-') << "\n";

    for (int i = 0; i < boys.size(); i++) {
        if (routes[i].empty()) continue;

        // Calculate total distance
        double totalDist = 0;
        for (int j = 0; j < routes[i].size()-1; j++) {
            totalDist += distance(routes[i][j], routes[i][j+1]);
        }

        // Calculate time
        double timeHours = totalDist / boys[i].speed;
        double timeMinutes = timeHours * 60;

        finishTimes.push_back({timeMinutes, boys[i].name});

        cout << " " << setw(10) << left << boys[i].name
             << " | Distance: " << setw(8) << fixed << setprecision(2) << totalDist << " km"
             << " | Speed: " << setw(6) << boys[i].speed << " km/h"
             << " | Time: " << setw(8) << fixed << setprecision(1) << timeMinutes << " min\n";
    }

    // Sort by finish time
    sort(finishTimes.begin(), finishTimes.end());

    cout << "\n" << string(50, '=') << "\n";
    cout << " FINAL RANKING \n";
    cout << string(50, '=') << "\n";

    for (int i = 0; i < finishTimes.size(); i++) {
        cout << "   " << (i+1) << ". " << setw(15) << left << finishTimes[i].second
             << " - " << fixed << setprecision(1) << finishTimes[i].first << " minutes\n";
    }

    if (!finishTimes.empty()) {
        cout << "\n" << string(50, '-') << "\n";
        cout << " WINNER: " << finishTimes[0].second
             << " returns to depot first!\n";
    }

    // Calculate time differences
    if (finishTimes.size() > 1) {
        cout << "\nTIME GAPS BETWEEN DELIVERY BOYS:\n";
        for (int i = 1; i < finishTimes.size(); i++) {
            double diff = finishTimes[i].first - finishTimes[0].first;
            cout << "   " << finishTimes[i].second << " is "
                 << fixed << setprecision(1) << diff
                 << " minutes behind " << finishTimes[0].second << "\n";
        }
    }

    cout << "\n" << string(70, '=') << "\n";
}

/*------------------------------
   5. Main Optimization - IMPROVED OUTPUT
-------------------------------*/
void optimizeRoutes(vector<Address> addresses, vector<DeliveryBoy> boys, Depot depot) {
    Address depotAddr = {0, depot.name, depot.x, depot.y};

    int k = boys.size();
    vector<vector<Address>> clusters = kMeansClustering(addresses, k);

    // Store all routes for competition analysis
    vector<vector<Address>> allRoutes(boys.size());

    cout << "\n" << string(70, '=') << "\n";
    cout << "          DELIVERY ROUTE OPTIMIZATION SYSTEM\n";
    cout << string(70, '=') << "\n";

    cout << "\n SYSTEM OVERVIEW:\n";
    cout << "   - Total Addresses: " << addresses.size() << "\n";
    cout << "   - Delivery Boys: " << boys.size() << "\n";
    cout << "   - Depot: " << depot.name << " (" << depot.x << ", " << depot.y << ")\n";

    for (int i = 0; i < boys.size(); i++) {
        cout << "\n" << string(60, '-') << "\n";
        cout << " DELIVERY BOY: " << boys[i].name << "\n";
        cout << string(60, '-') << "\n";

        cout << " Capacity: " << boys[i].capacity << " packages\n";
        cout << " Speed: " << boys[i].speed << " km/h\n";
        cout << " Cluster " << (i+1) << " has " << clusters[i].size()
             << " address(es): ";

        if (clusters[i].empty()) {
            cout << "None assigned\n";
            cout << "Status: No delivery required\n";
            continue;
        }

        for (auto &addr : clusters[i]) cout << addr.name << " ";
        cout << "\n";

        vector<Address> route;

        // Choose algorithm based on cluster size
        if (clusters[i].size() <= 5) {
            route = divideAndConquerRoute(clusters[i], depotAddr);
        } else if (clusters[i].size() <= 10) {
            route = divideAndConquerRoute(clusters[i], depotAddr);
        } else {
            route = greedyRoute(clusters[i], depotAddr);
        }

        // Store route for competition analysis
        allRoutes[i] = route;

        // Calculate total distance
        double totalDist = 0;
        for (int j = 0; j < route.size()-1; j++) {
            totalDist += distance(route[j], route[j+1]);
        }

        // Calculate estimated time
        double estimatedTime = (boys[i].speed > 0) ? (totalDist / boys[i].speed) * 60 : 0;

        // Display optimized route
        cout << "\n OPTIMIZED ROUTE:\n";
        cout << "   ";
        for (int j = 0; j < route.size(); j++) {
            cout << route[j].name;
            if (j < route.size()-1) cout << " -> ";
        }

        cout << "\n\n ROUTE STATISTICS:\n";
        cout << "   - Total Distance: " << fixed << setprecision(2) << totalDist << " km\n";
        cout << "   - Estimated Time: " << fixed << setprecision(1) << estimatedTime << " minutes\n";
        cout << "   - Number of Stops: " << max(0, (int)route.size()-2) << "\n";

        // Visualization
        visualizeRoute(route, boys[i].name, i+1);
    }

    // Competition Analysis
    analyzeCompetition(allRoutes, boys);

    cout << "\n" << string(70, '=') << "\n";
    cout << "          DELIVERY OPTIMIZATION COMPLETED\n";
    cout << string(70, '=') << "\n";
}

/*------------------------------
   Weather Condition System
-------------------------------*/

// Weather condition types
enum WeatherCondition {
    SUNNY,      // Clear weather
    RAINY,      // Rain
    FOGGY,      // Fog/Mist
    STORMY,     // Storm/Heavy rain
    HEATWAVE,   // Extreme heat
    SNOWY       // Snow
};

// Weather effect structure
struct WeatherEffect {
    WeatherCondition condition;
    double speedMultiplier;    // Effect on speed (0.7 = 30% slower)
    double timeMultiplier;     // Effect on delivery time
    string warningMessage;
    string safetyTips;
};

// Weather database
vector<WeatherEffect> weatherDatabase = {
    {SUNNY,     1.0,   1.0,   "Clear weather", "Normal driving conditions"},
    {RAINY,     0.7,   1.4,   "Rainy weather", "Use windshield wipers, maintain safe distance"},
    {FOGGY,     0.6,   1.6,   "Low visibility", "Use fog lights, drive slowly"},
    {STORMY,    0.5,   2.0,   "Storm warning", "Avoid travel if possible, seek shelter"},
    {HEATWAVE,  0.8,   1.2,   "Extreme heat", "Stay hydrated, check vehicle temperature"},
    {SNOWY,     0.4,   2.5,   "Snow/Ice", "Use snow chains, drive very carefully"}
};

class WeatherManager {
private:
    WeatherCondition currentWeather;
    double temperature; // in Celsius
    double humidity;    // percentage
    double windSpeed;   // km/h

public:
    WeatherManager() : currentWeather(SUNNY), temperature(25.0), humidity(60.0), windSpeed(10.0) {}

    // Set current weather
    void setWeather(WeatherCondition weather, double temp, double hum, double wind) {
        currentWeather = weather;
        temperature = temp;
        humidity = hum;
        windSpeed = wind;
    }

    // Get weather effect for current conditions
    WeatherEffect getCurrentWeatherEffect() {
        for (const auto& effect : weatherDatabase) {
            if (effect.condition == currentWeather) {
                return effect;
            }
        }
        return weatherDatabase[0]; // Default to sunny
    }

    // Calculate adjusted speed based on weather
    double getAdjustedSpeed(double baseSpeed) {
        WeatherEffect effect = getCurrentWeatherEffect();
        return baseSpeed * effect.speedMultiplier;
    }

    // Calculate adjusted delivery time
    double getAdjustedTime(double baseTime) {
        WeatherEffect effect = getCurrentWeatherEffect();
        return baseTime * effect.timeMultiplier;
    }

    // Display weather report
    void displayWeatherReport() {
        WeatherEffect effect = getCurrentWeatherEffect();

        cout << "\n" << string(60, '=') << "\n";
        cout << "              CURRENT WEATHER REPORT\n";
        cout << string(60, '=') << "\n";

        cout << "\n WEATHER CONDITIONS:\n";
        cout << "   - Condition: " << effect.warningMessage << "\n";
        cout << "   - Temperature: " << temperature << "C\n";
        cout << "   - Humidity: " << humidity << "%\n";
        cout << "   - Wind Speed: " << windSpeed << " km/h\n";

        cout << "\n DELIVERY IMPACT:\n";
        cout << "   - Speed Multiplier: " << effect.speedMultiplier << "x\n";
        cout << "   - Time Multiplier: " << effect.timeMultiplier << "x\n";

        cout << "\n SAFETY TIPS:\n";
        cout << "   " << effect.safetyTips << "\n";

        // Special warnings
        if (temperature > 35.0) {
            cout << "\n HEAT WARNING: High temperature may affect vehicle performance!\n";
        }
        if (windSpeed > 30.0) {
            cout << "\n WIND WARNING: Strong winds may affect vehicle stability!\n";
        }

        cout << string(60, '=') << "\n";
    }
};

/*------------------------------
   Customer Feedback System
-------------------------------*/

// Review structure
struct Review {
    int id;
    string customerName;
    string customerPhone;
    string addressDelivered;
    string deliveryBoyName;
    int deliveryBoyId;
    int rating;            // 1-5 stars
    string comment;
    string date;
    string time;
    vector<string> tags;   // "on_time", "friendly", "damaged", "late", "excellent"
    bool issueReported;
    string issueDetails;
};

// Feedback management class
class FeedbackManager {
private:
    vector<Review> allReviews;
    map<int, vector<Review>> reviewsByDeliveryBoy; // Boy ID -> Reviews
    map<int, double> averageRatings;              // Boy ID -> Average rating
    map<string, int> tagFrequency;               // Tag -> Count

public:
    // Add new review
    void addReview(const Review& review) {
        allReviews.push_back(review);
        reviewsByDeliveryBoy[review.deliveryBoyId].push_back(review);
        updateStatistics(review);

        cout << "\n REVIEW ADDED SUCCESSFULLY!\n";
        cout << "   Customer: " << review.customerName << "\n";
        cout << "   Rating: " << review.rating << "/5\n";
        if (review.issueReported) {
            cout << "   Issue reported: " << review.issueDetails << "\n";
        }
    }

    // Update statistics
    void updateStatistics(const Review& review) {
        // Update average rating for delivery boy
        int boyId = review.deliveryBoyId;
        vector<Review>& boyReviews = reviewsByDeliveryBoy[boyId];

        double sum = 0;
        for (const auto& rev : boyReviews) {
            sum += rev.rating;
        }
        averageRatings[boyId] = sum / boyReviews.size();

        // Update tag frequency
        for (const auto& tag : review.tags) {
            tagFrequency[tag]++;
        }
    }

    // Get delivery boy's average rating
    double getBoyRating(int boyId) {
        if (averageRatings.find(boyId) != averageRatings.end()) {
            return averageRatings[boyId];
        }
        return 0.0;
    }

    // Get delivery boy's performance summary
    void getBoyPerformance(int boyId, const string& boyName) {
        if (reviewsByDeliveryBoy.find(boyId) == reviewsByDeliveryBoy.end()) {
            cout << "\nNo reviews found for " << boyName << "\n";
            return;
        }

        const vector<Review>& reviews = reviewsByDeliveryBoy[boyId];
        int totalReviews = reviews.size();
        int totalRating = 0;
        int onTimeCount = 0;
        int issueCount = 0;

        for (const auto& review : reviews) {
            totalRating += review.rating;
            if (find(review.tags.begin(), review.tags.end(), "on_time") != review.tags.end()) {
                onTimeCount++;
            }
            if (review.issueReported) {
                issueCount++;
            }
        }

        double avgRating = (double)totalRating / totalReviews;
        double onTimePercentage = (double)onTimeCount / totalReviews * 100;

        cout << "\n" << string(60, '=') << "\n";
        cout << "       PERFORMANCE REPORT - " << boyName << "\n";
        cout << string(60, '=') << "\n";

        cout << "\n STATISTICS:\n";
        cout << "   - Total Deliveries Reviewed: " << totalReviews << "\n";
        cout << "   - Average Rating: " << fixed << setprecision(1) << avgRating << "/5.0\n";
        cout << "   - On-time Delivery Rate: " << fixed << setprecision(1) << onTimePercentage << "%\n";
        cout << "   - Issues Reported: " << issueCount << "\n";

        // Rating breakdown
        map<int, int> ratingCount;
        for (const auto& review : reviews) {
            ratingCount[review.rating]++;
        }

        cout << "\n RATING DISTRIBUTION:\n";
        for (int i = 5; i >= 1; i--) {
            int count = ratingCount[i];
            double percentage = (double)count / totalReviews * 100;
            cout << "   " << i << " stars: " << string(count, '*')
                 << " " << count << " reviews (" << fixed << setprecision(1) << percentage << "%)\n";
        }

        // Recent feedback
        cout << "\n RECENT FEEDBACK:\n";
        int showCount = min(3, totalReviews);
        for (int i = 0; i < showCount; i++) {
            const Review& r = reviews[i];
            cout << "   " << r.rating << "/5 - " << r.customerName
                 << " (" << r.date << ")\n";
            if (!r.comment.empty()) {
                cout << "     \"" << r.comment << "\"\n";
            }
            if (r.issueReported) {
                cout << "     Issue: " << r.issueDetails << "\n";
            }
        }

        cout << string(60, '=') << "\n";
    }

    // Show overall feedback summary
    void showOverallSummary() {
        cout << "\n" << string(70, '=') << "\n";
        cout << "          OVERALL CUSTOMER FEEDBACK SUMMARY\n";
        cout << string(70, '=') << "\n";

        if (allReviews.empty()) {
            cout << "\nNo customer feedback available yet.\n";
            return;
        }

        // Overall statistics
        int totalReviews = allReviews.size();
        double totalRating = 0;
        int excellentCount = 0;
        int poorCount = 0;

        for (const auto& review : allReviews) {
            totalRating += review.rating;
            if (review.rating >= 4) excellentCount++;
            if (review.rating <= 2) poorCount++;
        }

        double overallRating = totalRating / totalReviews;
        double excellentPercentage = (double)excellentCount / totalReviews * 100;
        double poorPercentage = (double)poorCount / totalReviews * 100;

        cout << "\n OVERALL PERFORMANCE:\n";
        cout << "   - Total Reviews: " << totalReviews << "\n";
        cout << "   - Overall Rating: " << fixed << setprecision(2) << overallRating << "/5.0\n";
        cout << "   - Excellent Reviews (4-5 stars): " << excellentPercentage << "%\n";
        cout << "   - Poor Reviews (1-2 stars): " << poorPercentage << "%\n";

        // Most common tags
        cout << "\n FREQUENT FEEDBACK TAGS:\n";
        vector<pair<string, int>> tagsSorted(tagFrequency.begin(), tagFrequency.end());
        sort(tagsSorted.begin(), tagsSorted.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });

        int tagCount = min(5, (int)tagsSorted.size());
        for (int i = 0; i < tagCount; i++) {
            double percentage = (double)tagsSorted[i].second / totalReviews * 100;
            cout << "   - " << setw(15) << left << tagsSorted[i].first
                 << ": " << tagsSorted[i].second << " times ("
                 << fixed << setprecision(1) << percentage << "%)\n";
        }

        // Top performing delivery boys
        cout << "\n TOP PERFORMERS:\n";
        vector<pair<int, double>> boysSorted;
        for (const auto& entry : averageRatings) {
            if (reviewsByDeliveryBoy[entry.first].size() >= 3) { // At least 3 reviews
                boysSorted.push_back({entry.first, entry.second});
            }
        }

        sort(boysSorted.begin(), boysSorted.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });

        int topCount = min(3, (int)boysSorted.size());
        for (int i = 0; i < topCount; i++) {
            int boyId = boysSorted[i].first;
            int reviewCount = reviewsByDeliveryBoy[boyId].size();
            cout << "   " << (i+1) << ". Boy ID " << boyId
                 << " - Rating: " << fixed << setprecision(2) << boysSorted[i].second
                 << " (" << reviewCount << " reviews)\n";
        }

        // Recent reviews
        cout << "\n RECENT CUSTOMER REVIEWS:\n";
        int recentCount = min(3, totalReviews);
        for (int i = 0; i < recentCount; i++) {
            const Review& r = allReviews[totalReviews - 1 - i];
            cout << "   " << r.rating << "/5 - " << r.customerName
                 << " for " << r.deliveryBoyName << "\n";
            cout << "     \"" << (r.comment.length() > 50 ? r.comment.substr(0, 50) + "..." : r.comment) << "\"\n";
        }

        // Issues summary
        int totalIssues = 0;
        for (const auto& review : allReviews) {
            if (review.issueReported) totalIssues++;
        }

        if (totalIssues > 0) {
            cout << "\n ISSUES REPORTED: " << totalIssues << " issues need attention\n";
        }

        cout << string(70, '=') << "\n";
    }

    // Get reviews by rating range
    vector<Review> getReviewsByRating(int minStars, int maxStars) {
        vector<Review> result;
        for (const auto& review : allReviews) {
            if (review.rating >= minStars && review.rating <= maxStars) {
                result.push_back(review);
            }
        }
        return result;
    }

    // Get reviews with issues
    vector<Review> getReviewsWithIssues() {
        vector<Review> result;
        for (const auto& review : allReviews) {
            if (review.issueReported) {
                result.push_back(review);
            }
        }
        return result;
    }

    // Clear all reviews (for testing/reset)
    void clearAllReviews() {
        allReviews.clear();
        reviewsByDeliveryBoy.clear();
        averageRatings.clear();
        tagFrequency.clear();
        cout << "\nAll feedback data has been cleared.\n";
    }
};

// Function to collect customer feedback
Review collectCustomerFeedback() {
    Review newReview;
    static int reviewId = 1;

    newReview.id = reviewId++;

    cout << "\n" << string(50, '=') << "\n";
    cout << "       CUSTOMER FEEDBACK FORM\n";
    cout << string(50, '=') << "\n";

    cin.ignore();
    cout << "\nCustomer Name: ";
    getline(cin, newReview.customerName);

    cout << "Phone Number: ";
    getline(cin, newReview.customerPhone);

    cout << "Delivery Address: ";
    getline(cin, newReview.addressDelivered);

    cout << "Delivery Boy Name: ";
    getline(cin, newReview.deliveryBoyName);

    cout << "Delivery Boy ID: ";
    cin >> newReview.deliveryBoyId;

    cout << "\nRating (1-5 stars): ";
    cin >> newReview.rating;

    cin.ignore();
    cout << "Comments (optional): ";
    getline(cin, newReview.comment);

    // Add tags based on rating
    if (newReview.rating >= 4) {
        newReview.tags.push_back("excellent");
        newReview.tags.push_back("recommended");
    } else if (newReview.rating <= 2) {
        newReview.tags.push_back("needs_improvement");
    }

    if (newReview.rating == 5) {
        newReview.tags.push_back("outstanding");
    }

    cout << "\nWas there any issue with the delivery? (1=Yes, 0=No): ";
    cin >> newReview.issueReported;

    if (newReview.issueReported) {
        cin.ignore();
        cout << "Please describe the issue: ";
        getline(cin, newReview.issueDetails);
        newReview.tags.push_back("issue_reported");
    }

    // Get current date and time
    time_t now = time(0);
    tm* localTime = localtime(&now);

    char dateBuffer[11];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", localTime);
    newReview.date = dateBuffer;

    char timeBuffer[9];
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", localTime);
    newReview.time = timeBuffer;

    return newReview;
}

/*------------------------------
   Main Function
-------------------------------*/
int main() {
    cout << "\n" << string(70, '=') << "\n";
    cout << "     WELCOME TO DELIVERY ROUTE OPTIMIZATION SYSTEM\n";
    cout << string(70, '=') << "\n\n";

    // Create managers
    WeatherManager weatherManager;
    FeedbackManager feedbackManager;

    // WEATHER SETUP SECTION
    cout << "\n" << string(60, '=') << "\n";
    cout << "           WEATHER INFORMATION\n";
    cout << string(60, '=') << "\n";

    int weatherChoice;
    cout << "\nSelect Current Weather Condition:\n";
    cout << "1. Sunny/Clear\n";
    cout << "2. Rainy\n";
    cout << "3. Foggy\n";
    cout << "4. Stormy\n";
    cout << "5. Heatwave\n";
    cout << "6. Snowy\n";
    cout << "Enter choice (1-6): ";
    cin >> weatherChoice;

    double temperature, humidity, windSpeed;
    cout << "\nEnter Temperature (C): ";
    cin >> temperature;
    cout << "Enter Humidity (%): ";
    cin >> humidity;
    cout << "Enter Wind Speed (km/h): ";
    cin >> windSpeed;

    // Map choice to weather condition
    WeatherCondition selectedWeather;
    switch(weatherChoice) {
        case 1: selectedWeather = SUNNY; break;
        case 2: selectedWeather = RAINY; break;
        case 3: selectedWeather = FOGGY; break;
        case 4: selectedWeather = STORMY; break;
        case 5: selectedWeather = HEATWAVE; break;
        case 6: selectedWeather = SNOWY; break;
        default: selectedWeather = SUNNY;
    }

    weatherManager.setWeather(selectedWeather, temperature, humidity, windSpeed);
    weatherManager.displayWeatherReport();

    // GET DEPOT INFORMATION
    Depot depot;
    cin.ignore(); // Clear newline

    cout << "\n" << string(60, '=') << "\n";
    cout << "           DEPOT INFORMATION\n";
    cout << string(60, '=') << "\n";

    cout << "\nEnter Depot Name: ";
    getline(cin, depot.name);

    cout << "Enter Depot X Coordinate: ";
    if (!(cin >> depot.x)) {
        cout << " Invalid X coordinate!\n";
        return 1;
    }

    cout << "Enter Depot Y Coordinate: ";
    if (!(cin >> depot.y)) {
        cout << " Invalid Y coordinate!\n";
        return 1;
    }
    cin.ignore();

    // GET ADDRESSES
    int numAddr;
    cout << "\n" << string(60, '=') << "\n";
    cout << "           ADDRESS INFORMATION\n";
    cout << string(60, '=') << "\n";

    cout << "\nNumber of Delivery Addresses: ";
    if (!(cin >> numAddr) || numAddr < 0) {
        cout << " Invalid number of addresses!\n";
        return 1;
    }
    cin.ignore();

    vector<Address> addresses(numAddr);
    for (int i = 0; i < numAddr; i++) {
        addresses[i].id = i+1;

        cout << "\n   Address #" << i+1 << ":\n";
        cout << "     Name: ";
        getline(cin, addresses[i].name);

        cout << "     X Coordinate: ";
        if (!(cin >> addresses[i].x)) {
            cout << " Invalid X coordinate!\n";
            return 1;
        }

        cout << "     Y Coordinate: ";
        if (!(cin >> addresses[i].y)) {
            cout << " Invalid Y coordinate!\n";
            return 1;
        }
        cin.ignore();
    }

    // GET DELIVERY BOYS
    int numBoys;
    cout << "\n" << string(60, '=') << "\n";
    cout << "           DELIVERY PERSONNEL\n";
    cout << string(60, '=') << "\n";

    cout << "\nNumber of Delivery Persons: ";
    if (!(cin >> numBoys) || numBoys < 0) {
        cout << " Invalid number of delivery persons!\n";
        return 1;
    }
    cin.ignore();

    vector<DeliveryBoy> boys(numBoys);
    for (int i = 0; i < numBoys; i++) {
        boys[i].id = i+1;

        cout << "\n   Delivery Person #" << i+1 << ":\n";
        cout << "     Name: ";
        getline(cin, boys[i].name);

        cout << "     Speed (km/h): ";
        if (!(cin >> boys[i].speed) || boys[i].speed <= 0) {
            cout << " Invalid speed!\n";
            return 1;
        }

        cout << "     Capacity (packages): ";
        if (!(cin >> boys[i].capacity) || boys[i].capacity <= 0) {
            cout << " Invalid capacity!\n";
            return 1;
        }
        cin.ignore();
    }

    // OPTIMIZE ROUTES
    cout << "\n" << string(70, '=') << "\n";
    cout << "        STARTING ROUTE OPTIMIZATION\n";
    cout << string(70, '=') << "\n";

    optimizeRoutes(addresses, boys, depot);

    // WEATHER ADJUSTED RESULTS
    cout << "\n" << string(70, '=') << "\n";
    cout << "        WEATHER-ADJUSTED ANALYSIS\n";
    cout << string(70, '=') << "\n";

    // Recalculate with weather effects
    cout << "\nDelivery times adjusted for weather conditions:\n";
    for (int i = 0; i < boys.size(); i++) {
        double avgDistance = 10.0;
        double baseTime = (avgDistance / boys[i].speed) * 60;
        double adjustedTime = weatherManager.getAdjustedTime(baseTime);

        cout << "   " << boys[i].name << ": "
             << fixed << setprecision(1) << baseTime << " min -> "
             << adjustedTime << " min ("
             << fixed << setprecision(0) << ((adjustedTime/baseTime)-1)*100 << "% change)\n";
    }

    // CUSTOMER FEEDBACK MENU
    int choice;
    do {
        cout << "\n" << string(70, '=') << "\n";
        cout << "           CUSTOMER FEEDBACK SYSTEM\n";
        cout << string(70, '=') << "\n";

        cout << "\n1. Submit Customer Feedback\n";
        cout << "2. View Delivery Boy Performance\n";
        cout << "3. Show Overall Summary\n";  // Changed from "Exit"
        cout << "4. Exit\n";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1: {
                Review feedback = collectCustomerFeedback();
                feedbackManager.addReview(feedback);
                break;
            }
            case 2: {
                int boyId;
                string boyName;
                cout << "\nEnter Delivery Boy ID: ";
                cin >> boyId;
                cout << "Enter Delivery Boy Name: ";
                cin.ignore();
                getline(cin, boyName);
                feedbackManager.getBoyPerformance(boyId, boyName);
                break;
            }
            case 3: {
                feedbackManager.showOverallSummary();
                break;
            }
            case 4: {
                cout << "\nThank you for using the system!\n";
                break;
            }
            default: {
                cout << "\nInvalid choice! Please try again.\n";
            }
        }
    } while (choice != 4);

    // FINAL SUMMARY
    cout << "\n" << string(70, '=') << "\n";
    cout << "             SYSTEM SUMMARY REPORT\n";
    cout << string(70, '=') << "\n";

    cout << "\n DELIVERY OPERATION COMPLETED\n";
    cout << "   - Total Addresses: " << addresses.size() << "\n";
    cout << "   - Delivery Personnel: " << boys.size() << "\n";
    cout << "   - Depot: " << depot.name << "\n";
    cout << "   - Weather Condition: ";
    switch(selectedWeather) {
        case SUNNY: cout << "Sunny"; break;
        case RAINY: cout << "Rainy"; break;
        case FOGGY: cout << "Foggy"; break;
        case STORMY: cout << "Stormy"; break;
        case HEATWAVE: cout << "Heatwave"; break;
        case SNOWY: cout << "Snowy"; break;
    }
    cout << "\n";

    // Show final weather impact
    WeatherEffect effect = weatherManager.getCurrentWeatherEffect();
    cout << "   - Weather Impact: " << fixed << setprecision(0)
         << (effect.timeMultiplier - 1.0) * 100 << "% time increase\n";

    cout << "\n SYSTEM READY FOR NEXT DELIVERY CYCLE\n";
    cout << string(70, '=') << "\n";

    return 0;
}
