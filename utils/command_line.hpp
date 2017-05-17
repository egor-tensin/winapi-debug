// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#pragma once

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <ostream>
#include <string>

namespace
{
    class SettingsParser
    {
    public:
        typedef boost::program_options::options_description Options;
        typedef boost::program_options::positional_options_description Arguments;

        explicit SettingsParser(const std::string& argv0)
            : prog_name{extract_filename(argv0)}
        { }

        SettingsParser(const std::string& argv0, const Options& options)
            : prog_name{extract_filename(argv0)}
            , options{options}
        { }

        SettingsParser(const std::string& argv0, const Options& options, const Arguments& args)
            : prog_name{extract_filename(argv0)}
            , options{options}
            , args{args}
        { }

        virtual ~SettingsParser() = default;

        virtual const char* get_short_description() const { return "[OPTION]..."; }

        void parse(int argc, char* argv[]) const
        {
            boost::program_options::variables_map vm;
            boost::program_options::store(
                boost::program_options::command_line_parser{argc, argv}
                    .options(options)
                    .positional(args)
                    .run(),
                vm);
            boost::program_options::notify(vm);
        }

        void usage() const
        {
            std::cout << *this;
        }

        void usage_error(const std::exception& e) const
        {
            std::cerr << "usage error: " << e.what() << '\n';
            std::cerr << *this;
        }

    private:
        static std::string extract_filename(const std::string& path)
        {
            return boost::filesystem::path{path}.filename().string();
        }

        const std::string prog_name;
        const Options options;
        const Arguments args;

        friend std::ostream& operator<<(std::ostream&, const SettingsParser&);
    };

    std::ostream& operator<<(std::ostream& os, const SettingsParser& cmd_parser)
    {
        const auto short_descr = cmd_parser.get_short_description();
        os << "usage: " << cmd_parser.prog_name << ' ' << short_descr << '\n';
        os << cmd_parser.options;
        return os;
    }
}
