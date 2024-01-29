#pragma once

#include <exception>
#include <string>
#include <vector>

namespace pgn_parser
{
    class PgnParsingException : public std::exception
    {
    public:
        PgnParsingException(const std::string& message,
                            const std::string& token) noexcept;

        virtual ~PgnParsingException() noexcept = default;

        virtual const char* what() const noexcept override;

    private:
        const std::string message_;
        const std::string token_;
        const std::string full_message_;
    };
} // namespace pgn_parser
