/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <string>

// template<> struct char_traits<char32_t>

// static constexpr int_type eof();

#include <string>
#include <cassert>
#include "libcxx_tc_common.h"

int tc_libcxx_strings_char_traits_specializations_char32_t_eof(void)
{
#ifndef _LIBCPP_HAS_NO_UNICODE_CHARS
    std::char_traits<char32_t>::int_type i = std::char_traits<char32_t>::eof();
    ((void)i); // Prevent unused warning
#endif
    TC_SUCCESS_RESULT();
    return 0;
}
