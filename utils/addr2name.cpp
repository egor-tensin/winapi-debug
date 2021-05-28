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
#include <sstream>
#include <string>
#include <vector>

namespace {

class Addr2Name : public SettingsParser {
public:
    explicit Addr2Name(int argc, char** argv) : SettingsParser{argc, argv} {
        namespace po = boost::program_options;

        visible.add_options()(
            "pdb", po::value<std::vector<PDB>>(&pdbs)->value_name("ADDR,PATH"), "load a PDB file");
        visible.add_options()(
            "lines,l", po::bool_switch(&lines), "try to resolve source files & line numbers");
        hidden.add_options()(
            "address",
            po::value<std::vector<winapi::Address>>(&addresses)->value_name("ADDR"),
            "add an address to resolve");
        positional.add("address", -1);
    }

    const char* get_short_description() const override {
        return "[-h|--help] [--pdb ADDR,PATH]... [-l|--lines] [--] [ADDR]...";
    }

    std::vector<PDB> pdbs;
    std::vector<winapi::Address> addresses;
    bool lines = false;
};

std::string format_symbol(const winapi::Module& module, const winapi::Symbol& symbol) {
    std::ostringstream oss;
    oss << module.get_name() << '!' << symbol.get_name();
    const auto displacement = symbol.get_displacement();
    if (displacement)
        oss << '+' << winapi::address::format(displacement);
    return oss.str();
}

std::string format_line_info(const winapi::LineInfo& line_info) {
    std::ostringstream oss;
    oss << '[' << line_info.file_path << " @ " << line_info.line_number << ']';
    return oss.str();
}

void dump_error(const std::exception& e) {
    std::cerr << "error: " << e.what() << '\n';
}

void resolve_symbol(const winapi::PostMortem& analysis,
                    winapi::Address address,
                    bool lines = false) {
    try {
        const auto symbol = analysis.resolve_symbol(address);
        const auto& module = analysis.module_with_offline_base(symbol.get_offline_base());

        std::ostringstream msg;
        msg << format_symbol(module, symbol);

        if (lines) {
            try {
                const auto line_info = analysis.resolve_line(address);
                msg << ' ' << format_line_info(line_info);
            } catch (const std::exception& e) {
                dump_error(e);
            }
        }

        std::cout << msg.str() << '\n';
    } catch (const std::exception& e) {
        dump_error(e);
        std::cout << winapi::address::format(address) << '\n';
    }
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        Addr2Name settings{argc, argv};

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

        winapi::PostMortem analysis;

        for (const auto& pdb : settings.pdbs)
            analysis.add_pdb(pdb.online_base, pdb.path);

        for (const auto& address : settings.addresses)
            resolve_symbol(analysis, address, settings.lines);
    } catch (const std::exception& e) {
        dump_error(e);
        return 1;
    }
    return 0;
}
