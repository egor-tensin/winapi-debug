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
    class EnumSymbols : public SettingsParser
    {
    public:
        explicit EnumSymbols(const std::string& argv0)
            : SettingsParser{argv0, build_options()}
        { }

        bool exit_with_usage() const { return help_flag; }

        const char* get_short_description() const override
        {
            return "[-h|--help] [--pdb ADDR,PATH]... [--functions]";
        }

        std::vector<PDB> pdbs;

        bool type_specified() const
        {
            return tag != reserved_tag;
        }

        pdb::Symbol::Type get_type() const
        {
            return static_cast<pdb::Symbol::Type>(tag);
        }

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
                ("functions",
                    program_options::value<pdb::Symbol::Tag>(&tag)
                        ->implicit_value(function_tag)
                        ->zero_tokens(),
                    "only list functions");
            return descr;
        }

        bool help_flag = false;

        static const auto reserved_tag = static_cast<pdb::Symbol::Tag>(pdb::Symbol::Type::RESERVED);
        static const auto function_tag = static_cast<pdb::Symbol::Tag>(pdb::Symbol::Type::Function);

        pdb::Symbol::Tag tag = reserved_tag;
    };
}

int main(int argc, char* argv[])
{
    try
    {
        const EnumSymbols settings{argv[0]};

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
        {
            const auto id = repo.add_pdb(pdb.online_base, pdb.path);

            repo.enum_symbols(id, [&] (const pdb::Symbol& symbol)
            {
                if (!settings.type_specified() || settings.get_type() == symbol.get_type())
                    std::cout << symbol.get_name() << '\n';
            });
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
