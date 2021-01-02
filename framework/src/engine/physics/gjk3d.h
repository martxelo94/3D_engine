#ifndef GJK3D_H
#define GJK3D_H

bool gjk3d(const std::vector<vec3>& convex_hull_a, const std::vector<vec3>& convex_hull_b, std::vector<gjk_simplex>& simplices);

// Helper functions
size_t closest_point_to_segment(const std::vector<vec3> &points, vec3 segment_s, vec3 segment_e);
size_t closest_point_to_triangle(const std::vector<vec3> &points, const Triangle& tri);
size_t closest_point_to_tetrahedron(const std::vector<vec3> &points, const std::array<vec3, 4>& tetrahedron);

bool get_barycentric_coordinates(const std::array<vec3, 4> &tetrahedron, const vec3 &p, vec4 *barycentric_coord);

#endif
