#include "utils.h"
#include <random>

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
	for (xy_points_t::iterator points1_it = points1.begin(); points1_it != points1.end(); points1_it++) {
		float mob1_x = (*points1_it)[0];
		float mob1_y = (*points1_it)[1];

		for (xy_points_t::iterator points2_it = points2.begin(); points2_it != points2.end(); points2_it++) {
			float hitbox1_x = (*points2_it)[0];
			float hitbox1_y = (*points2_it)[1];

			float hitbox2_x;
			float hitbox2_y;

			if (points2_it + 1 == points2.end()) {
				hitbox2_x = (*points2.begin())[0];
				hitbox2_y = (*points2.begin())[1];
			}
			else {
				hitbox2_x = (*(points2_it + 1))[0];
				hitbox2_y = (*(points2_it + 1))[1];
			}

			bool x_intersects = false;
			bool y_intersects = false;

			if (hitbox1_x < hitbox2_x) {
				if (mob1_x >= hitbox1_x && mob1_x <= hitbox2_x) {
					x_intersects = true;
				}

			}
			else {
				if (mob1_x <= hitbox1_x && mob1_x >= hitbox2_x) {
					x_intersects = true;
				}
			}

			if (hitbox1_y < hitbox2_y) {
				if (mob1_y >= hitbox1_y && mob1_y <= hitbox2_y) {
					y_intersects = true;
				}
			}
			else {
				if (mob1_y <= hitbox1_y && mob1_y >= hitbox2_y) {
					y_intersects = true;
				}
			}

			if (x_intersects && y_intersects) {
				return true;
			}

		}
	}
	return false;
}