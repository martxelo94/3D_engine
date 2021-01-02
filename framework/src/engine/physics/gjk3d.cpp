#include <pch.h>

/**
*
* @brief find farthest point on Minkowski difference over a direction
* @param dir
* @param convex_hull_a
* @param convex_hull_b
* @return support point of Minkowski difference
*/
vec3 support(const vec3 &dir, const std::vector<vec3>& convex_hull_a, const std::vector<vec3>& convex_hull_b)
{
	float max_dot = -FLT_MAX;
	// find farthest point on A
	size_t idx_a = 0;
	for (int i = 0; i < convex_hull_a.size(); ++i) {
		float d = glm::dot(dir, convex_hull_a[i]);
		if (d > max_dot) {
			idx_a = i;
			max_dot = d;
		}
	}
	max_dot = -FLT_MAX;
	// find farthest point on B
	size_t idx_b = 0;
	for (int i = 0; i < convex_hull_b.size(); ++i) {
		float d = glm::dot(-dir, convex_hull_b[i]);
		if (d > max_dot) {
			idx_b = i;
			max_dot = d;
		}
	}
	return convex_hull_a[idx_a] - convex_hull_b[idx_b];
}

/**
*
* @brief perform GJK algorithm in 3D context
* @param convex_hull_a
* @param convex_hull_b
* @param simplices
* @return true if two hulls collide
*/
bool gjk3d(const std::vector<vec3>& convex_hull_a, const std::vector<vec3>& convex_hull_b, std::vector<gjk_simplex>& simplices)
{
	vec3 s = support(vec3{-1}, convex_hull_a, convex_hull_b);
	gjk_simplex simplex;
	simplex.add_point(s);
	vec3 dir = -s;
	simplices.clear();
	
	//for (int it = 0; it < 10000; ++it) // avoid infinite loops...
	while(1) // I'm not in danger, I AM THE DANGER
	{
		vec3 a = support(dir, convex_hull_a, convex_hull_b);
		// if 0, on origin
		if (glm::length2(a) == 0.f)
			return true;
		// if behind, can't reach origin
		if (glm::dot(a, dir) < 0.f)
			return false;
		simplex.add_point(a);
		// Do Simplex -> update simplex and search direction
		simplices.push_back(simplex);
		// line case:
		if (simplex.point_count() == 2) {
			vec2 coord;
			get_barycentric_coordinates(simplex.points()[1], simplex.points()[0], {}, &coord);
			if (coord[1] > 0.f) {
				vec3 AB = simplex.points()[0] - simplex.points()[1];
				dir = glm::cross(glm::cross(AB, -simplex.points()[1]), AB);
			}
			else {
				assert(coord[0] < 1.f);
				dir = -simplex.points()[1];
				// delete B
				simplex.reduce();
			}
		}
		// triangle case
		else if (simplex.point_count() == 3) {
			Triangle tri{ simplex.points()[0],simplex.points()[1] ,simplex.points()[2] };
			vec3 coord;
			if (get_barycentric_coordinates(tri, {}, &coord)) {
				return true;
			}
			else {
				// find direction
				if (glm::epsilonEqual(coord[2], 0.f, cEpsilon * 2) && coord[0] > 0.f && coord[1] > 0.f) {
					// over BC (shoul not happen ... or yes?)
					//assert(0);
					return true;
				}
				if (coord[2] >= 1.f && coord[1] + coord[0] <= 0.f) {
					// A is closest
					dir = -tri.p2;
					simplex.reduce();
					simplex.reduce();
				}
				else if (coord[2] >= 0.f && coord[0] > 0.f && coord[1] < 0.f) {
					// AC is closest
					vec3 AC = tri.p0 - tri.p2;
					dir = glm::cross(AC, glm::cross(-tri.p2, AC));
					{
						// dir opposite to AB
						vec3 AB = tri.p1 - tri.p2;
						float d = glm::dot(AB, dir);
						assert(d < 0.f);
					}
					// remove B
					simplex.clear();
					simplex.add_point(tri.p0); simplex.add_point(tri.p2);
				}
				else if (coord[2] >= 0.f && coord[1] > 0.f && coord[0] < 0.f) {
					// AB is closest
					vec3 AB = tri.p1 - tri.p2;
					dir = glm::cross(glm::cross(AB, -tri.p2), AB);
					{
						// dir opposite to AC
						vec3 AC = tri.p0 - tri.p2;
						float d = glm::dot(AC, dir);
						assert(d < 0.f);
					}
					// remove C
					simplex.reduce();
				}
				else if (coord[2] < 0.f) {
					// origin passed... fail or crash? that is the question
					return false;
				}
				else {
					// inside ABC
					assert(coord[0] >= 0.f && coord[1] >= 0.f && coord[2] >= 0.f);
					Plane pl{tri};
					if (glm::dot(pl.normal, -tri.p0) > 0.f) {
						dir = pl.normal;
					}
					else {
						dir = -pl.normal;
						simplex.clear();
						simplex.add_point(tri.p1);
						simplex.add_point(tri.p0);
						simplex.add_point(tri.p2);
					}
				}
			}
		}
		// tetrahedron case
		else if (simplex.point_count() == 4) {
			vec4 coord;
			std::array<vec3, 4> tetrahedron = simplex.points();
			if (get_barycentric_coordinates(simplex.points(), {}, &coord)) {
				// origin inside!
				return true;
			}
				// find next direction...
			else {
				//  triangle cases
				if (coord[0] < 0.f && coord[1] > 0.f && coord[2] > 0.f && coord[3] > 0.f) {
					// ABC triangle
					Plane pl{ tetrahedron[1], tetrahedron[2], tetrahedron[3] };
					{
						vec3 AD = tetrahedron[0] - tetrahedron[3];
						float d = glm::dot(AD, pl.normal);
						assert(d < 0.f);
					}
					dir = pl.normal;
					// remove D
					simplex.reduce();
				}
				else if (coord[1] < 0.f && coord[0] > 0.f && coord[2] > 0.f && coord[3] > 0.f) {
					// ABD triangle
					Plane pl{ tetrahedron[2], tetrahedron[0], tetrahedron[3] };
					{
						vec3 AC = tetrahedron[1] - tetrahedron[3];
						float d = glm::dot(AC, pl.normal);
						assert(d < 0.f);
					}
					dir = pl.normal;
					// remove C
					simplex.clear();
					simplex.add_point(tetrahedron[2]);
					simplex.add_point(tetrahedron[0]);
					simplex.add_point(tetrahedron[3]);
				}
				else if (coord[2] < 0.f && coord[1] > 0.f && coord[0] > 0.f && coord[3] > 0.f) {
					// ACD triangle
					Plane pl{ tetrahedron[0], tetrahedron[1], tetrahedron[3] };
					{
						vec3 AB = tetrahedron[2] - tetrahedron[3];
						float d = glm::dot(AB, pl.normal);
						assert(d < 0.f);
					}
					dir = pl.normal;
					// remove B
					simplex.clear();
					simplex.add_point(tetrahedron[0]);
					simplex.add_point(tetrahedron[1]);
					simplex.add_point(tetrahedron[3]);
				}
#if 0
				else if (coord[3] < 0.f && coord[1] > 0.f && coord[2] > 0.f && coord[0] > 0.f) {
					assert(0);	// should not happen
				}
				// point cases
				else if (coord[0] > 1.f && (coord[1] + coord[2] + coord[3]) < 0) {
					assert(0);	// should not happen
				}
				else if (coord[1] > 1.f && (coord[0] + coord[2] + coord[3]) < 0) {
					assert(0);	// should not happen
				}
				else if (coord[2] > 1.f && (coord[1] + coord[0] + coord[3]) < 0) {
					assert(0);	// should not happen
				}
#endif
				else if (coord[3] >= 1.f && (coord[1] + coord[2] + coord[0]) < 0) {
					// only point A
					simplex.clear();
					simplex.add_point(tetrahedron[3]);
					dir = -tetrahedron[3];
				}
				// edge cases, 6 more OMG... (but discard edges from the base)
				else if (coord[3] > 0.f) {
					if (coord[0] > 0.f) {
						vec3 AD = tetrahedron[3] - tetrahedron[0];
						vec3 BD = tetrahedron[2] - tetrahedron[0];
						vec3 CD = tetrahedron[1] - tetrahedron[0];
						vec3 ABD = glm::cross(AD, BD);
						vec3 ACD = glm::cross(CD, AD);
						{
							// check normals outside
							vec3 AC = tetrahedron[1] - tetrahedron[3];
							vec3 AB = tetrahedron[2] - tetrahedron[3];
							float d = glm::dot(AC, ABD);
							assert(d <= 0.f);
							d = glm::dot(AB, ACD);
							assert(d <= 0.f);
						}
						dir = ABD + ACD;
						simplex.clear();
						simplex.add_point(tetrahedron[0]);
						simplex.add_point(tetrahedron[3]);
					}
					else if (coord[1] > 0.f) {
						vec3 AC = tetrahedron[3] - tetrahedron[1];
						vec3 DC = tetrahedron[0] - tetrahedron[1];
						vec3 BC = tetrahedron[2] - tetrahedron[1];
						vec3 ACD = glm::cross(AC, DC);
						vec3 ABC = glm::cross(BC, AC);
						{
							// check normals outside
							vec3 AD = tetrahedron[0] - tetrahedron[3];
							vec3 AB = tetrahedron[2] - tetrahedron[3];
							float d = glm::dot(AD, ABC);
							assert(d <= 0.f);
							d = glm::dot(AB, ACD);
							assert(d <= 0.f);
						}
						dir = ACD + ABC;
						simplex.clear();
						simplex.add_point(tetrahedron[1]);
						simplex.add_point(tetrahedron[3]);
					}
					else if (coord[2] > 0.f) {
						vec3 AB = tetrahedron[3] - tetrahedron[2];
						vec3 CB = tetrahedron[1] - tetrahedron[2];
						vec3 DB = tetrahedron[0] - tetrahedron[2];
						vec3 ABC = glm::cross(AB, CB);
						vec3 ABD = glm::cross(DB, AB);
						{
							// check normals outside
							vec3 AD = tetrahedron[0] - tetrahedron[3];
							vec3 AC = tetrahedron[1] - tetrahedron[3];
							float d = glm::dot(AD, ABC);
							assert(d <= 0.f);
							d = glm::dot(AC, ABD);
							assert(d <= 0.f);
						}
						dir = ABC + ABD;
						simplex.clear();
						simplex.add_point(tetrahedron[2]);
						simplex.add_point(tetrahedron[3]);
					}
					else {
						// when? causes adding a point to tetrahedron...
						assert(0);
					}
				}
				else {
					// when? causes adding a point to tetrahedron...
					assert(0);
				}
			}
		}
	}

	return false;
}

