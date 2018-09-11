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

TEST_CASE("Get from 64-bit src.", "[getBits]") {
    uint64_t src = 0xA000000000000F45;
    REQUIRE(getSbits<3, 61>(src) == -3);
    REQUIRE(getUbits<3, 61>(src) == 5);
    REQUIRE(getBits<4, 8, State>(src) == State::ON);
    REQUIRE(getBits<4, 8, Ustate>(src) == Ustate::ON);
}

TEST_CASE("Get from a 16-bit src.", "[getBits]"){
    uint16_t src = 0xA045;
    REQUIRE(getSbits<3, 13>(src) == -3);
    REQUIRE(getSbits<3, 0>(src) == -3);
    REQUIRE(getUbits<3, 0>(src) == 5);
    REQUIRE(getBits<4, 0, State>(src) == State::CA);
    REQUIRE(getBits<4, 0, Ustate>(src) == Ustate::CA);
}

TEST_CASE("Get from an 8-bit src.", "[getBits]"){
    uint8_t src = 0x45;
    REQUIRE(getSbits<3, 0>(src) == -3);
    REQUIRE(getUbits<3, 0>(src) == 5);
    REQUIRE(getSbits<3, 5>(src) == 2);
    REQUIRE(getBits<4, 0, State>(src) == State::CA);
    REQUIRE(getBits<4, 0, Ustate>(src) == Ustate::CA);
}

TEST_CASE("Set three fields, dest = 0, uint64_t.", "[setBits]") {
    uint64_t dest = 0;
    setBits<3, 61>(dest, 0xFFFFFFFFFFFFFFFF);
    REQUIRE(dest == 0xE000000000000000);
    int32_t value = -1;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0xE000000000000007);
    setBits<4, 5>(dest, State::SuperconductiveAtRoomTemperature);
    REQUIRE(dest == 0xE000000000000147);
}

TEST_CASE("Set three fields, dest = 0xFFFFFFFFFFFFFFFF, uint64_t.", "[setBits]") {
    uint64_t dest = 0xFFFFFFFFFFFFFFFF;
    setBits<3, 61>(dest, 0);
    REQUIRE(dest == 0x1FFFFFFFFFFFFFFF);
    setBits<3, 2>(dest, 0);
    REQUIRE(dest == 0x1FFFFFFFFFFFFFE3);
    int32_t value = -4;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0x1FFFFFFFFFFFFFE4);
}

TEST_CASE("Set three fields, dest = 0, uint8_t.", "[setBits]") {
    uint8_t dest = 0;
    setBits<3, 5>(dest, 0xFF);
    REQUIRE(dest == 0xE0);
    int32_t value = -1;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0xE7);
    setBits<4, 3>(dest, State::SuperconductiveAtRoomTemperature);
    REQUIRE(dest == 0xD7);
}

TEST_CASE("Set three fields, dest = 0xFF, uint8_t.", "[setBits]") {
    uint8_t dest = 0xFF;
    setBits<3, 5>(dest, 0);
    REQUIRE(dest == 0x1F);
    setBits<3, 2>(dest, 0);
    REQUIRE(dest == 0x03);
    int32_t value = -4;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0x04);
}

TEST_CASE("Set three fields, dest = 0, uint16_t.", "[setBits]") {
    uint16_t dest = 0;
    setBits<3, 13>(dest, 0xFFFF);
    REQUIRE(dest == 0xE000);
    int32_t value = -1;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0xE007);
    setBits<4, 3>(dest, State::SuperconductiveAtRoomTemperature);
    REQUIRE(dest == 0xE057);
}


TEST_CASE("Set three fields, dest = 0xFFFF, uint16_t.", "[setBits]") {
    uint16_t dest = 0xFFFF;
    setBits<3, 13>(dest, 0);
    REQUIRE(dest == 0x1FFF);
    setBits<3, 2>(dest, 0);
    REQUIRE(dest == 0x1FE3);
    int32_t value = -4;
    setBits<3, 0>(dest, value);
    REQUIRE(dest == 0x1FE4);
}


