// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2019, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2019, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

/*!\file
 * \brief This header includes C++17 filesystem support and imports it into namespace seqan3::filesystem (independent of whether it is marked as "experimental").
 * \author Vitor C. Piro <pirov AT zedat.fu-berlin.de >
 */
#pragma once


// Temporal workaround GCC 8.1 not implementing filesystem for windows
#ifndef BOOST_FILESYSTEM_FORCE


#if __has_include(<filesystem>)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif // __has_include(experimental/filesystem)

#include <seqan3/core/platform.hpp>

namespace seqan3
{
#if __has_include(<filesystem>)
namespace filesystem = std::filesystem;
#else
namespace filesystem = std::experimental::filesystem;
#endif // __has_include(experimental/filesystem)
} // namespace seqan3


#else    // defined BOOST_FILESYSTEM_FORCE
#define _GLIBCXX_FILESYSTEM 1
#  include <chrono>

// Workaround boost assert producing warnings (at least in 1.67):
//   C:/MinGW/include/boost/mpl/assert.hpp:188:21: error: unnecessary parentheses in declaration of 'assert_arg' [-Werror=parentheses]
//    failed ************ (Pred::************
//                     ^
//   C:/MinGW/include/boost/mpl/assert.hpp:193:21: error: unnecessary parentheses in declaration of 'assert_not_arg' [-Werror=parentheses]
//    failed ************ (boost::mpl::not_<Pred>::************
// todo: report to boost?? suppress error only for those two lines
#pragma GCC diagnostic push
#  pragma GCC diagnostic warning "-Wparentheses"
#  include <boost/filesystem.hpp>
#pragma GCC diagnostic pop
namespace std {
    namespace filesystem {
        using namespace boost::filesystem;
        using file_time_type = std::chrono::time_point<std::chrono::system_clock>;

        enum class file_type {
            none      = boost::filesystem::file_type::status_unknown,
            not_found = boost::filesystem::file_type::file_not_found,
            regular   = boost::filesystem::file_type::regular_file,
            directory = boost::filesystem::file_type::directory_file,
            symlink   = boost::filesystem::file_type::symlink_file,
            block     = boost::filesystem::file_type::block_file,
            character = boost::filesystem::file_type::character_file,
            fifo      = boost::filesystem::file_type::fifo_file,
            socket    = boost::filesystem::file_type::socket_file,
            unknown   = boost::filesystem::file_type::type_unknown
        };
    } // filesystem
      // std::string operator ()(const filesystem::path& p){return p.string();}
} // std
namespace seqan3 {
    namespace filesystem = std::filesystem;
}

#endif //   BOOST_FILESYSTEM_FORCE




