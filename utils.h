#include <string>
#include <array>
#include <vector>

typedef std::vector<std::array<float, 2>> xy_points_t;


void gen_random(char *s, const int len);
std::string base64_decode(std::string &ifstream);
bool intersects(xy_points_t points1, xy_points_t points2);