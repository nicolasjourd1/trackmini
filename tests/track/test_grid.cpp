#include "track/Grid.h"
#include <gtest/gtest.h>

using namespace trackmini::track;

static BlockInstance
road_block()
{
    return { BlockId::Road, Rotation::R0 };
}

TEST(GridTest, EmptyGridHasZeroBlocks)
{
    Grid g;
    EXPECT_EQ(g.block_count(), 0u);
}

TEST(GridTest, PlaceAndRetrieve)
{
    Grid g;
    GridPos pos{ 0, 0, 0 };
    EXPECT_TRUE(g.place(pos, road_block()));
    EXPECT_EQ(g.at(pos).id, BlockId::Road);
}

TEST(GridTest, UnplacedCellIsEmpty)
{
    Grid g;
    EXPECT_TRUE(g.at({ 5, 0, 3 }).is_empty());
}

TEST(GridTest, PlaceIncrementsCount)
{
    Grid g;
    g.place({ 0, 0, 0 }, road_block());
    g.place({ 1, 0, 0 }, road_block());
    EXPECT_EQ(g.block_count(), 2u);
}

TEST(GridTest, PlaceSamePositionOverwrites)
{
    Grid g;
    GridPos pos{ 0, 0, 0 };
    g.place(pos, { BlockId::Road, Rotation::R0 });
    g.place(pos, { BlockId::Finish, Rotation::R90 });
    EXPECT_EQ(g.block_count(), 1u);
    EXPECT_EQ(g.at(pos).id, BlockId::Finish);
    EXPECT_EQ(g.at(pos).rotation, Rotation::R90);
}

TEST(GridTest, RemoveExistingBlock)
{
    Grid g;
    GridPos pos{ 2, 0, 1 };
    g.place(pos, road_block());
    EXPECT_TRUE(g.remove(pos));
    EXPECT_TRUE(g.at(pos).is_empty());
    EXPECT_EQ(g.block_count(), 0u);
}

TEST(GridTest, RemoveAbsentReturnsFalse)
{
    Grid g;
    EXPECT_FALSE(g.remove({ 0, 0, 0 }));
}

TEST(GridTest, OutOfBoundsPlaceReturnsFalse)
{
    Grid g;
    GridPos oob{ 200, 0, 0 }; // > kGridMax = 127
    EXPECT_FALSE(g.place(oob, road_block()));
    EXPECT_EQ(g.block_count(), 0u);
}

TEST(GridTest, ClearEmptiesGrid)
{
    Grid g;
    g.place({ 0, 0, 0 }, road_block());
    g.place({ 1, 0, 0 }, road_block());
    g.clear();
    EXPECT_EQ(g.block_count(), 0u);
}

TEST(GridTest, FindAllReturnsCorrectPositions)
{
    Grid g;
    g.place({ 0, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 1, 0, 0 }, { BlockId::Boost, Rotation::R0 });
    g.place({ 2, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 3, 0, 0 }, { BlockId::Finish, Rotation::R0 });

    auto roads = g.find_all(BlockId::Road);
    EXPECT_EQ(roads.size(), 2u);

    auto finishes = g.find_all(BlockId::Finish);
    EXPECT_EQ(finishes.size(), 1u);
}

TEST(GridTest, ForEachVisitsAllBlocks)
{
    Grid g;
    g.place({ 0, 0, 0 }, road_block());
    g.place({ 1, 0, 0 }, road_block());
    g.place({ 0, 1, 0 }, { BlockId::Boost, Rotation::R0 });

    std::size_t count = 0;
    g.for_each([&](GridPos, BlockInstance) { ++count; });
    EXPECT_EQ(count, 3u);
}

TEST(GridTest, ValidationFailsWithNoFinish)
{
    Grid g;
    g.place({ 0, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 1, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 2, 0, 0 }, { BlockId::Road, Rotation::R0 });
    auto r = g.validate();
    EXPECT_FALSE(r.valid);
    EXPECT_FALSE(r.errors.empty());
}

TEST(GridTest, ValidationPassesWithFinish)
{
    Grid g;
    g.place({ 0, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 1, 0, 0 }, { BlockId::Road, Rotation::R0 });
    g.place({ 2, 0, 0 }, { BlockId::Finish, Rotation::R0 });
    auto r = g.validate();
    EXPECT_TRUE(r.valid);
    EXPECT_TRUE(r.errors.empty());
}

TEST(GridTest, RotationIsPreserved)
{
    Grid g;
    g.place({ 0, 0, 0 }, { BlockId::Road, Rotation::R270 });
    EXPECT_EQ(g.at({ 0, 0, 0 }).rotation, Rotation::R270);
}

TEST(GridTest, InBoundsCheck)
{
    Grid g;
    EXPECT_TRUE(g.in_bounds({ 0, 0, 0 }));
    EXPECT_TRUE(g.in_bounds({ 127, 127, 127 }));
    EXPECT_TRUE(g.in_bounds({ -128, -128, -128 }));
    EXPECT_FALSE(g.in_bounds({ 128, 0, 0 }));
}