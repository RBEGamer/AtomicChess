#include "UCI.hpp"
#include <catch_amalgamated.hpp>

#define LATENCY 4000

using namespace uciadapter;

TEST_CASE("Berserk Initialisation", "[berserk/init]") {
  REQUIRE_NOTHROW([&]() { UCI("./berserk_engine"); }());

  UCI u("./berserk_engine");
  REQUIRE(u.GetName() == "Berserk 8.5.1 NN (6fb1a076a856)");
  REQUIRE(u.GetAuthor() == "Jay Honnold");

  std::vector<Option> opts = u.GetOptions();
  REQUIRE(opts.size() == 9);

  char i = 0;
  for (Option &opt : opts) {
    if (opt.name == "Hash") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "32");
      CHECK(opt.min == "4");
      CHECK(opt.max == "65536");
    } else if (opt.name == "Threads") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1");
      CHECK(opt.min == "1");
      CHECK(opt.max == "256");
    } else if (opt.name == "NoobBookLimit") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "8");
      CHECK(opt.min == "0");
      CHECK(opt.max == "32");
    } else if (opt.name == "NoobBook") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "SyzygyPath") {
      i++;
      CHECK(opt.type == "string");
      CHECK(opt.default_value == "<empty>");
    } else if (opt.name == "MultiPV") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1");
      CHECK(opt.min == "1");
      CHECK(opt.max == "256");
    } else if (opt.name == "Ponder") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "UCI_Chess960") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "MoveOverhead") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "300");
      CHECK(opt.min == "100");
      CHECK(opt.max == "10000");
    }
  }
  REQUIRE(i == 9);
}

TEST_CASE("Berserk MultiPV", "[berserk/MultiPV]") {
  UCI u("./berserk_engine");
  u.setoption("MultiPV", "5");
  u.SyncAfter(LATENCY); // Wait 2s should be enough
  CHECK(u.GetLines().size() == 0);
}

TEST_CASE("Berserk Position", "[berserk/Position]") {
  UCI u("./berserk_engine");
  u.position("2k2r2/6R1/8/8/8/6Q1/4K3/8 w - - 0 1");
  u.go(Go());
  u.SyncAfter(LATENCY); // Wait 2s should be enough
  REQUIRE(u.GetLines().size() == 1);
  CHECK(u.GetLines()[1].pv[0] == "g3c7");
  CHECK(u.GetBestMove() == "g3c7");
}