// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "command_line.hpp"
#include "pdb_descr.hpp"

#include "pdb/all.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    class Name2Addr : public SettingsParser
    {
    public:
        explicit Name2Addr(const std::string& argv0)
            : SettingsParser{argv0, build_options(), build_args()}
        { }

        bool exit_with_usage() const { return help_flag; }

        const char* get_short_description() const override
        {
            return "[-h|--help] [--pdb ADDR,PATH]... [--] [NAME]...";
        }

        std::vector<PDB> pdbs;

        std::vector<std::string> names;

    private:
        Options build_options()
        {
            namespace program_options = boost::program_options;
            Options descr{"options"};
            descr.add_options()
                ("help,h",
                    program_options::bool_switch(&help_flag),
                    "show this message and exit")
                ("pdb",
                    program_options::value<std::vector<PDB>>(&pdbs)
                        ->value_name("ADDR,PATH"),
                    "load a PDB file")
                ("name",
                    program_options::value<std::vector<std::string>>(&names)
                        ->value_name("NAME"),
                    "add a name to resolve");
            return descr;
        }

        static Arguments build_args()
        {
            Arguments descr;
            descr.add("name", -1);
            return descr;
        }

        bool help_flag = false;
    };

    std::string format_address(pdb::Address address)
    {
        std::ostringstream oss;
        oss << std::showbase << std::hex << address;
        return oss.str();
    }

    void dump_error(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
    }

    void resolve_symbol(const pdb::Repo& repo, const std::string& name)
    {
        try
        {
            const auto address = repo.resolve_symbol(name).get_online_address();
            std::cout << format_address(address) << '\n';
        }
        catch (const std::exception& e)
        {
            dump_error(e);
            std::cout << name << '\n';
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        const Name2Addr settings{argv[0]};

        try
        {
            settings.parse(argc, argv);
        }
        catch (const boost::program_options::error& e)
        {
            settings.usage_error(e);
            return 1;
        }

        if (settings.exit_with_usage())
        {
            settings.usage();
            return 0;
        }

        pdb::Repo repo;

        for (const auto& pdb : settings.pdbs)
            repo.add_pdb(pdb.online_base, pdb.path);

        for (const auto& name : settings.names)
            resolve_symbol(repo, name);
    }
    catch (const std::exception& e)
    {
        dump_error(e);
        return 1;
    }
    return 0;
}
