#pragma once

#include <cstdint>

namespace board
{
    struct defs
    {
        static constexpr auto NB_POS = 64;
        static constexpr auto DIRECTIONS = 8;

        static constexpr uint64_t RANK_ONE = 0xffULL;
        static constexpr uint64_t RANK_TWO = 0xff00ULL;
        static constexpr uint64_t RANK_THREE = 0xff0000ULL;
        static constexpr uint64_t RANK_FOUR = 0xff000000ULL;
        static constexpr uint64_t RANK_FIVE = 0xff00000000ULL;
        static constexpr uint64_t RANK_SIX = 0xff0000000000ULL;
        static constexpr uint64_t RANK_SEVEN = 0xff000000000000ULL;
        static constexpr uint64_t RANK_EIGHT = 0xff00000000000000ULL;

        static constexpr uint64_t FILE_H = 0x8080808080808080ull;
        static constexpr uint64_t FILE_G = 0x4040404040404040ull;
        static constexpr uint64_t FILE_F = 0x2020202020202020ull;
        static constexpr uint64_t FILE_E = 0x1010101010101010ull;
        static constexpr uint64_t FILE_D = 0x808080808080808ull;
        static constexpr uint64_t FILE_C = 0x404040404040404ull;
        static constexpr uint64_t FILE_B = 0x202020202020202ull;
        static constexpr uint64_t FILE_A = 0x101010101010101ull;

        static constexpr uint64_t borders = FILE_A | FILE_H
                                            | RANK_ONE | RANK_EIGHT;

        // In those arrays for each position we count the number of possibile
        // position (excluding the border that we consider at the end) we can
        // go to based on the piece type
        static constexpr int rook_bits[NB_POS] =
        {
            12, 11, 11, 11, 11, 11, 11, 12,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            11, 10, 10, 10, 10, 10, 10, 11,
            12, 11, 11, 11, 11, 11, 11, 12
        };

        static constexpr int bishop_bits[NB_POS] =
        {
            6, 5, 5, 5, 5, 5, 5, 6,
            5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 9, 9, 7, 5, 5,
            5, 5, 7, 7, 7, 7, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5,
            6, 5, 5, 5, 5, 5, 5, 6
        };

        static constexpr uint64_t rook_magics[NB_POS] =
        {
            0x80002494400581ULL,
            0x240004020001000ULL,
            0x608010000c802000ULL,
            0x6100040820100102ULL,
            0x200052008100200ULL,
            0x800841002200840ULL,
            0xc00080201009004ULL,
            0x80142144800500ULL,
            0x480800461884004ULL,
            0x8000802000400080ULL,
            0x4021002001004010ULL,
            0x901001001000c20ULL,
            0x8004800800040080ULL,
            0xa001000300040008ULL,
            0x9c04000482081150ULL,
            0x28a000044010082ULL,
            0x880044000200040ULL,
            0x2220c14000a01001ULL,
            0x202c808010002000ULL,
            0x800101000e100020ULL,
            0x8404008004080080ULL,
            0x44008002000480ULL,
            0x1002440002415018ULL,
            0x200220000608104ULL,
            0x100400080008020ULL,
            0x2500140002000ULL,
            0x80904430015a000ULL,
            0x80800a2300100100ULL,
            0x204e0006000aULL,
            0x20080800400ULL,
            0x8002100400820108ULL,
            0x40208a20011004cULL,
            0x4804009800221ULL,
            0x120100020400040ULL,
            0x40841006802000ULL,
            0x80a001042002008ULL,
            0x9020801001004ULL,
            0x820800200800400ULL,
            0x40281802040050a1ULL,
            0x20049502000044ULL,
            0x1080002000414000ULL,
            0x742010004a4001ULL,
            0x21004020010012ULL,
            0x1802020820120040ULL,
            0x31000800110004ULL,
            0xa60c001008020200ULL,
            0x80802802b0040021ULL,
            0x40090b1280420004ULL,
            0x1080002011c00940ULL,
            0x880810042002a00ULL,
            0x4e11004620001100ULL,
            0x801000180180ULL,
            0x6808010084100900ULL,
            0x10c0040002008080ULL,
            0x8080010810020400ULL,
            0x2908008104204200ULL,
            0x40201080030441ULL,
            0x100081004000102bULL,
            0x100120820010041ULL,
            0x810090104201001ULL,
            0xf081002410080023ULL,
            0x2001430910802ULL,
            0x7000009008020144ULL,
            0x8008008100340842ULL
        };

        static constexpr uint64_t bishop_magics[NB_POS] =
        {
            0x21200086008220ULL,
            0x4220056c0904c881ULL,
            0x2024440c20400400ULL,
            0x41482240c0010200ULL,
            0x411040000e0400ULL,
            0x5100884000014ULL,
            0x440405400800ULL,
            0x824c1004020bULL,
            0x24040042400c200ULL,
            0x820c051a0a00ULL,
            0x29222206010cULL,
            0x800080a51004500ULL,
            0x388011040544208ULL,
            0x70832820108005ULL,
            0x20010410048482ULL,
            0x20480a8011002ULL,
            0x1400201810930eULL,
            0x9082052040240ULL,
            0xc01001001020010ULL,
            0x318001220801000ULL,
            0x4800400a00000ULL,
            0x4281008808821020ULL,
            0x8000442424020884ULL,
            0x2000051040140ULL,
            0x2024302021200902ULL,
            0x4001040820081208ULL,
            0x21091000044400a0ULL,
            0x1040040011110020ULL,
            0x802100110100401cULL,
            0x104840800100a004ULL,
            0x1601a004088800ULL,
            0x3002450100848800ULL,
            0x500210a408d02100ULL,
            0x25a0a1020921000ULL,
            0x4046220a300406ULL,
            0x4000040400080120ULL,
            0x802208400220020ULL,
            0x808100020100ULL,
            0x5510041090010080ULL,
            0x8050900004050ULL,
            0x8888460a0040800ULL,
            0x2a00c220403820ULL,
            0x16084450000800ULL,
            0x100046018020300ULL,
            0x900080100400400ULL,
            0x6040010041008080ULL,
            0x90204080a108484ULL,
            0x802040042020088ULL,
            0x310908c200082ULL,
            0x301040894041008ULL,
            0x4011802108080001ULL,
            0xe00a4042020010ULL,
            0x2a028504000ULL,
            0x900022041b620000ULL,
            0x8608021002221000ULL,
            0x80100a6084049000ULL,
            0x2440084104240ULL,
            0x100460a102228ULL,
            0x8010042080400ULL,
            0x204002000840400ULL,
            0x4000001208030400ULL,
            0x6a0040100ULL,
            0x10055808550400ULL,
            0x4040011252004900ULL
        };
    };
}