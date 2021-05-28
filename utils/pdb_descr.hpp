// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#pragma once

#include <winapi/debug.hpp>

#include <boost/program_options.hpp>

#include <sstream>
#include <string>
#include <vector>

struct PDB {
    winapi::Address online_base;
    std::string path;

    static PDB parse(std::string src) {
        static constexpr auto sep = ',';

        const auto sep_pos = src.find(sep);
        if (sep_pos == std::string::npos)
            boost::throw_exception(boost::program_options::invalid_option_value{src});

        winapi::Address online_base;
        if (!winapi::address::parse(online_base, src.substr(0, sep_pos)))
            boost::throw_exception(boost::program_options::invalid_option_value{src});

        return {online_base, src.substr(sep_pos + 1)};
    }

    static winapi::Address parse_address(const std::string& src) {
        winapi::Address dest;
        if (!winapi::address::parse(dest, src))
            boost::throw_exception(boost::program_options::invalid_option_value{src});
        return dest;
    }
};

template <typename charT>
void validate(boost::any& dest,
              const std::vector<std::basic_string<charT>>& src_tokens,
              PDB*,
              int) {
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(dest);
    const auto& src_token = po::validators::get_single_string(src_tokens);
    dest = boost::any{PDB::parse(src_token)};
}

template <typename charT>
void validate(boost::any& dest,
              const std::vector<std::basic_string<charT>>& src_tokens,
              winapi::Address*,
              int) {
    namespace po = boost::program_options;
    po::validators::check_first_occurrence(dest);
    const auto& src_token = po::validators::get_single_string(src_tokens);
    dest = boost::any{PDB::parse_address(src_token)};
}
