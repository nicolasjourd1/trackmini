#include "editor/Command.h"
#include "editor/CommandHistory.h"
#include "track/BlockCatalog.h"
#include <gtest/gtest.h>

using namespace trackmini;

class CommandTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        catalog = std::make_unique<track::BlockCatalog>(
          track::BlockCatalog::make_default());
        track = std::make_unique<trackmini::track::Track>(*catalog);
    }

    std::unique_ptr<track::BlockCatalog> catalog;
    std::unique_ptr<track::Track> track;
};

TEST_F(CommandTest, PlaceCommandPlacesBlock)
{
    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        track::GridPos{ 0, 0, 0 },
        track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
      *track);
    EXPECT_EQ(track->grid().at({ 0, 0, 0 }).id, track::BlockId::Road);
}

TEST_F(CommandTest, UndoPlaceRestoresEmpty)
{
    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        track::GridPos{ 0, 0, 0 },
        track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
      *track);
    hist.undo(*track);
    EXPECT_TRUE(track->grid().at({ 0, 0, 0 }).is_empty());
}

TEST_F(CommandTest, RedoRePlacesBlock)
{
    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        track::GridPos{ 0, 0, 0 },
        track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
      *track);
    hist.undo(*track);
    hist.redo(*track);
    EXPECT_EQ(track->grid().at({ 0, 0, 0 }).id, track::BlockId::Road);
}

TEST_F(CommandTest, RemoveCommandRemovesBlock)
{
    track->grid().place({ 1, 0, 0 },
                        { track::BlockId::Boost, track::Rotation::R0 });
    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::RemoveBlockCommand>(track::GridPos{ 1, 0, 0 }),
      *track);
    EXPECT_TRUE(track->grid().at({ 1, 0, 0 }).is_empty());
}

TEST_F(CommandTest, UndoRemoveRestoresBlock)
{
    track->grid().place({ 1, 0, 0 },
                        { track::BlockId::Boost, track::Rotation::R0 });
    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::RemoveBlockCommand>(track::GridPos{ 1, 0, 0 }),
      *track);
    hist.undo(*track);
    EXPECT_EQ(track->grid().at({ 1, 0, 0 }).id, track::BlockId::Boost);
}

TEST_F(CommandTest, NewCommandAfterUndoClearsFuture)
{
    editor::CommandHistory hist;
    track::GridPos p0{ 0, 0, 0 }, p1{ 1, 0, 0 };

    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        p0, track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
      *track);

    hist.undo(*track);
    EXPECT_EQ(hist.redo_count(), 1u);

    // Nouvelle commande → redo effacé
    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        p1, track::BlockInstance{ track::BlockId::Boost, track::Rotation::R0 }),
      *track);

    EXPECT_EQ(hist.redo_count(), 0u);
    EXPECT_FALSE(hist.can_redo());
}

TEST_F(CommandTest, UndoOnEmptyReturnsFalse)
{
    editor::CommandHistory hist;
    EXPECT_FALSE(hist.undo(*track));
}

TEST_F(CommandTest, RedoOnEmptyReturnsFalse)
{
    editor::CommandHistory hist;
    EXPECT_FALSE(hist.redo(*track));
}

TEST_F(CommandTest, MaxSizeLimitsHistory)
{
    editor::CommandHistory hist{ 3 }; // max 3 commands

    for (int i = 0; i < 5; ++i) {
        hist.execute(
          std::make_unique<editor::PlaceBlockCommand>(
            track::GridPos{ static_cast<int16_t>(i), 0, 0 },
            track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
          *track);
    }

    EXPECT_EQ(hist.undo_count(), 3u);
}

TEST_F(CommandTest, CanUndoAndRedoAreAccurate)
{
    editor::CommandHistory hist;
    EXPECT_FALSE(hist.can_undo());
    EXPECT_FALSE(hist.can_redo());

    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        track::GridPos{ 0, 0, 0 },
        track::BlockInstance{ track::BlockId::Road, track::Rotation::R0 }),
      *track);

    EXPECT_TRUE(hist.can_undo());
    EXPECT_FALSE(hist.can_redo());

    hist.undo(*track);
    EXPECT_FALSE(hist.can_undo());
    EXPECT_TRUE(hist.can_redo());
}

TEST_F(CommandTest, PlaceOverExistingUndoRestoresOriginal)
{
    track->grid().place({ 0, 0, 0 },
                        { track::BlockId::Boost, track::Rotation::R0 });

    editor::CommandHistory hist;
    hist.execute(
      std::make_unique<editor::PlaceBlockCommand>(
        track::GridPos{ 0, 0, 0 },
        track::BlockInstance{ track::BlockId::Finish, track::Rotation::R0 }),
      *track);

    EXPECT_EQ(track->grid().at({ 0, 0, 0 }).id, track::BlockId::Finish);
    hist.undo(*track);
    EXPECT_EQ(track->grid().at({ 0, 0, 0 }).id, track::BlockId::Boost);
}