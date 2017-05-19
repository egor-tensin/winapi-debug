// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <safeint.h>

#include <Windows.h>
#include <DbgHelp.h>

#include <cstddef>

#include <limits>
#include <stdexcept>
#include <string>

namespace pdb
{
    namespace
    {
        void enable_debug_output()
        {
            SymSetOptions(SymGetOptions() | SYMOPT_DEBUG | SYMOPT_UNDNAME);
        }

        void initialize(HANDLE id)
        {
            enable_debug_output();
            const auto ret = SymInitialize(id, NULL, FALSE);
            if (!ret)
                throw error::windows(GetLastError());
        }

        void clean_up(HANDLE id)
        {
            const auto ret = SymCleanup(id);
            if (!ret)
                throw error::windows(GetLastError());
        }

        Address next_offline_base = 0x10000000;

        Address gen_next_offline_base(std::size_t pdb_size)
        {
            const auto base = next_offline_base;
            if (!msl::utilities::SafeAdd(
                    next_offline_base,
                    pdb_size,
                    next_offline_base))
                throw std::runtime_error{"no more PDB files can be added, the internal address space is exhausted"};
            return base;
        }

        BOOL CALLBACK enum_symbols_callback(
            SYMBOL_INFO *info,
            ULONG,
            VOID *raw_callback_ptr)
        {
            const auto callback_ptr = reinterpret_cast<DbgHelp::OnSymbol*>(raw_callback_ptr);
            const auto& callback = *callback_ptr;
            callback(SymbolInfo{*info});
            return TRUE;
        }
    }

    DbgHelp::DbgHelp()
    {
        initialize(id);
    }

    ModuleInfo DbgHelp::load_pdb(const std::string& path) const
    {
        DWORD size = 0;
        if (!msl::utilities::SafeCast(file::get_size(path), size))
            throw std::range_error{"PDB file is too large"};

        const auto offline_base = SymLoadModule64(
            id,
            NULL,
            path.c_str(),
            NULL,
            gen_next_offline_base(size),
            size);

        if (!offline_base)
            throw error::windows(GetLastError());

        return get_module_info(offline_base);
    }

    ModuleInfo DbgHelp::get_module_info(Address offline_base) const
    {
        ModuleInfo info;

        const auto ret = SymGetModuleInfo64(
            id,
            offline_base,
            &static_cast<ModuleInfo::Raw&>(info));

        if (!ret)
            throw error::windows(GetLastError());

        return info;
    }

    void DbgHelp::enum_symbols(const ModuleInfo& module, const OnSymbol& callback) const
    {
        const auto ret = SymEnumSymbols(
            id,
            module.get_offline_base(),
            NULL,
            &enum_symbols_callback,
            const_cast<OnSymbol*>(&callback));

        if (!ret)
            throw error::windows(GetLastError());
    }

    SymbolInfo DbgHelp::resolve_symbol(Address online) const
    {
        Address displacement = 0;
        SymbolInfo symbol;

        const auto ret = SymFromAddr(
            id,
            online,
            &displacement,
            &static_cast<SYMBOL_INFO&>(symbol));

        if (!ret)
            throw error::windows(GetLastError());

        symbol.set_displacement(displacement);
        return symbol;
    }

    SymbolInfo DbgHelp::resolve_symbol(const std::string& name) const
    {
        SymbolInfo symbol;

        const auto ret = SymFromName(
            id,
            name.c_str(),
            &static_cast<SYMBOL_INFO&>(symbol));

        if (!ret)
            throw error::windows(GetLastError());

        return symbol;
    }

    void DbgHelp::close()
    {
        if (!closed)
        {
            clean_up(id);
            closed = true;
        }
    }

    DbgHelp::~DbgHelp()
    {
        try
        {
            close();
        }
        catch (...)
        { }
    }
}
