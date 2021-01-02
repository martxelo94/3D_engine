#include <gtest/gtest.h>
#include <pch.h>

// To be implemented by student
//std::vector<Triangle> load_triangles_from_obj(const char* filename);

TEST(kdtrees, empty)
{
    ::kdtree kdtree;
    kdtree.build({}, ::kdtree::config());

    ASSERT_TRUE(kdtree.aabbs().empty());
    ASSERT_TRUE(kdtree.nodes().empty());
    ASSERT_TRUE(kdtree.indices().empty());
}

TEST(kdtrees, slides)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles;
    triangles.push_back({{{{0, 0, 0}, {0.5, 0.5, 0}, {0.5, 0, 0}}}});       // Most left triangle
    triangles.push_back({{{{2.0, 2.0, 0}, {2.5, 2.5, 0}, {2.5, 0, 0}}}});   // Right cluster
    triangles.push_back({{{{2.5, 2.0, 0}, {3.0, 2.5, 0}, {3.0, 2, 0}}}});   // Right cluster
    triangles.push_back({{{{2.0, 2.5, 0}, {2.5, 2.5, 0}, {2.5, 2.5, 0}}}}); // Right cluster // ke perro
    triangles.push_back({{{{2.5, 2.5, 0}, {3.0, 3.0, 0}, {3.0, 2.5, 0}}}}); // Right cluster

    // KdTree
    ::kdtree::config config{};
    config.cost_intersection = 80;
    config.cost_traversal    = 1;
    kdtree.build(triangles, config);
    
    // Assuming first split happens in X
    auto root = kdtree.nodes().front();
    ASSERT_TRUE(root.is_internal());
    ASSERT_EQ(root.axis(), 0);    
    ASSERT_EQ(root.split(), 2.0);    
}

TEST(kdtrees, basic)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles;
    triangles.push_back({{{{0, 0, 0}, {1, 0, 1}, {1, 1, 1}}}});

    // KdTree
    kdtree.build(triangles, ::kdtree::config());
    ASSERT_FALSE(kdtree.aabbs().empty());
    ASSERT_FALSE(kdtree.nodes().empty());
    ASSERT_FALSE(kdtree.indices().empty());
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.x - kdtree.aabbs()[0].min_point.x, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.y - kdtree.aabbs()[0].min_point.y, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.z - kdtree.aabbs()[0].min_point.z, 1.0f);
    ASSERT_EQ(kdtree.indices()[0], 0u);
    ASSERT_TRUE(kdtree.nodes()[0].is_leaf());
    ASSERT_EQ(kdtree.nodes()[0].primitive_count(), 1);
    ASSERT_EQ(kdtree.nodes()[0].primitive_start(), 0);
}

TEST(kdtrees, basic_reuse)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles;
    triangles.push_back({{{{0, 0, 0}, {1, 0, 1}, {1, 1, 1}}}});

    // KdTree
    kdtree.build(triangles, ::kdtree::config());
    ASSERT_FALSE(kdtree.aabbs().empty());
    ASSERT_FALSE(kdtree.nodes().empty());
    ASSERT_FALSE(kdtree.indices().empty());
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.x - kdtree.aabbs()[0].min_point.x, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.y - kdtree.aabbs()[0].min_point.y, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.z - kdtree.aabbs()[0].min_point.z, 1.0f);
    ASSERT_EQ(kdtree.indices()[0], 0u);
    ASSERT_TRUE(kdtree.nodes()[0].is_leaf());
    ASSERT_EQ(kdtree.nodes()[0].primitive_count(), 1);
    ASSERT_EQ(kdtree.nodes()[0].primitive_start(), 0);

    // Reuse
    kdtree.build({}, ::kdtree::config());
    ASSERT_TRUE(kdtree.aabbs().empty());
    ASSERT_TRUE(kdtree.nodes().empty());
    ASSERT_TRUE(kdtree.indices().empty());

    // Reuse 2
    kdtree.build(triangles, ::kdtree::config());
    ASSERT_FALSE(kdtree.aabbs().empty());
    ASSERT_FALSE(kdtree.nodes().empty());
    ASSERT_FALSE(kdtree.indices().empty());
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.x - kdtree.aabbs()[0].min_point.x, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.y - kdtree.aabbs()[0].min_point.y, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.z - kdtree.aabbs()[0].min_point.z, 1.0f);
    ASSERT_EQ(kdtree.indices()[0], 0u);
    ASSERT_TRUE(kdtree.nodes()[0].is_leaf());
    ASSERT_EQ(kdtree.nodes()[0].primitive_count(), 1);
    ASSERT_EQ(kdtree.nodes()[0].primitive_start(), 0);
}

TEST(kdtrees, basic_2)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles;
    triangles.push_back({{{{0, 0, 0}, {1, 0, 1}, {1, 1, 1}}}});
    triangles.push_back({{{{0, 0, 0}, {1, 0, 1}, {1, 1, 1}}}});

    // KdTree
    kdtree.build(triangles, ::kdtree::config());
    ASSERT_EQ(kdtree.aabbs().size(), 1u);
    ASSERT_EQ(kdtree.nodes().size(), 1u);
    ASSERT_EQ(kdtree.indices().size(), 2u);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.x - kdtree.aabbs()[0].min_point.x, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.y - kdtree.aabbs()[0].min_point.y, 1.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.z - kdtree.aabbs()[0].min_point.z, 1.0f);
    ASSERT_EQ(kdtree.indices()[0], 0u);
    ASSERT_TRUE(kdtree.nodes()[0].is_leaf());
    ASSERT_EQ(kdtree.nodes()[0].primitive_count(), 2);
    ASSERT_EQ(kdtree.nodes()[0].primitive_start(), 0);
}

TEST(kdtrees, basic_3)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles;
    triangles.push_back({{{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}}}); // No volume
    triangles.push_back({{{{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}}}); // No volume

    // KdTree
    kdtree.build(triangles, ::kdtree::config());
    ASSERT_EQ(kdtree.aabbs().size(), 1u);
    ASSERT_EQ(kdtree.nodes().size(), 1u);
    ASSERT_EQ(kdtree.indices().size(), 2u);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.x - kdtree.aabbs()[0].min_point.x, 0.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.y - kdtree.aabbs()[0].min_point.y, 0.0f);
    ASSERT_FLOAT_EQ(kdtree.aabbs()[0].max_point.z - kdtree.aabbs()[0].min_point.z, 0.0f);
    ASSERT_EQ(kdtree.indices()[0], 0u);
    ASSERT_TRUE(kdtree.nodes()[0].is_leaf());
    ASSERT_EQ(kdtree.nodes()[0].primitive_count(), 2);
    ASSERT_EQ(kdtree.nodes()[0].primitive_start(), 0);
}

TEST(kdtrees, gourd)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles = load_triangles_from_obj("../resources/meshes/gourd.obj");

    // KdTree (just don't crash)
    kdtree.build(triangles, ::kdtree::config());

	printf("kdtree finish");
}

TEST(kdtrees, stress_bunny)
{
    ::kdtree kdtree;

    // Scene
    std::vector<Triangle> triangles = load_triangles_from_obj("../resources/meshes/bunny.obj");

    // KdTree (just don't crash)
    kdtree.build(triangles, ::kdtree::config());
}
