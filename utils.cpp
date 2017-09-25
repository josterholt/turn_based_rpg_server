#include "utils.h"
#include <random>
#include "math.h"
#include <iostream>

void gen_random(char *s, const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

static const std::string base64_chars =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz"
	"0123456789+/";

static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(std::string &encoded_string) {
	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	std::string ret;

	while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

bool intersects(xy_points_t points1, xy_points_t points2) {
	std::cout << "<======================================>\n";
	for (xy_points_t::iterator points1_it = points1.begin(); points1_it != points1.end(); points1_it++) {
		point_t mob_point = (*points1_it);
		float mob1_x = mob_point[0];
		float mob1_y = mob_point[1];

		std::cout << "///////////////// New Point /////////////////\n";
		for (xy_points_t::iterator points2_it = points2.begin(); points2_it != points2.end(); points2_it++) {
			point_t hitbox_point = *points2_it;
			float hitbox1_x = hitbox_point[0];
			float hitbox1_y = hitbox_point[1];

			float hitbox2_x;
			float hitbox2_y;

			xy_points_t::iterator points2_it2 = std::next(points2_it);

			if (points2_it2 == points2.end()) {
				hitbox2_x = (*points2.begin())[0];
				hitbox2_y = (*points2.begin())[1];
			}
			else {
				hitbox2_x = (*points2_it2)[0];
				hitbox2_y = (*points2_it2)[1];
			}

			bool x_intersects = false;
			bool y_intersects = false;

			if (hitbox1_x < hitbox2_x) {
				std::cout << mob1_x << ">= " << hitbox1_x << " && " << mob1_x << " <= " << hitbox2_x << "\n";
				if (mob1_x >= hitbox1_x && mob1_x <= hitbox2_x) {
					x_intersects = true;
				}

			}
			else {
				std::cout << mob1_x << " <= " << hitbox1_x << " && " << mob1_x << " >= " << hitbox2_x << "\n";
				if (mob1_x <= hitbox1_x && mob1_x >= hitbox2_x) {
					x_intersects = true;
				}
			}

			if (hitbox1_y < hitbox2_y) {
				std::cout << mob1_y << " >= " << hitbox1_y << " && " << mob1_y << " <= " << hitbox2_y << "\n";
				if (mob1_y >= hitbox1_y && mob1_y <= hitbox2_y) {
					y_intersects = true;
				}
			}
			else {
				std::cout << mob1_y << " >= " << hitbox1_y << " && " << mob1_y << " <= " << hitbox2_y << "\n";
				if (mob1_y <= hitbox1_y && mob1_y >= hitbox2_y) {
					y_intersects = true;
				}
			}

			if (x_intersects && y_intersects) {
				std::cout << "<===================INTERSECTS===================>\n";
				return true;
			}

		}
	}
	std::cout << "<======================================>\n";
	return false;
}

point_t rotatePoint(point_t point, float rotation) {
	point_t new_point;
	new_point[0] = round(point[0] * cos(rotation) - point[1] * sin(rotation));
	new_point[1] = round(point[1] * cos(rotation) + point[0] * sin(rotation));
	return new_point;
}

point_t rotatePoint(float x, float y, float rotation) {
	point_t new_point;
	new_point[0] = round(x * cos(rotation) - y * sin(rotation));
	new_point[1] = round(y * cos(rotation) + x * sin(rotation));
	return new_point;
}