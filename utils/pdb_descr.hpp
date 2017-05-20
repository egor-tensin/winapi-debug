// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include "pdb/all.hpp"

#include <boost/program_options.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace
{
    struct PDB
    {
        pdb::Address online_base;
        std::string path;

        static PDB parse(std::string src)
        {
            static constexpr auto sep = ',';

            const auto sep_pos = src.find(sep);
            if (sep_pos == std::string::npos)
                boost::throw_exception(boost::program_options::invalid_option_value{src});

            pdb::Address online_base;
            if (!pdb::parse_address(online_base, src.substr(0, sep_pos)))
                boost::throw_exception(boost::program_options::invalid_option_value{src});

            return {online_base, src.substr(sep_pos + 1)};
        }

        static pdb::Address parse_address(const std::string& src)
        {
            pdb::Address dest;
            if (!pdb::parse_address(dest, src))
                boost::throw_exception(boost::program_options::invalid_option_value{src});
            return dest;
        }
    };
}

namespace boost
{
    namespace program_options
    {
        template <typename charT>
        void validate(
            boost::any& dest,
            const std::vector<std::basic_string<charT>>& src_tokens,
            PDB*,
            int)
        {
            validators::check_first_occurrence(dest);
            const auto& src_token = validators::get_single_string(src_tokens);
            dest = any{PDB::parse(src_token)};
        }

        template <typename charT>
        void validate(
            boost::any& dest,
            const std::vector<std::basic_string<charT>>& src_tokens,
            pdb::Address*,
            int)
        {
            validators::check_first_occurrence(dest);
            const auto& src_token = validators::get_single_string(src_tokens);
            dest = any{PDB::parse_address(src_token)};
        }
    }
}
