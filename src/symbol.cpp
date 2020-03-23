// Copyright (c) 2020 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <boost/nowide/convert.hpp>

#include <SafeInt.hpp>

#include <dbghelp.h>
#include <windows.h>

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>

namespace pdb {
namespace {

std::size_t calc_size(const SymbolInfo::Impl& impl) {
    try {
        return SafeInt<std::size_t>{impl.SizeOfStruct} + impl.NameLen - 1;
    } catch (const SafeIntException&) {
        throw std::runtime_error{"invalid SYMBOL_INFO size"};
    }
}

unsigned long cast_line_number(DWORD impl) {
    unsigned long dest = 0;

    if (!SafeCast(impl, dest))
        throw std::runtime_error{"invalid line number"};

    return dest;
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
    return boost::nowide::narrow(std::wstring{get_impl().Name, get_impl().NameLen});
}

LineInfo::LineInfo(const Impl& impl)
    : file_path{boost::nowide::narrow(impl.FileName)},
      line_number{cast_line_number(impl.LineNumber)} {}

} // namespace pdb
