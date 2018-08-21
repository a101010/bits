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

TEST_CASE("Set two fields, dest = 0.", "[setBits<uint32_t, uint32_t> setBits<uint32_t, int32_t>]") {
	uint32_t dest = 0;
	setBits(dest, width_lsb<uint32_t>(3, 29), 0xFFFFFFFF);
	REQUIRE(dest == 0xE0000000);
	int32_t value = -1;
	setBits(dest, width_lsb<uint32_t>(3, 0), value);
	REQUIRE(dest == 0xE0000007);
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
