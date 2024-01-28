/*
 * bitboard.hpp
 *
 *  Created on: 25 Sep 2015
 *      Author: miles
 */

#ifndef UTIL_BITBOARD_HPP_
#define UTIL_BITBOARD_HPP_

#include <bitset>

typedef unsigned long long int UINT_64;

namespace bitchess {

/**
 * Represents a bitset of 64-bits.
 */
class Bitboard {
public:

	/**
	 * Initialises a new empty Bitboard, equivalent to a blank board
	 */
	Bitboard() {
	}

	/**
	 * Initialises the bitboard to the specified 64-bit integer value
	 * @param initval 64-bit integer
	 */
	Bitboard( long long unsigned int initval ) :
			bits(initval) {
	}

	/**
	 * Initialises the bitboard with the bitset provided.
	 * @param new_bs bitset to initialise the Bitboard with.
	 */
	Bitboard( std::bitset<64> new_bs):
		bits(new_bs) {
	}

	/**
	 * Static factory method for providing a BitBoard with a single bit set to 1
	 * at the specified index. E.g. (using shortened 8-bit example)
	 * Bitboard::with_bit_set_at(2) = 00000100.
	 * @param index Index to set bit at. Must be positive. Starts with zero being the first bit.
	 * @return a new Bitboard with a single bit set.
	 */
	static Bitboard with_bit_set_at(short index) {
		Bitboard b = Bitboard();
		b.bits.set(index);
		return b;
	}
	/**
	 * @return a new Bitboard with all bits shifted up one rank. E.g.
	 * 0 0 0 0     0 1 1 0
	 * 0 1 1 0     0 1 1 0
	 * 0 1 1 0 - > 0 0 0 0
	 * 0 0 0 0     0 0 0 0
	 */
	inline Bitboard nortOne();
	/**
	 * @return a new Bitboard with all bits shifted down one rank. E.g.
	 * 0 0 0 0     0 0 0 0
	 * 0 1 1 0     0 0 0 0
	 * 0 1 1 0 - > 0 1 1 0
	 * 0 0 0 0     0 1 1 0
	 */
	inline Bitboard soutOne();
	/**
	 * @return a new Bitboard with all bits shifted left one file. E.g.
	 * 0 0 0 0     0 0 0 0
	 * 0 1 1 0     1 1 0 0
	 * 0 1 1 0 - > 1 1 0 0
	 * 0 0 0 0     0 0 0 0
	 */
	inline Bitboard westOne();
	/**
	 * @return a new Bitboard with all bits shifted right one file. E.g.
	 * 0 0 0 0     0 0 0 0
	 * 0 1 1 0     0 0 1 1
	 * 0 1 1 0 - > 0 0 1 1
	 * 0 0 0 0     0 0 0 0
	 */
	inline Bitboard eastOne();
	/**
	 * @return a new Bitboard with all bits shifted up a rank and to the right one file. E.g.
	 * 0 0 0 0     0 0 1 1
	 * 0 1 1 0     0 0 1 1
	 * 0 1 1 0 - > 0 0 0 0
	 * 0 0 0 0     0 0 0 0
	 */
	inline Bitboard noEaOne();
	/**
	 * @return a new Bitboard with all bits shifted down a rank and to the right one file. E.g.
	 * 0 0 0 0     0 0 0 0
	 * 0 1 1 0     0 0 0 0
	 * 0 1 1 0 - > 0 0 1 1
	 * 0 0 0 0     0 0 1 1
	 */
	inline Bitboard soEaOne();
	/**
	 * @return a new Bitboard with all bits shifted up a rank and to the left one file. E.g.
	 * 0 0 0 0     1 1 0 0
	 * 0 1 1 0     1 1 0 0
	 * 0 1 1 0 - > 0 0 0 0
	 * 0 0 0 0     0 0 0 0
	 */
	inline Bitboard noWeOne();
	/**
	 * @return a new Bitboard with all bits shifted down a rank and to the left one file. E.g.
	 * 0 0 0 0     0 0 0 0
	 * 0 1 1 0     0 0 0 0
	 * 0 1 1 0 - > 1 1 0 0
	 * 0 0 0 0     1 1 0 0
	 */
	inline Bitboard soWeOne();

	/**
	 * @return the least significant bit, or -1 if the bitset is empty.
	 */
	inline short ls1b() {
		return __builtin_ffsll(bits.to_ulong()) -1;
	}

	/**
	 * Bitscans for the least significant bit and returns its index, as well as
	 * the Bitboard with the bit set to zero (for further bitscanning).
	 * @return std::pair with the index of the LS1B and a new bitboard with the bit removed.
	 * If this Bitboard is empty, the function returns pair<-1, empty_bitboard>.
	 */
	inline std::pair<short, Bitboard> bitscan_and_remove() {
		short lsb = ls1b();

		if(lsb==-1) {
			return std::pair<short, Bitboard>(-1, Bitboard());
		}
		Bitboard b(this->bits);
		b.bits.set(lsb,false);
		return std::pair<short, Bitboard>(lsb, b);
	}

