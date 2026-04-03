#include "track/BlockCatalog.h"
#include <gtest/gtest.h>

using namespace trackmini::track;

TEST(BlockCatalogTest, DefaultCatalogHasExpectedBlocks)
{
    auto cat = BlockCatalog::make_default();
    EXPECT_GE(cat.size(), 4u); // Road, Boost, Jump, Finish
}

TEST(BlockCatalogTest, FindKnownBlock)
{
    auto cat = BlockCatalog::make_default();
    auto result = cat.find(BlockId::Road);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ((*result)->id, BlockId::Road);
    EXPECT_EQ((*result)->name, "Road");
}

TEST(BlockCatalogTest, FindUnknownReturnsNullopt)
{
    auto cat = BlockCatalog::make_default();
    auto result = cat.find(BlockId::Empty);
    EXPECT_FALSE(result.has_value());
}

TEST(BlockCatalogTest, GetThrowsOnUnknown)
{
    auto cat = BlockCatalog::make_default();
    EXPECT_THROW({ (void)cat.get(BlockId::Empty); }, std::out_of_range);
}

TEST(BlockCatalogTest, BoostBlockHasForce)
{
    auto cat = BlockCatalog::make_default();
    auto const& boost = cat.get(BlockId::Boost);
    EXPECT_TRUE(boost.is_boost);
    EXPECT_GT(boost.boost_force, 0.f);
}

TEST(BlockCatalogTest, FinishIsNotSolid)
{
    auto cat = BlockCatalog::make_default();
    auto const& finish = cat.get(BlockId::Finish);
    EXPECT_TRUE(finish.is_finish);
    EXPECT_FALSE(finish.is_solid);
}

TEST(BlockCatalogTest, ForEachVisitsAll)
{
    auto cat = BlockCatalog::make_default();
    std::size_t count = 0;
    cat.for_each([&](BlockDef const&) { ++count; });
    EXPECT_EQ(count, cat.size());
}

TEST(BlockCatalogTest, RegisterCustomBlock)
{
    BlockCatalog cat;
    cat.register_block({
      .id = BlockId::Road,
      .name = "CustomRoad",
      .mesh_path = {},
    });
    auto r = cat.find(BlockId::Road);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ((*r)->name, "CustomRoad");
}