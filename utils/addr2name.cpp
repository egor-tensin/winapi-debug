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
#include <string>
#include <vector>

namespace
{
    class Addr2Name : public SettingsParser
    {
    public:
        explicit Addr2Name(const std::string& argv0)
            : SettingsParser{argv0, build_options(), build_args()}
        { }

        bool exit_with_usage() const { return help_flag; }

        const char* get_short_description() const override
        {
            return "[-h|--help] [--pdb ADDR,PATH]... [--] [ADDR]...";
        }

        std::vector<PDB> pdbs;

        std::vector<pdb::Address> addresses;

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
                ("address",
                    program_options::value<std::vector<pdb::Address>>(&addresses)
                        ->value_name("ADDR"),
                    "add an address to resolve");
            return descr;
        }

        static Arguments build_args()
        {
            Arguments descr;
            descr.add("address", -1);
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

    std::string format_symbol(const pdb::Symbol& symbol)
    {
        std::ostringstream oss;
        oss << symbol.get_name();
        const auto displacement = symbol.get_displacement();
        if (displacement)
            oss << '+' << format_address(displacement);
        return oss.str();
    }

    void dump_error(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
    }

    void resolve_symbol(const pdb::Repo& repo, pdb::Address address)
    {
        try
        {
            std::cout << format_symbol(repo.resolve_symbol(address)) << '\n';
        }
        catch (const std::exception& e)
        {
            dump_error(e);
            std::cout << format_address(address) << '\n';
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        const Addr2Name settings{argv[0]};

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

        for (const auto& address : settings.addresses)
            resolve_symbol(repo, address);
    }
    catch (const std::exception& e)
    {
        dump_error(e);
        return 1;
    }
    return 0;
}
