#ifndef GJK_SIMPLEX_H
#define GJK_SIMPLEX_H

class gjk_simplex
{
  private:
    std::array<vec3, 4> m_simplex{};
    std::bitset<4>      m_simplex_active{};

  public:
    void add_point(const vec3& point);
    void reduce();
	void clear();
	
    inline size_t                     point_count() const noexcept { return m_simplex_active.count(); }
    inline const std::array<vec3, 4>& points() const noexcept { return m_simplex; }
};

#endif