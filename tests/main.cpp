#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "bits.hpp"
#include <cinttypes>

using namespace bits;

TEST_CASE("Set a bit.", "[setBit<uint32_t>]") {
	uint32_t dest = 0;
	setBit(dest, 2, true);
	REQUIRE(dest == 0x0004);
}

TEST_CASE("Clear a bit.", "[setBit<uint32_t>]") {
	uint32_t dest = ~0;
	setBit(dest, 2, false);
	REQUIRE(dest == 0xFFFFFFFB);
}

TEST_CASE("Read some individual bits.", "[getBit<uint32_t>]") {
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

TEST_CASE("Set three fields, dest = 0.", "[setBits<uint32_t, uint32_t> setBits<uint32_t, int32_t>]") {
	uint32_t dest = 0;
	setBits(dest, width_lsb<uint32_t>(3, 29), 0xFFFFFFFF);
	REQUIRE(dest == 0xE0000000);
	int32_t value = -1;
	setBits(dest, width_lsb<uint32_t>(3, 0), value);
	REQUIRE(dest == 0xE0000007);
	setBits(dest, width_lsb<uint32_t>(4, 5), State::SuperconductiveAtRoomTemperature);
	REQUIRE(dest == 0xE0000147);
}

TEST_CASE("Set three fields, dest = 0xFFFFFFFF.", "[setBits<uint32_t, uint32_t> setBits<uint32_t, int32_t>]") {
	uint32_t dest = 0xFFFFFFFF;
	setBits(dest, width_lsb<uint32_t>(3, 29), 0);
	REQUIRE(dest == 0x1FFFFFFF);
	setBits(dest, width_lsb<uint32_t>(3, 2), 0);
	REQUIRE(dest == 0x1FFFFFE3);
	int32_t value = -4;
	setBits(dest, width_lsb<uint32_t>(3, 0), value);
	REQUIRE(dest == 0x1FFFFFE4);
}

TEST_CASE("Get two unsigned fields.", "[getUbits<uint32_t>]") {
	uint32_t src = 0xA0000F45;
	REQUIRE(getUbits(src, width_lsb<uint32_t>(3, 29)) == 5);
	REQUIRE(getUbits(src, width_lsb<uint32_t>(9, 0)) == 0x145);
}

TEST_CASE("Get two signed fields.", "[getSbits<uint32_t, int32_t>]") {
	uint32_t src = 0xA0000F45;
	REQUIRE(getSbits<int32_t>(src, w_lsb<uint32_t>(3, 29)) == -3);
	REQUIRE(getSbits<int32_t>(src, w_lsb<uint32_t>(9, 0)) == -187);
}

TEST_CASE("Set three fields, dest = 0." "[SBits::Set and UBits::SET]") {
    uint32_t storage = 0;
    UBits<3, 29, uint32_t, uint32_t>::Set(storage, 0XFFFFFFFF);
    REQUIRE(storage == 0xE0000000);
    UBits<3,  0, uint32_t, uint32_t>::Set(storage, 0XFFFFFFFF);
    REQUIRE(storage == 0xE0000007);
    setBits<4, 5>(storage, State::SuperconductiveAtRoomTemperature);
    REQUIRE(storage == 0xE0000147);

}

TEST_CASE("Set signed enum fields, dest = 0." "[EBits::Set]") {
    uint32_t storage = 0;
    setBits<4, 5>(storage, State::SuperconductiveAtRoomTemperature);
    REQUIRE(storage == 0x00000140);
}

TEST_CASE("Set unsigned enum fields, dest = 0." "[EBits::Set]") {
    uint32_t storage = 0;
    setBits<4, 5>(storage, Ustate::SuperconductiveAtRoomTemperature);
    REQUIRE(storage == 0x00000140);
}

TEST_CASE("Get three signed fields.", "[getSbits]") {
    uint32_t src = 0xA0000F45;
    REQUIRE(SBits<3,29, int32_t, uint32_t>::Get(src) == -3);
    REQUIRE(SBits<9, 0, int32_t, uint32_t>::Get(src) == -187);
    REQUIRE(SBits<4, 8, State, uint32_t>::Get(src) == State::ON);
    REQUIRE(getEbits<4, 8, State>(src) == State::ON);
    REQUIRE(getEbits<4, 8, Ustate>(src) == Ustate::ON);
}

