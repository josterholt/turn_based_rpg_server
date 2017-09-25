#include <string>
#include <array>
#include <vector>
typedef std::array<float, 2> point_t;
typedef std::vector<point_t> xy_points_t;


void gen_random(char *s, const int len);
std::string base64_decode(std::string &ifstream);
bool intersects(xy_points_t points1, xy_points_t points2);
point_t rotatePoint(point_t point, float rotation);
point_t rotatePoint(float x, float y, float rotation);