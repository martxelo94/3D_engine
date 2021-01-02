#include <pch.h>

/**
*
* @brief push back
* @param point
*/
void gjk_simplex::add_point(const vec3& point)
{
	// let's assume points are always in order
	m_simplex[m_simplex_active.count()] = point;
	m_simplex_active.set(m_simplex_active.count(), 1);
#if 0
	// find free point slot
	for (int i = 0; i < 4; ++i) {
		if (m_simplex_active[i] == 0) {
			m_simplex[i] = point;
			m_simplex_active.set(i, 1);
			break;
		}
	}
#endif
}
/**
*
* @brief pop front
*/
void gjk_simplex::reduce()
{
	// pop front and shift
	for (int i = 0; i < m_simplex_active.count() - 1; ++i) {
		m_simplex[i] = m_simplex[i + 1];
	}
	m_simplex_active.set(m_simplex_active.count() - 1, 0);
}

/**
*
* @brief remove all points reseting m_simplex_active bitset
*/
void gjk_simplex::clear()
{
	m_simplex_active.reset();
}

