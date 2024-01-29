#include "UCI.hpp"
#include <catch_amalgamated.hpp>

#define LATENCY 2000

using namespace uciadapter;

TEST_CASE("Stockfish Initialisation", "[stockfish/init]") {
  // Using bad engine path shoud raise an exception
  REQUIRE_THROWS([&]() { UCI("./random_path_zlkdejdlk"); }());
  REQUIRE_NOTHROW([&]() { UCI("./stockfish_engine"); }());

  UCI u("./stockfish_engine");
  REQUIRE(u.GetName() == "Stockfish 14.1");
  REQUIRE(u.GetAuthor() == "the Stockfish developers (see AUTHORS file)");

  std::vector<Option> opts = u.GetOptions();
  REQUIRE(opts.size() == 21);

  char i = 0;
  for (Option &opt : opts) {
    if (opt.name == "Debug Log File") {
      i++;
      CHECK(opt.type == "string");
      CHECK(opt.default_value.size() == 0);
    } else if (opt.name == "Threads") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1");
      CHECK(opt.min == "1");
      CHECK(opt.max == "512");
    } else if (opt.name == "Hash") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "16");
      CHECK(opt.min == "1");
      CHECK(opt.max == "33554432");
    } else if (opt.name == "Clear Hash") {
      i++;
      CHECK(opt.type == "button");
    } else if (opt.name == "Ponder") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "MultiPV") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1");
      CHECK(opt.min == "1");
      CHECK(opt.max == "500");
    } else if (opt.name == "Skill Level") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "20");
      CHECK(opt.min == "0");
      CHECK(opt.max == "20");
    } else if (opt.name == "Move Overhead") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "10");
      CHECK(opt.min == "0");
      CHECK(opt.max == "5000");
    } else if (opt.name == "Slow Mover") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "100");
      CHECK(opt.min == "10");
      CHECK(opt.max == "1000");
    } else if (opt.name == "nodestime") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "0");
      CHECK(opt.min == "0");
      CHECK(opt.max == "10000");
    } else if (opt.name == "UCI_Chess960") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "UCI_AnalyseMode") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "UCI_LimitStrength") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "UCI_Elo") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1350");
      CHECK(opt.min == "1350");
      CHECK(opt.max == "2850");
    } else if (opt.name == "UCI_ShowWDL") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "false");
    } else if (opt.name == "SyzygyPath") {
      i++;
      CHECK(opt.type == "string");
      CHECK(opt.default_value == "<empty>");
    } else if (opt.name == "SyzygyProbeDepth") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "1");
      CHECK(opt.min == "1");
      CHECK(opt.max == "100");
    } else if (opt.name == "Syzygy50MoveRule") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "true");
    } else if (opt.name == "SyzygyProbeLimit") {
      i++;
      CHECK(opt.type == "spin");
      CHECK(opt.default_value == "7");
      CHECK(opt.min == "0");
      CHECK(opt.max == "7");
    } else if (opt.name == "Use NNUE") {
      i++;
      CHECK(opt.type == "check");
      CHECK(opt.default_value == "true");
    } else if (opt.name == "EvalFile") {
      i++;
      CHECK(opt.type == "string");
      CHECK(opt.default_value == "nn-13406b1dcbe0.nnue");
    }
  }
  REQUIRE(i == 21);
}

TEST_CASE("Stockfish MultiPV", "[stockfish/MultiPV]") {
  UCI u("./stockfish_engine");
  u.setoption("MultiPV", "5");
  u.SyncAfter(LATENCY); // Wait 2s should be enough
  CHECK(u.GetLines().size() == 0);
}

TEST_CASE("Stockfish Position", "[stockfish/Position]") {
  UCI u("./stockfish_engine");
  u.position("2k2r2/6R1/8/8/8/6Q1/4K3/8 w - - 0 1");
  u.go(Go());
  u.SyncAfter(LATENCY); // Wait 2s should be enough
  REQUIRE(u.GetLines().size() == 1);
  CHECK(u.GetLines()[1].pv[0] == "g3c7");
  CHECK(u.GetBestMove() == "g3c7");
}