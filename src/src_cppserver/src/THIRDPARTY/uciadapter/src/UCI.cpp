#include "UCI.hpp"

namespace uciadapter {

Info::Info()
    : depth(-1), seldepth(-1), multipv(-1), score_cp(-1), score_mate(-1),
      score_lowerbound(-1), score_upperbound(-1), cp(-1), nodes(-1), nps(-1),
      tbhits(-1), time(-1), hashfull(-1), cpuload(-1), currmovenumber(-1)

{}

Go::Go()
    : wtime(-1), btime(-1), winc(-1), binc(-1), movestogo(-1), depth(-1),
      nodes(-1), mate(-1), movetime(-1), infinite(false) {}

UCI::UCI(std::string engine_path) {
  INIT_PROCESS(p);
  p->Start(engine_path);
  // Init UCI
  p->Write("uci\n");
  uciok = false;
  registration_required = false;
  copyprotection_failed = false;
  registered = false;
  Sync();
  if (!uciok) {
    throw EngineError("failed to start engine in uci mode");
  }
  Sync(); // Check copy protection
  if (copyprotection_failed) {
    throw EngineError("Copy protection check failed");
  }
}

UCI::~UCI() {
  p->Kill();
  delete p;
}

void UCI::Sync() {
  p->Write("isready\n");
  bool readyok = false;
  std::string token;
  while (!readyok) {
    std::istringstream iss(p->ReadLine());
    buffer += iss.str();
    while (iss >> token) {
      if (token == "readyok") {
        readyok = true;
      } else if (token == "uciok") {
        uciok = true;
      } else if (token == "id") {
        ParseId(iss.str());
      } else if (token == "option") {
        ParseOption(iss.str());
      } else if (token == "copyprotection") {
        iss >> token;
        if (token == "error") {
          copyprotection_failed = true;
        } else if (token == "ok") {
          registered = true;
        }
      } else if (token == "registration") {
        if (iss.str() == "registration error\n")
          registration_required = true;
      } else if (token == "bestmove") {
        iss >> token;
        bestmove = token;
        iss >> token;
        if (token == "ponder") {
          iss >> token;
          ponder = token;
        }
      } else if (token == "info") {
        ParseInfo(iss.str());
      }
    }
  }
}

void UCI::Command(std::string cmd) {
  p->Write(cmd + "\n");
  Sync();
}

bool UCI::IsRegistered() { return (registered); }

void UCI::SyncAfter(int ms) {
  if (ms <= 0) {
    Sync();
  } else {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    Sync();
  }
}

void UCI::ParseInfo(std::string line) {
  Info info;
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) {
    if (token == "depth") {
      iss >> info.depth;
    } else if (token == "string") {
      std::string line;
      while (iss >> token) {
        line += token + " ";
      }
      line = line.substr(0, line.size() - 1); // Remove trailing space
      infostrings.push_back(line);
      return;
    } else if (token == "seldepth") {
      iss >> info.seldepth;
    } else if (token == "refutation") {
      while (iss >> token) {
        refutations.push_back(token);
      }
      return;
    } else if (token == "nps") {
      iss >> info.nps;
    } else if (token == "multipv") {
      iss >> info.multipv;
    } else if (token == "nodes") {
      iss >> info.nodes;
    } else if (token == "time") {
      iss >> info.time;
    } else if (token == "tbhits") {
      iss >> info.tbhits;
    } else if (token == "hashfull") {
      iss >> info.hashfull;
    } else if (token == "cpuload") {
      iss >> info.cpuload;
    } else if (token == "currmove") {
      iss >> info.currmove;
    } else if (token == "currmovenumber") {
      iss >> info.currmovenumber;
    } else if (token == "score") {
      iss >> token;
      if (token == "cp") {
        iss >> info.score_cp;
      } else if (token == "mate") {
        iss >> info.score_mate;
      } else if (token == "lowerbound") {
        iss >> info.score_lowerbound;
      } else if (token == "upperbound") {
        iss >> info.score_upperbound;
      }
    } else if (token == "pv") {
      while (iss >> token) {
        info.pv.push_back(token);
      }
    }
  }

