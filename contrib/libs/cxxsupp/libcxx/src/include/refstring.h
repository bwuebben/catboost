//===------------------------ __refstring ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_REFSTRING_H
#define _LIBCPP_REFSTRING_H

#include <__config>
#include <stdexcept>
#include <cstddef>
#include <cstring>
#ifdef __APPLE__
#include <dlfcn.h>
#include <mach-o/dyld.h>
#endif

#if !defined(_LIBCPP_HAS_NO_THREADS) && !defined(_LIBCPP_CXX03_LANG) && !defined(_LIBCPP_USE_ATOMIC)
#define _LIBCPP_USE_ATOMIC
#include <atomic>
#else
#include "atomic_support.h"
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

namespace __refstring_imp { namespace {
typedef int count_t;

struct _Rep_base {
    std::size_t len;
    std::size_t cap;
#ifdef _LIBCPP_USE_ATOMIC
    std::atomic<count_t> count;
#else
    count_t     count;
#endif
};

inline _Rep_base* rep_from_data(const char *data_) noexcept {
    char *data = const_cast<char *>(data_);
    return reinterpret_cast<_Rep_base *>(data - sizeof(_Rep_base));
}

inline char * data_from_rep(_Rep_base *rep) noexcept {
    char *data = reinterpret_cast<char *>(rep);
    return data + sizeof(*rep);
}

#if defined(__APPLE__)
inline
const char* compute_gcc_empty_string_storage() _NOEXCEPT
{
    void* handle = dlopen("/usr/lib/libstdc++.6.dylib", RTLD_NOLOAD);
    if (handle == nullptr)
        return nullptr;
    void* sym = dlsym(handle, "_ZNSs4_Rep20_S_empty_rep_storageE");
    if (sym == nullptr)
        return nullptr;
    return data_from_rep(reinterpret_cast<_Rep_base *>(sym));
}

inline
const char*
get_gcc_empty_string_storage() _NOEXCEPT
{
    static const char* p = compute_gcc_empty_string_storage();
    return p;
}
#endif

}} // namespace __refstring_imp

using namespace __refstring_imp;

inline
__libcpp_refstring::__libcpp_refstring(const char* msg) {
    std::size_t len = strlen(msg);
    _Rep_base* rep = static_cast<_Rep_base *>(::operator new(sizeof(*rep) + len + 1));
    rep->len = len;
    rep->cap = len;
    rep->count = 0;
    char *data = data_from_rep(rep);
    std::memcpy(data, msg, len + 1);
    __imp_ = data;
}

inline
__libcpp_refstring::__libcpp_refstring(const __libcpp_refstring &s) _NOEXCEPT
    : __imp_(s.__imp_)
{
    if (__uses_refcount())
#ifdef _LIBCPP_USE_ATOMIC
        rep_from_data(__imp_)->count.fetch_add(1);
#else
        __libcpp_atomic_add(&rep_from_data(__imp_)->count, 1);
#endif
}

inline
__libcpp_refstring& __libcpp_refstring::operator=(__libcpp_refstring const& s) _NOEXCEPT {
    bool adjust_old_count = __uses_refcount();
    struct _Rep_base *old_rep = rep_from_data(__imp_);
    __imp_ = s.__imp_;
    if (__uses_refcount())
#ifdef _LIBCPP_USE_ATOMIC
        rep_from_data(__imp_)->count.fetch_add(1);
#else
        __libcpp_atomic_add(&rep_from_data(__imp_)->count, 1);
#endif

    if (adjust_old_count)
    {
#ifdef _LIBCPP_USE_ATOMIC
        if (old_rep->count.fetch_sub(1) == 0)
#else
        if (__libcpp_atomic_add(&old_rep->count, count_t(-1)) < 0)
#endif
        {
            ::operator delete(old_rep);
        }
    }
    return *this;
}

inline
__libcpp_refstring::~__libcpp_refstring() {
    if (__uses_refcount()) {
        _Rep_base* rep = rep_from_data(__imp_);
#ifdef _LIBCPP_USE_ATOMIC
        if (rep->count.fetch_sub(1) == 0) {
#else
        if (__sync_add_and_fetch(&rep->count, count_t(-1)) < 0) {
#endif
            ::operator delete(rep);
        }
    }
}

inline
bool __libcpp_refstring::__uses_refcount() const {
#ifdef __APPLE__
    return __imp_ != get_gcc_empty_string_storage();
#else
    return true;
#endif
}

_LIBCPP_END_NAMESPACE_STD

#endif //_LIBCPP_REFSTRING_H
