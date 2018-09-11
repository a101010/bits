#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "bits.hpp"
#include <cinttypes>

using namespace bits;

TEST_CASE("Set a bit.", "[setBit]") {
	uint32_t dest = 0;
	setBit(dest, 2, true);
	REQUIRE(dest == 0x0004);
}

TEST_CASE("Clear a bit.", "[setBit]") {
	uint32_t dest = ~0;
	setBit(dest, 2, false);
	REQUIRE(dest == 0xFFFFFFFB);
}

TEST_CASE("Read some individual bits.", "[getBit]") {
	uint32_t src = 0x80000001;
	REQUIRE(getBit(src, 31) == true);
	REQUIRE(getBit(src, 30) == false);
	REQUIRE(getBit(src, 0) == true);
	REQUIRE(getBit(src, 1) == false);
}

enum class State {
	ON = -1,
	OFF = 0,
	Frozen = 1,
	AK,
	Panic,
	Gas,
	CA,
	TX,
	UT,
	Plasma,
	HI,
	SuperconductiveAtRoomTemperature
};

enum class Ustate : uint32_t {
    ON = 0xF,
    OFF = 0,
    Frozen = 1,
    AK,
    Panic,
    Gas,
    CA,
    TX,
    UT,
    Plasma,
    HI,
    SuperconductiveAtRoomTemperature
};

TEST_CASE("Set three fields, dest = 0.", "[setBits]") {
	uint32_t dest = 0;
	setBits<3, 29>(dest, 0xFFFFFFFF);
	REQUIRE(dest == 0xE0000000);
	int32_t value = -1;
	setBits<3, 0>(dest, value);
	REQUIRE(dest == 0xE0000007);
	setBits<4, 5>(dest, State::SuperconductiveAtRoomTemperature);
	REQUIRE(dest == 0xE0000147);
}

TEST_CASE("Set three fields, dest = 0xFFFFFFFF.", "[setBits]") {
	uint32_t dest = 0xFFFFFFFF;
	setBits<3, 29>(dest, 0);
	REQUIRE(dest == 0x1FFFFFFF);
	setBits<3, 2>(dest, 0);
	REQUIRE(dest == 0x1FFFFFE3);
	int32_t value = -4;
	setBits<3, 0>(dest, value);
	REQUIRE(dest == 0x1FFFFFE4);
}

TEST_CASE("Get two unsigned fields.", "[getUbits]") {
	uint32_t src = 0xA0000F45;
	REQUIRE(getUbits<3, 29>(src) == 5);
	REQUIRE(getUbits<9,  0>(src) == 0x145);
}

TEST_CASE("Get three signed fields.", "[getSbits<uint32_t, int32_t>]") {
	uint32_t src = 0xA0000F45;
	REQUIRE(getSbits<3, 29>(src) == -3);
	REQUIRE(getSbits<3,  0>(src) == -3);
	REQUIRE(getSbits<9,  0>(src) == -187);
}

TEST_CASE("Set three fields, dest = 0." "[setBits]") {
    uint32_t storage = 0;
    setBits<3, 29>(storage, 0XFFFFFFFF);
    REQUIRE(storage == 0xE0000000);
    setBits<3,  0>(storage, -1);
    REQUIRE(storage == 0xE0000007);
    setBits<4,  5>(storage, State::SuperconductiveAtRoomTemperature);
    REQUIRE(storage == 0xE0000147);
}

TEST_CASE("Set unsigned enum field, dest = 0." "[setBits]") {
    uint32_t storage = 0;
    setBits<4, 5>(storage, Ustate::SuperconductiveAtRoomTemperature);
    REQUIRE(storage == 0x00000140);
}

TEST_CASE("Get enum fields.", "[getBits]") {
    uint32_t src = 0xA0000F45;
    REQUIRE(getBits<4, 8, State>(src) == State::ON);
    REQUIRE(getBits<4, 8, Ustate>(src) == Ustate::ON);
}

TEST_CASE("type of ValueIntType made from uint64_t", "[uint64_t]") {
    uint64_t src = 1;
    REQUIRE(sizeof(src) * bits::BITS_IN_BYTE == 64);
    REQUIRE(src << 63 == 0x8000000000000000);
    typename std::make_signed<uint64_t>::type signed_src = 1;
    REQUIRE(sizeof(signed_src) * bits::BITS_IN_BYTE == 64);
    REQUIRE(static_cast<uint64_t>(signed_src << 63) == 0x8000000000000000);
}

TEST_CASE("Get from 64-bit src.", "[getBits]") {
    uint64_t src = 0xA000000000000F45;
    REQUIRE(getSbits<3, 61>(src) == -3);
    REQUIRE(getUbits<3, 61>(src) == 5);
    REQUIRE(getBits<4, 8, State>(src) == State::ON);
    REQUIRE(getBits<4, 8, Ustate>(src) == Ustate::ON);
}

TEST_CASE("Get from an 8-bit src.", "[getBits]"){
    uint8_t src = 0x45;
    REQUIRE(getSbits<3, 0>(src) == -3);
    REQUIRE(getUbits<3, 0>(src) == 5);
    REQUIRE(getBits<4, 0, State>(src) == State::CA);
    REQUIRE(getBits<4, 0, Ustate>(src) == Ustate::CA);
}




