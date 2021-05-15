// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "winapi-debug" project.
// For details, see https://github.com/egor-tensin/winapi-debug.
// Distributed under the MIT License.

#include <pdb/all.hpp>

#include <winapi/utf8.hpp>

#include <dbghelp.h>
#include <windows.h>

#include <cstddef>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace pdb {
namespace {

std::size_t calc_size(const SymbolInfo::Impl& impl) {
    static constexpr auto char_size = sizeof(std::remove_extent<decltype(impl.Name)>::type);
    return impl.SizeOfStruct + (impl.NameLen - 1) * char_size;
}

unsigned long cast_line_number(DWORD impl) {
    unsigned long dest = 0;

    if (impl > std::numeric_limits<decltype(dest)>::max())
        throw std::runtime_error{"invalid line number"};

    return static_cast<decltype(dest)>(dest);
}

} // namespace

SymbolInfo::SymbolInfo() {
    buffer.fill(0);
    get_impl().SizeOfStruct = sizeof(Impl);
    get_impl().MaxNameLen = MAX_SYM_NAME;
}

SymbolInfo::SymbolInfo(const Impl& impl) : SymbolInfo{} {
    if (impl.SizeOfStruct != sizeof(impl))
        throw std::runtime_error{"invalid SYMBOL_INFO.SizeOfStruct"};
    const auto impl_size = calc_size(impl);
    if (impl_size > buffer.size())
        throw std::runtime_error{"SYMBOL_INFO is too large"};
    std::memcpy(buffer.data(), &impl, impl_size);
}

std::string SymbolInfo::get_name() const {
    // SymFromAddrW, contrary to SymFromAddrA, seems to include the terminating
    // null character in NameLen.
    return winapi::narrow(get_impl().Name);
}

LineInfo::LineInfo(const Impl& impl)
    : file_path{winapi::narrow(impl.FileName)}, line_number{cast_line_number(impl.LineNumber)} {}

} // namespace pdb
