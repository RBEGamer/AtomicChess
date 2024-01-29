#include "pgn-exception.hh"

namespace pgn_parser
{
    PgnParsingException::PgnParsingException(const std::string& message,
                                             const std::string& token) noexcept
        : message_(message)
        , token_(token)
        , full_message_("pgn parsing error: " + message + " @ token " + token)
    {}

    const char* PgnParsingException::what() const noexcept
    {
        return full_message_.c_str();
    }
} // namespace pgn_parser
