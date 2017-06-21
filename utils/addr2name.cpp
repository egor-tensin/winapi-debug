// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "command_line.hpp"
#include "pdb_descr.hpp"

#include "pdb/all.hpp"

#pragma warning(push, 0)
#include <boost/program_options.hpp>
#pragma warning(pop)

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    class Addr2Name : public SettingsParser
    {
    public:
        explicit Addr2Name(const std::string& argv0)
            : SettingsParser{argv0}
        {
            visible.add_options()
                ("pdb",
                    boost::program_options::value<std::vector<PDB>>(&pdbs)
                        ->value_name("ADDR,PATH"),
                    "load a PDB file")
                ("lines,l",
                    boost::program_options::bool_switch(&lines),
                    "try to resolve source files & line numbers");
            hidden.add_options()
                ("address",
                    boost::program_options::value<std::vector<pdb::Address>>(&addresses)
                        ->value_name("ADDR"),
                    "add an address to resolve");
            positional.add("address", -1);
        }

        const char* get_short_description() const override
        {
            return "[-h|--help] [--pdb ADDR,PATH]... [--] [ADDR]...";
        }

        std::vector<PDB> pdbs;
        std::vector<pdb::Address> addresses;
        bool lines = false;
    };

    std::string format_symbol(const pdb::Module& module, const pdb::Symbol& symbol)
    {
        std::ostringstream oss;
        oss << module.get_name() << '!' << symbol.get_name();
        const auto displacement = symbol.get_displacement();
        if (displacement)
            oss << '+' << pdb::format_address(displacement);
        return oss.str();
    }

    std::string format_line_info(const pdb::LineInfo& line_info)
    {
        std::ostringstream oss;
        oss << '[' << line_info.file_path << " @ " << line_info.line_number << ']';
        return oss.str();
    }

    void dump_error(const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
    }

    void resolve_symbol(const pdb::Repo& repo, pdb::Address address, bool lines = false)
    {
        try
        {
            const auto symbol = repo.resolve_symbol(address);
            const auto& module = repo.module_with_offline_base(symbol.get_offline_base());

            std::ostringstream msg;
            msg << format_symbol(module, symbol);

            if (lines)
            {
                try
                {
                    const auto line_info = repo.resolve_line(address);
                    msg << ' ' << format_line_info(line_info);
                }
                catch (const std::exception& e)
                {
                    dump_error(e);
                }
            }

            std::cout << msg.str() << '\n';
        }
        catch (const std::exception& e)
        {
            dump_error(e);
            std::cout << pdb::format_address(address) << '\n';
        }
    }
}

int main(int argc, char* argv[])
{
    try
    {
        Addr2Name settings{argv[0]};

        try
        {
            settings.parse(argc, argv);
        }
        catch (const boost::program_options::error& e)
        {
            settings.usage_error(e);
            return 1;
        }

        if (settings.exit_with_usage)
        {
            settings.usage();
            return 0;
        }

        pdb::Repo repo;

        for (const auto& pdb : settings.pdbs)
            repo.add_pdb(pdb.online_base, pdb.path);

        for (const auto& address : settings.addresses)
            resolve_symbol(repo, address, settings.lines);
    }
    catch (const std::exception& e)
    {
        dump_error(e);
        return 1;
    }
    return 0;
}
