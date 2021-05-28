// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include "command_line.hpp"
#include "pdb_descr.hpp"

#include <winapi/debug.hpp>

#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

class Name2Addr : public SettingsParser {
public:
    explicit Name2Addr(int argc, char** argv) : SettingsParser{argc, argv} {
        namespace po = boost::program_options;

        visible.add_options()(
            "pdb", po::value<std::vector<PDB>>(&pdbs)->value_name("ADDR,PATH"), "load a PDB file");
        hidden.add_options()("name",
                             po::value<std::vector<std::string>>(&names)->value_name("NAME"),
                             "add a name to resolve");
        positional.add("name", -1);
    }

    const char* get_short_description() const override {
        return "[-h|--help] [--pdb ADDR,PATH]... [--] [NAME]...";
    }

    std::vector<PDB> pdbs;
    std::vector<std::string> names;
};

void dump_error(const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
}

void resolve_symbol(const winapi::Repo& repo, const std::string& name) {
    try {
        const auto address = repo.resolve_symbol(name).get_online_address();
        std::cout << winapi::format_address(address) << '\n';
    } catch (const std::exception& e) {
        dump_error(e);
        std::cout << name << '\n';
    }
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        Name2Addr settings{argc, argv};

        try {
            settings.parse(argc, argv);
        } catch (const boost::program_options::error& e) {
            settings.usage_error(e);
            return 1;
        }

        if (settings.exit_with_usage) {
            settings.usage();
            return 0;
        }

        winapi::Repo repo;

        for (const auto& pdb : settings.pdbs)
            repo.add_pdb(pdb.online_base, pdb.path);

        for (const auto& name : settings.names)
            resolve_symbol(repo, name);
    } catch (const std::exception& e) {
        dump_error(e);
        return 1;
    }
    return 0;
}
