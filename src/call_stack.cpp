// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include <pdb/all.hpp>

#include <windows.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>

namespace pdb {
namespace {

template <typename T>
static std::string put_between_brackets(const T& x) {
    std::ostringstream oss;
    oss << "[" << x << "]";
    return oss.str();
}

std::string format_address_fallback(Address addr) {
    return put_between_brackets(format_address(addr));
}

std::string offset_from(const std::string& thing, Address offset) {
    if (offset == 0)
        return put_between_brackets(thing);
    else
        return put_between_brackets(thing + "+" + format_address(offset));
}

std::string offset_from_module(const ModuleInfo& module, Address addr) {
    Address offset = 0;
    if (addr < module.get_offline_base())
        throw std::range_error{"invalid address in module"};
    offset = addr - module.get_offline_base();
    return offset_from(module.get_name(), offset);
}

std::string offset_from_symbol(const SymbolInfo& symbol) {
    return offset_from(symbol.get_name(), symbol.get_displacement());
}

std::string offset_from_symbol(const ModuleInfo& module, const SymbolInfo& symbol) {
    return offset_from(module.get_name() + "!" + symbol.get_name(), symbol.get_displacement());
}

std::string resolve_and_format(const DbgHelp& dbghelp, const SymbolInfo& symbol, Address addr) {
    try {
        const auto module = dbghelp.resolve_module(addr);
        return offset_from_symbol(module, symbol);
    } catch (const std::system_error&) {
        return offset_from_symbol(symbol);
    }
}

std::string resolve_and_format(const DbgHelp& dbghelp, Address addr) {
    try {
        const auto symbol = dbghelp.resolve_symbol(addr);
        return resolve_and_format(dbghelp, symbol, addr);
    } catch (const std::system_error&) {
        try {
            const auto module = dbghelp.resolve_module(addr);
            return offset_from_module(module, addr);
        } catch (const std::system_error&) {
            return format_address_fallback(addr);
        }
    }
    assert(false);
    return {};
}

} // namespace

CallStack CallStack::capture() {
    std::array<void*, max_length> frames_impl{nullptr};
    const auto length =
        CaptureStackBackTrace(frames_to_skip, frames_to_capture, frames_impl.data(), NULL);

    std::array<Address, max_length> frames;
    std::transform(frames_impl.cbegin(), frames_impl.cend(), frames.begin(), [](void* addr) {
        return reinterpret_cast<Address>(addr);
    });
    return {frames, length};
}

bool CallStack::for_each_address(const AddressCallback& callback) const {
    for (std::size_t i = 0; i < length; ++i) {
        if (!callback(frames[i])) {
            return false;
        }
    }
    return true;
}

std::string CallStack::pretty_print_address(const DbgHelp& dbghelp, Address addr) {
    return resolve_and_format(dbghelp, addr);
}

void CallStack::dump(std::ostream& os, const DbgHelp& dbghelp) const {
    for_each_address([&](Address addr) {
        os << format_address(addr) << ' ' << pretty_print_address(dbghelp, addr) << '\n';
        return true;
    });
}

} // namespace pdb
