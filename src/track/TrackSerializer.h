#ifndef TRACKSERIALIZER_H_
#define TRACKSERIALIZER_H_

/**
 [Header 8 bytes]
  magic    : uint32  = 0x544D4300 ('TMC\0')
  version  : uint16  = 1
  reserved : uint16  = 0

[Meta]
  name_len   : uint16
  name       : char[name_len]
  author_len : uint16
  author     : char[author_len]
  best_time  : uint32  (0 = none)

[Blocks]
  block_count : uint32
  per block   : int16 x, int16 y, int16 z, uint16 id, uint8 rotation
                = 9 bytes per block
 */

#include "track/Track.h"
#include <cstring>
#include <expected>
#include <filesystem>
#include <fstream>
#include <string>

namespace trackmini::track {

struct SerializeError
{
    std::string message;
};

inline constexpr uint32_t kTrackMagic = 0x544D4300u; // 'TMC\0'
inline constexpr uint16_t kTrackVersion = 1u;

class TrackSerializer
{

  public:
    [[nodiscard]]
    static auto save(Track const& track, std::filesystem::path const& path)
      -> std::expected<void, SerializeError>
    {
        std::ofstream f{ path, std::ios::binary };
        if (!f)
            return std::unexpected(
              SerializeError{ "Cannot open for writing: " + path.string() });

        // Header
        write_u32(f, kTrackMagic);
        write_u16(f, kTrackVersion);
        write_u16(f, 0); // reserved

        // Meta
        auto const& meta = track.meta();
        write_str(f, meta.name);
        write_str(f, meta.author);
        write_u32(f, meta.best_time_ms.value_or(0u));

        // Blocks
        auto const count = static_cast<uint32_t>(track.block_count());
        write_u32(f, count);

        track.grid().for_each([&](GridPos pos, BlockInstance blk) {
            write_i16(f, pos.x);
            write_i16(f, pos.y);
            write_i16(f, pos.z);
            write_u16(f, static_cast<uint16_t>(blk.id));
            write_u8(f, static_cast<uint8_t>(blk.rotation));
        });

        return {};
    }

    [[nodiscard]]
    static auto load(BlockCatalog const& catalog,
                     std::filesystem::path const& path)
      -> std::expected<Track, SerializeError>
    {
        std::ifstream f{ path, std::ios::binary };
        if (!f)
            return std::unexpected(
              SerializeError{ "Cannot open for reading: " + path.string() });

        // Header
        if (read_u32(f) != kTrackMagic)
            return std::unexpected(SerializeError{ "Invalid magic number" });

        uint16_t version = read_u16(f);
        if (version != kTrackVersion)
            return std::unexpected(SerializeError{ "Unsupported version: " +
                                                   std::to_string(version) });
        read_u16(f); // reserved

        // Meta
        Track track{ catalog };
        track.meta().name = read_str(f);
        track.meta().author = read_str(f);
        uint32_t best = read_u32(f);
        if (best > 0)
            track.meta().best_time_ms = best;

        // Blocks
        uint32_t count = read_u32(f);
        for (uint32_t i = 0; i < count; ++i) {
            GridPos pos;
            pos.x = read_i16(f);
            pos.y = read_i16(f);
            pos.z = read_i16(f);
            BlockInstance blk;
            blk.id = static_cast<BlockId>(read_u16(f));
            blk.rotation = static_cast<Rotation>(read_u8(f));
            track.grid().place(pos, blk);
        }

        return track;
    }

  private:
    // Writing helpers
    static void write_u8(std::ostream& f, uint8_t v)
    {
        f.write(reinterpret_cast<char const*>(&v), 1);
    }
    static void write_u16(std::ostream& f, uint16_t v)
    {
        f.write(reinterpret_cast<char const*>(&v), 2);
    }
    static void write_u32(std::ostream& f, uint32_t v)
    {
        f.write(reinterpret_cast<char const*>(&v), 4);
    }
    static void write_i16(std::ostream& f, int16_t v)
    {
        f.write(reinterpret_cast<char const*>(&v), 2);
    }
    static void write_str(std::ostream& f, std::string const& s)
    {
        write_u16(f, static_cast<uint16_t>(s.size()));
        f.write(s.data(), static_cast<std::streamsize>(s.size()));
    }

    // Reading helpers
    static uint8_t read_u8(std::istream& f)
    {
        uint8_t v;
        f.read(reinterpret_cast<char*>(&v), 1);
        return v;
    }
    static uint16_t read_u16(std::istream& f)
    {
        uint16_t v;
        f.read(reinterpret_cast<char*>(&v), 2);
        return v;
    }
    static uint32_t read_u32(std::istream& f)
    {
        uint32_t v;
        f.read(reinterpret_cast<char*>(&v), 4);
        return v;
    }
    static int16_t read_i16(std::istream& f)
    {
        int16_t v;
        f.read(reinterpret_cast<char*>(&v), 2);
        return v;
    }
    static std::string read_str(std::istream& f)
    {
        uint16_t len = read_u16(f);
        std::string s(len, '\0');
        f.read(s.data(), len);
        return s;
    }
};

} // namespace trackmini::track

#endif /* TRACKSERIALIZER_H_ */
