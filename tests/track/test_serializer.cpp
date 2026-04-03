#include "track/TrackSerializer.h"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

using namespace trackmini::track;
namespace fs = std::filesystem;

// temporary dir for test files
class SerializerTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        m_tmp = fs::temp_directory_path() / "tmc_test_track";
        fs::create_directories(m_tmp);
        m_path = m_tmp / "test.tmc";
    }
    void TearDown() override { fs::remove_all(m_tmp); }

    BlockCatalog make_catalog() { return BlockCatalog::make_default(); }

    fs::path m_tmp;
    fs::path m_path;
};

TEST_F(SerializerTest, SaveAndLoadRoundTrip)
{
    auto cat = make_catalog();
    Track original{ cat };
    original.meta().name = "Test Track";
    original.meta().author = "Nicolas";
    original.place_block({ 0, 0, 0 }, BlockId::Road);
    original.place_block({ 1, 0, 0 }, BlockId::Road);
    original.place_block({ 2, 0, 0 }, BlockId::Finish);

    auto save_result = TrackSerializer::save(original, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;

    auto load_result = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(load_result.has_value()) << load_result.error().message;

    auto const& loaded = *load_result;
    EXPECT_EQ(loaded.meta().name, "Test Track");
    EXPECT_EQ(loaded.meta().author, "Nicolas");
    EXPECT_EQ(loaded.block_count(), 3u);
    EXPECT_EQ(loaded.grid().at({ 0, 0, 0 }).id, BlockId::Road);
    EXPECT_EQ(loaded.grid().at({ 2, 0, 0 }).id, BlockId::Finish);
}

TEST_F(SerializerTest, BestTimePreserved)
{
    auto cat = make_catalog();
    Track t{ cat };
    t.meta().best_time_ms = 43210u;
    t.place_block({ 0, 0, 0 }, BlockId::Finish);

    auto save_result = TrackSerializer::save(t, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;
    auto loaded = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(loaded.has_value());
    ASSERT_TRUE(loaded->meta().best_time_ms.has_value());
    EXPECT_EQ(*loaded->meta().best_time_ms, 43210u);
}

TEST_F(SerializerTest, NoBestTimePreserved)
{
    auto cat = make_catalog();
    Track t{ cat };
    // best_time_ms undefined
    t.place_block({ 0, 0, 0 }, BlockId::Finish);

    auto save_result = TrackSerializer::save(t, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;
    auto loaded = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_FALSE(loaded->meta().best_time_ms.has_value());
}

TEST_F(SerializerTest, RotationPreservedThroughSerialization)
{
    auto cat = make_catalog();
    Track t{ cat };
    t.place_block({ 0, 0, 0 }, BlockId::Road, Rotation::R270);

    auto save_result = TrackSerializer::save(t, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;
    auto loaded = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->grid().at({ 0, 0, 0 }).rotation, Rotation::R270);
}

TEST_F(SerializerTest, LoadFromNonExistentFileFails)
{
    auto cat = make_catalog();
    auto result = TrackSerializer::load(cat, m_tmp / "nonexistent.tmc");
    EXPECT_FALSE(result.has_value());
}

TEST_F(SerializerTest, LoadFromCorruptedFileFails)
{
    // write invalid data
    std::ofstream f{ m_path, std::ios::binary };
    f << "not a valid track file at all $$$$";
    f.close();

    auto cat = make_catalog();
    auto result = TrackSerializer::load(cat, m_path);
    EXPECT_FALSE(result.has_value());
}

TEST_F(SerializerTest, EmptyTrackRoundTrip)
{
    auto cat = make_catalog();
    Track t{ cat };
    t.meta().name = "Empty";

    auto save_result = TrackSerializer::save(t, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;
    auto loaded = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->block_count(), 0u);
}

TEST_F(SerializerTest, LargeTrackRoundTrip)
{
    auto cat = make_catalog();
    Track t{ cat };

    // place 500 blocks at various locations
    for (int i = 0; i < 50; ++i)
        for (int j = 0; j < 10; ++j)
            t.place_block(
              { static_cast<int16_t>(i), 0, static_cast<int16_t>(j) },
              BlockId::Road);

    auto save_result = TrackSerializer::save(t, m_path);
    ASSERT_TRUE(save_result.has_value()) << save_result.error().message;
    auto loaded = TrackSerializer::load(cat, m_path);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(loaded->block_count(), 500u);
}