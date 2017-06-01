// Copyright (c) 2017 Egor Tensin <Egor.Tensin@gmail.com>
// This file is part of the "PDB repository" project.
// For details, see https://github.com/egor-tensin/pdb-repo.
// Distributed under the MIT License.

#include "pdb/all.hpp"

#include <safeint.h>

#include <Windows.h>
#pragma warning(push, 0)
#include <DbgHelp.h>
#pragma warning(pop, 0)

#include <cstddef>
#include <cstring>

#include <stdexcept>
#include <string>

namespace pdb
{
    namespace
    {
        void set_dbghelp_options()
        {
            SymSetOptions(SymGetOptions()
                | SYMOPT_DEBUG
                | SYMOPT_LOAD_LINES
                | SYMOPT_UNDNAME);
        }

        void initialize(HANDLE id)
        {
            set_dbghelp_options();

            if (!SymInitialize(id, NULL, FALSE))
                throw error::windows(GetLastError());
        }

        void clean_up(HANDLE id)
        {
            if (!SymCleanup(id))
                throw error::windows(GetLastError());
        }

        Address next_offline_base = 0x10000000;

        Address gen_next_offline_base(std::size_t pdb_size)
        {
            const auto base = next_offline_base;
            using msl::utilities::SafeAdd;
            if (!SafeAdd(next_offline_base, pdb_size, next_offline_base))
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

    DbgHelp::~DbgHelp()
    {
        try
        {
            close();
        }
        catch (...)
        { }
    }

    void DbgHelp::close()
    {
        if (!closed)
        {
            clean_up(id);
            closed = true;
        }
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

        if (!SymGetModuleInfo64(
                id,
                offline_base,
                &static_cast<ModuleInfo::Raw&>(info)))
            throw error::windows(GetLastError());

        return info;
    }

    void DbgHelp::enum_symbols(const ModuleInfo& module, const OnSymbol& callback) const
    {
        if (!SymEnumSymbols(
                id,
                module.get_offline_base(),
                NULL,
                &enum_symbols_callback,
                const_cast<OnSymbol*>(&callback)))
            throw error::windows(GetLastError());
    }

    SymbolInfo DbgHelp::resolve_symbol(Address offline) const
    {
        Address displacement = 0;
        SymbolInfo symbol;

        if (!SymFromAddr(
                id,
                offline,
                &displacement,
                &static_cast<SYMBOL_INFO&>(symbol)))
            throw error::windows(GetLastError());

        symbol.set_displacement(displacement);
        return symbol;
    }

    SymbolInfo DbgHelp::resolve_symbol(const std::string& name) const
    {
        SymbolInfo symbol;

        if (!SymFromName(
                id,
                name.c_str(),
                &static_cast<SYMBOL_INFO&>(symbol)))
            throw error::windows(GetLastError());

        return symbol;
    }

    LineInfo DbgHelp::resolve_line(Address offline) const
    {
        IMAGEHLP_LINE64 raw;
        std::memset(&raw, 0, sizeof(raw));
        raw.SizeOfStruct = sizeof(raw);

        DWORD displacement = 0;

        if (!SymGetLineFromAddr64(
                id,
                offline,
                &displacement,
                &raw))
            throw error::windows(GetLastError());

        return LineInfo{raw};
    }
}
