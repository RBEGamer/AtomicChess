#pragma once

#include <cstdint>
#include <optional>
#include <cassert>

#include "entity/position.hh"
#include "entity/color.hh"
#include "entity/piece-type.hh"
#include "defs.hh"
#include "utils/bits-utils.hh"

namespace board
{
    class MoveInitialization
    {
    private:
        static MoveInitialization instance_;

        uint64_t rook_masks[defs::NB_POS];
        uint64_t bishop_masks[defs::NB_POS];
        uint64_t king_masks[defs::NB_POS];
        uint64_t knight_masks[defs::NB_POS];
        uint64_t pawn_masks[2][defs::NB_POS];

        uint64_t rook_attacks[defs::NB_POS][4096];
        uint64_t bishop_attacks[defs::NB_POS][1024];

        uint64_t rays[defs::DIRECTIONS][defs::NB_POS];

        MoveInitialization();

        uint64_t get_ray(const Position& from, const int file,
                         const int rank) const;
        void init_rays(void);

        void init_rook_masks(void);
        void init_bishop_masks(void);
        void init_king_masks(void);
        void init_knight_masks(void);
        void init_pawn_masks(void);

        /**
        * @short Gerate for this specific index & mask the blocker mask
        * @param index of the current blocker
        * @param mask of possible move of the current piece
        * @return the mask representing the current blocker
        *
        * We need a way to generate a unique mask of blockers or each blocker
        * To do that we are going to use the bit representation of index
        * To generate all those possibilites the for loops goes on all the set
        * bits of the current mask
        * For each of those set bits we get their position and remove them
        * from the mask
        * We then only set it at the correct pos if it intersect with the index
        * By using the index bit representation we generate all the possiblities
        * and uniquely for all the blockers possiblities
        */
        uint64_t get_blockers(const int blocker_index,
                              uint64_t current_mask) const;

        uint64_t get_bishop_attacks_classical(int pos, uint64_t blockers) const;
        uint64_t get_rook_attacks_classical(int pos, uint64_t blockers) const;

        void init_bishop_attacks(void);
        void init_rook_attacks(void);

    public:
        static MoveInitialization& get_instance();

        // Returns attacks set (masks for kings and knights) from the hashmap
        uint64_t get_targets(const PieceType& piece,
                             const int pos,
                             uint64_t blockers) const;
        // Same but special case for pawns
        uint64_t get_pawn_targets(const int pos, const Color& color) const;
    };
}