/**
*
* @brief compute barycentric coordinates of point in tetrahedron
* @param tetrahedron
* @param p
* @param barycentric_coord
* @return true if point inside tetrahedron
*/
bool get_barycentric_coordinates(const std::array<vec3, 4> &tetrahedron, const vec3 &p, vec4 *barycentric_coord)
{
	// credit to https://www.iue.tuwien.ac.at/phd/nentchev/node31.html

	// convert to vec4 to use in mat4
	vec4 p0 = vec4{tetrahedron[0], 1.f};
	vec4 p1 = vec4{ tetrahedron[1], 1.f };
	vec4 p2 = vec4{ tetrahedron[2], 1.f };
	vec4 p3 = vec4{ tetrahedron[3], 1.f };
	vec4 _p = vec4{ p,1.f };
	// compute determinants to opposing faces to compute coefficients
	float det = glm::determinant(mat4{ p0, p1, p2, p3 });
	if (det == 0.f)
		return false;
	float det0 = glm::determinant(mat4{ _p, p1, p2, p3 });
	float det1 = glm::determinant(mat4{ p0, _p, p2, p3 });
	float det2 = glm::determinant(mat4{ p0, p1, _p, p3 });
	float det3 = glm::determinant(mat4{ p0, p1, p2, _p });
	// ratio between determinants and volume (det) is the coordinate
	float a = det0 / det;
	float b = det1 / det;
	float c = det2 / det;
	float d = det3 / det;
	// store coordinate results
	if (barycentric_coord) {
		*barycentric_coord = vec4{ a, b, c, d };
	}
	
	// return true if p inside
	return (a >= 0.f && a <= 1.f) && (b >= 0.f && b <= 1.f) && (c >= 0.f && c <= 1.f) && (d >= 0.f && d <= 1.f);
}
