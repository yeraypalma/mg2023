// -*-C++-*-

#pragma once

struct Constants {
	static const float pi;               // pi value
	static const float distance_epsilon; // Minimum distance
	static const float angle_epsilon;    // Minimun angle

	static const float degree_to_rad;
	static const float rad_to_degree;

	struct gl_texunits {
		static const int texture;    // Texture unit 0
		static const int bump;       // Texture unit 1

		static const int specular;   // Texture unit 2
		static const int projective; // Texture unit 3
		static const int shadow;     // Texture unit 4
		static const int envmap;     // Texture unit 5
		static const int diffuseMap;  // Texture unit 6
		static const int normalMap;  // Texture unit 7
		static const int metallicMap;  // Texture unit 8
		static const int roughnessMap;  // Texture unit 9
		static const int rest;       // Texture unit 10
	};
};