	/**
	 * Stores the set of bits.
	 */
	std::bitset<64> bits;
};

/**
 * @return the Bitboard b with bits left shifted by amount.
 */
inline Bitboard operator<<(Bitboard b,UINT_64 amount) {
	return Bitboard(b.bits.to_ulong() << amount);
}
/**
 * @return the Bitboard a with bits left shifted by bitboard b.
 */
inline Bitboard operator<<(Bitboard a,Bitboard b) {
	return Bitboard(a.bits << b.bits);
}
/**
 * @return the Bitboard b with bits right shifted by amount.
 */
inline Bitboard operator>>(Bitboard b,UINT_64 amount) {
	return Bitboard(b.bits.to_ulong() >> amount);
}
/**
 * @return the Bitboard a with bits right shifted by bitboard b.
 */
inline Bitboard operator>>(Bitboard a,Bitboard b) {
	return Bitboard(a.bits >> b.bits);
}
/**
 * @return the Bitboard b, bitwise AND with amount
 */
inline Bitboard operator&(Bitboard b,UINT_64 amount) {
	return Bitboard(b.bits.to_ulong() & amount);
}
/**
 * @return the Bitboard a, bitwise AND with Bitboard b
 */
inline Bitboard operator&(Bitboard a,Bitboard b) {
	return Bitboard(a.bits & b.bits);
}
/**
 * @return a = a & b
 */
inline Bitboard operator&=(Bitboard &a, Bitboard b) {
	a = a & b;
	return a;
}
/**
 * @return the bitwise complement of Bitboard b
 */
inline Bitboard operator~(Bitboard b) {
	return Bitboard(~b.bits);
}
/**
 * @return the Bitboard b, bitwise OR with amount
 */
inline Bitboard operator|(Bitboard b,UINT_64 amount) {
	return Bitboard(b.bits.to_ulong() | amount);
}
/**
 * @return the Bitboard a, bitwise OR with Bitboard b
 */
inline Bitboard operator|(Bitboard a,Bitboard b) {
	return Bitboard(a.bits | b.bits);
}
/**
 * @return a = a | b
 */
inline Bitboard operator|=(Bitboard &a, Bitboard b) {
	a = a | b;
	return a;
}
/**
 * @return the Bitboard b, bitwise XOR with amount
 */
inline Bitboard operator^(Bitboard b,UINT_64 amount) {
	return Bitboard(b.bits.to_ulong() ^ amount);
}
/**
 * @return the Bitboard a, bitwise XOR with Bitboard b
 */
inline Bitboard operator^(Bitboard a,Bitboard b) {
	return Bitboard(a.bits ^ b.bits);
}
/**
 * @return whether Bitboard a equals Bitboard b
 */
inline bool operator==(Bitboard a,Bitboard b) {
	return a.bits == b.bits;
}
/**
 * @return whether Bitboard a does not equal Bitboard b
 */
inline bool operator!=(Bitboard a,Bitboard b) {
	return a.bits != b.bits;
}

/**
 * a Bitboard masking the A rank, e.g. to get a bitboard and ignore
 * the A rank, do calc_result = calc_result & NOT_A_MASK
 */
static const Bitboard NOT_A_MASK = Bitboard(0xfefefefefefefefeULL);
/**
 * a Bitboard masking the H rank, e.g. to get a bitboard and ignore
 * the A rank, do calc_result = calc_result & NOT_H_MASK
 */
static const Bitboard NOT_H_MASK = Bitboard(0x7f7f7f7f7f7f7f7fULL);

// implementations of Bitboard:: bit shifts below

inline Bitboard Bitboard::nortOne() {
	return *this << 8;
}

inline Bitboard Bitboard::eastOne() {
	return (*this << 1) & NOT_A_MASK;
}

inline Bitboard Bitboard::soutOne() {
	return *this >> 8;
}

inline Bitboard Bitboard::westOne() {
	return (*this >> 1) & NOT_H_MASK;
}

inline Bitboard Bitboard::noEaOne() {
	return (*this << 9) & NOT_A_MASK;
}

inline Bitboard Bitboard::soEaOne() {
	return (*this >> 7) & NOT_A_MASK;
}

inline Bitboard Bitboard::noWeOne() {
	return (*this << 7) & NOT_H_MASK;
}

inline Bitboard Bitboard::soWeOne() {
	return (*this >> 9) & NOT_H_MASK;
}

}

#endif /* UTIL_BITBOARD_HPP_ */