  // Save line
  if (info.pv.size() > 0) {
    lines[info.multipv] = info;
  }
}

std::vector<std::string> UCI::GetInfoStrings() { return (infostrings); }
void UCI::ParseOption(std::string line) {
  std::istringstream iss(line);
  std::string token;
  Option opt;
  iss >> token; // option token
  std::string *entry = NULL;
  while (iss) {
    if (token == "name") {
      entry = &opt.name;
    } else if (token == "type") {
      entry = &opt.type;
    } else if (token == "default") {
      entry = &opt.default_value;
    } else if (token == "min") {
      entry = &opt.min;
    } else if (token == "max") {
      entry = &opt.max;
    } else if (token == "var") {
      entry = &opt.var;
    } else {
      iss >> token;
    }
    if (entry != NULL) {
      iss >> token;
      while (!IS_OPT_PARAM(token) && iss) {
        (*entry) += token + " ";
        iss >> token;
      }
      *entry = entry->substr(0, entry->size() - 1); // Remove trailing space
      entry = NULL;
    }
  }
  options.push_back(opt);
}

void UCI::ParseId(std::string line) {
  std::istringstream iss(line);
  std::string token;
  iss >> token; // id
  iss >> token; // name or author
  if (token == "name") {
    while (iss >> token) {
      name += token + " ";
    }
    name = name.substr(0, name.size() - 1);
  } else {
    while (iss >> token) {
      author += token + " ";
    }
    author = author.substr(0, author.size() - 1);
  }
}

std::unordered_map<int, Info> UCI::GetLines() { return (lines); }
std::vector<Option> UCI::GetOptions() { return (options); }
std::string UCI::GetName() { return (name); }
std::string UCI::GetAuthor() { return (author); }
std::string UCI::GetBuffer() { return (buffer); }
std::string UCI::GetBestMove() { return (bestmove); }

bool UCI::IsRegistrationRequired() { return (registration_required); }

void UCI::register_now(std::string name, std::string code) {
  Command("register name " + name + " code " + code);
}
void UCI::register_later() { Command("register later"); }
void UCI::stop() { Command("stop"); }
void UCI::setoption(std::string name, std::string value) {
  Command("setoption name " + name + " value " + value);
}
void UCI::debug(bool d) {
  if (d) {
    Command("debug on");
  } else {
    Command("debug off");
  }
}
void UCI::ponderhit() { Command("ponderhit"); }
void UCI::quit() {
  // Note that calling Command() will fail (because of Sync())
  p->Write("quit\n");
}
void UCI::ucinewgame() { Command("ucinewgame"); }
void UCI::position(std::string fen, std::string moves) {
  position(fen + " moves " + moves);
}
void UCI::position(std::string fen) { Command("position fen " + fen); }

void UCI::go(Go go) {
  // Flush data
  bestmove = "";
  ponder = "";
  infostrings.clear();
  lines.clear();
  refutations.clear();

  std::string cmd = "go";
  if (go.searchmoves.size() > 0)
    cmd += " searchmoves " + go.searchmoves;
  if (go.ponder.size() > 0)
    cmd += " ponder " + go.ponder;
  if (go.wtime >= 0)
    cmd += " wtime " + std::to_string(go.wtime);
  if (go.btime >= 0)
    cmd += " btime " + std::to_string(go.btime);
  if (go.winc >= 0)
    cmd += " winc " + std::to_string(go.winc);
  if (go.binc >= 0)
    cmd += " binc " + std::to_string(go.binc);
  if (go.movestogo >= 0)
    cmd += " movestogo " + std::to_string(go.movestogo);
  if (go.depth >= 0)
    cmd += " depth " + std::to_string(go.depth);
  if (go.mate >= 0)
    cmd += " mate " + std::to_string(go.mate);
  if (go.movetime >= 0)
    cmd += " movetime " + std::to_string(go.movetime);
  if (go.infinite) {
    cmd += " infinite";
  }
  Command(cmd);
}
} // namespace uciadapter