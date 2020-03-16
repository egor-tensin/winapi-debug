// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "command_line.hpp"
#include "pdb/all.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {

class EnumSymbols : public SettingsParser {
public:
    explicit EnumSymbols(const std::string& argv0) : SettingsParser{argv0} {
        namespace po = boost::program_options;

        visible.add_options()("pdb",
                              po::value<std::vector<std::string>>(&pdbs)->value_name("PATH"),
                              "load a PDB file");
        visible.add_options()(
            "functions",
            po::value<pdb::symbol::Tag>(&tag)->implicit_value(function_tag)->zero_tokens(),
            "only list functions");
        visible.add_options()(
            "mask", po::value<std::string>(&symbol_mask)->value_name("MASK"), "symbol mask");
    }

    const char* get_short_description() const override {
        return "[-h|--help] [--pdb PATH]... [--functions] [--mask MASK]";
    }

    std::vector<std::string> pdbs;

    bool type_specified() const { return tag != reserved_tag; }

    pdb::Symbol::Type get_type() const { return static_cast<pdb::Symbol::Type>(tag); }

    std::string get_mask() const { return symbol_mask; }

private:
    static constexpr auto reserved_tag = static_cast<pdb::symbol::Tag>(pdb::Symbol::Type::RESERVED);
    static constexpr auto function_tag = static_cast<pdb::symbol::Tag>(pdb::Symbol::Type::Function);

    pdb::symbol::Tag tag = reserved_tag;
    std::string symbol_mask{pdb::DbgHelp::all_symbols};
};

constexpr pdb::symbol::Tag EnumSymbols::reserved_tag;
constexpr pdb::symbol::Tag EnumSymbols::function_tag;

} // namespace

int main(int argc, char* argv[]) {
    try {
        EnumSymbols settings{argv[0]};

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

        const auto dbghelp = pdb::DbgHelp::post_mortem();

        for (const auto& pdb : settings.pdbs) {
            const auto id = dbghelp.load_pdb(pdb);

            dbghelp.enum_symbols(id, settings.get_mask(), [&](const pdb::SymbolInfo& symbol) {
                if (!settings.type_specified() || settings.get_type() == symbol.get_type())
                    std::cout << symbol.get_name() << '\n';
            });
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
