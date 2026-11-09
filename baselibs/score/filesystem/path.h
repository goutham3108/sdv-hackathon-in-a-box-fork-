/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#ifndef SCORE_LIB_FILESYSTEM_PATH_H
#define SCORE_LIB_FILESYSTEM_PATH_H

#include <cstdint>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <vector>

namespace score
{
namespace filesystem
{
namespace detail
{

template <class Source>
using UnderlyingTypeFromPointer = typename std::remove_const_t<typename std::remove_pointer_t<Source>>;

template <class Source>
using IsCharacter = typename std::is_same<Source, char>;

template <class Source>
using IsStringView = typename std::is_same<typename Source::value_type, char>;

template <typename Source, class enable = void>
struct CanBeInterpretedAsPath : std::false_type
{
};

template <typename Source>
struct CanBeInterpretedAsPath<
    Source,
    typename std::enable_if_t<IsCharacter<UnderlyingTypeFromPointer<std::decay_t<Source>>>::value>> : std::true_type
{
};

template <typename Source>
struct CanBeInterpretedAsPath<Source, typename std::enable_if_t<IsStringView<Source>::value>> : std::true_type
{
};

}  // namespace detail

/// \brief Objects of type path represent paths on a filesystem. Only syntactic aspects of paths are handled: the
/// pathname may represent a non-existing path or even one that is not allowed to exist on the current file system or
/// OS.
///
/// \details We try to implement major parts of https://en.cppreference.com/w/cpp/filesystem/path .
///
/// It shall be noted that really only parts are implemented, if you miss some functionality. Add it!
///
/// Attention, please note that we right now only support POSIX paths! No Windows or Network paths!
class Path final
{
  public:
    /// \brief character type used by the native encoding of the filesystem
    ///
    /// \details On POSIX that is char but in a none POSIX environment this might have to be different
    using value_type = char;

    using string_type = std::basic_string<value_type>;

    /// \brief alternative directory separator
    // coverity[autosar_cpp14_a0_1_1_violation] false-positive, assigned value is being used
    constexpr static value_type preferred_separator = '/';

    constexpr static std::string_view dot = ".";
    constexpr static std::string_view dotdot = "..";

    /// \brief Determines how string representations of path names are interpreted by the constructors of
    /// score::filesystem::Path that accept strings.
    ///
    /// \details On POSIX systems, there is no difference between native and generic format.
    enum class Format : std::uint16_t
    {
        kNativeFormat,
        kGenericFormat,
        kAutoFormat,  // Implementation-defined pathname format, auto-detected where possible
    };

    /// \brief Constructs an empty path.
    Path() noexcept;

    /// \brief Copy constructor. Constructs a path whose pathname, in both native and generic formats, is the same as
    /// that of p
    Path(const Path&) noexcept;

    /// \brief  Move constructor. Constructs a path whose pathname, in both native and generic formats, is the same as
    /// that of p, p is left in valid but unspecified state.
    Path(Path&&) noexcept;

    /// \brief Constructs the path from a character sequence (format interpreted as specified by fmt) provided by
    /// source, which is a pointer or an input iterator to a null-terminated character/wide character sequence
    ///
    /// \details In difference to std::filesystem::path, we only allow char based strings (only ASCI)
    // NOLINTNEXTLINE(google-explicit-constructor): We allow implicit conversion to follow std::filesystem::path
    Path(const string_type& user_path, const Format format = Format::kAutoFormat) noexcept;

    /// \brief Constructs the path from a character sequence (format interpreted as specified by fmt) provided by
    /// source, which is a pointer or an input iterator to a null-terminated character/wide character sequence
    ///
    /// \details In difference to std::filesystem::path, we only allow char based strings (only ASCI)
    // NOLINTNEXTLINE(google-explicit-constructor): We allow implicit conversion to follow std::filesystem::path
    Path(string_type&& user_path, const Format format = Format::kAutoFormat) noexcept;

    /// \brief Constructs the path from a character sequence (format interpreted as specified by fmt) provided by
    /// source, which is a pointer or an input iterator to a null-terminated character/wide character sequence
    ///
    /// \details Only participates in resolution overloading, if underlying type is a char
    template <class Source, typename = std::enable_if_t<detail::CanBeInterpretedAsPath<Source>::value>>
    // clang-format off
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay, google-explicit-constructor): Required to mimic std::filesystem::path from the C++ Standard
    Path(const Source& source, const Format format = Format::kAutoFormat) noexcept : Path{string_type{source}, true}
    // clang-format on
    {
        static_cast<void>(format);
    }

    /// \brief Destroys the Path object
    ~Path() noexcept;

    /// \brief Replaces the contents of *this with a pathname whose both native and generic format representations equal
    /// those of p.
    Path& operator=(const Path&) noexcept;

    /// \brief Replaces the contents of *this with a pathname whose both native and generic format representations equal
    /// those of p, possibly using move semantics: p is left in a valid, but unspecified state.
    Path& operator=(Path&&) noexcept;

    // concatenation

    Path& operator/=(const Path& to_append) noexcept;

    template <class Source, typename = std::enable_if_t<detail::CanBeInterpretedAsPath<Source>::value>>
    Path& operator/=(const Source& to_append) noexcept
    {
        // coverity[autosar_cpp14_m6_2_1_violation] intentional use in return statement
        return operator/=(Path{to_append});
    }

    template <class Source, typename = std::enable_if_t<detail::CanBeInterpretedAsPath<Source>::value>>
    Path& Append(const Source& to_append) noexcept
    {
        // coverity[autosar_cpp14_m6_2_1_violation] intentional use in return statement
        return operator/=(Path{to_append});
    }

    // modifiers

    // @todo: Add clear, remove_filename, replace_filename, replace_extension, swap

    // format observers

    /// \brief Accesses the native path name as a character string.
    const value_type* CStr() const noexcept;

    /// \brief Accesses the Native path name as a character string.
    const string_type& Native() const noexcept;

    /// \brief Accesses the native path name as a character string.
    ///
    /// \details Note: implicit conversion requested as by std::filesystem::path interface
    // NOLINTNEXTLINE(google-explicit-constructor): Required to mimic std::filesystem::path from the C++ Standard
    operator string_type() const noexcept;

    // generation

    /// \brief Returns *this converted to normal form in its generic format
    ///
    /// \details For example a path /../../foo/./bar/hello/.. gets converted to /foo/bar
    Path LexicallyNormal() const noexcept;

    // decomposition

    /// \brief Returns the root name of the generic-format path. If the path (in generic format) does not include root
    /// name, returns path().
    Path RootName() const noexcept;

    /// \brief Returns the root directory of the generic-format path. If the path (in generic format) does not include
    /// root directory, returns path().
    Path RootDirectory() const noexcept;

    /// \brief Returns the root path of the path. If the path does not include root path, returns path().
    /// Effectively returns root_name() / root_directory().
    /// \returns The root path of the path.
    Path RootPath() const noexcept;

    /// \brief Returns path relative to root-path, that is, a pathname composed of every generic-format component of
    /// *this after root-path. If *this is an empty path, returns an empty path.
    Path RelativePath() const noexcept;

    /// \brief Returns the path to the parent directory.
    ///
    /// \details If has_relative_path() returns false, the result is a copy of *this. Otherwise, the result is a path
    /// whose generic format pathname is the longest prefix of the generic format pathname of *this that produces one
    /// fewer element in its iteration.
    Path ParentPath() const noexcept;

    /// \brief Returns the generic-format filename component of the path.
    /// Equivalent to relative_path().empty() ? path() : *--end()
    Path Filename() const noexcept;

    /// \brief Returns the extension of the filename component of the generic-format view of *this.
    ///
    /// \details If the filename() component of the generic-format path contains a period (.), and is not one of the
    /// special filesystem elements dot or dot-dot, then the extension is the substring beginning at the rightmost
    /// period (including the period) and until the end of the pathname.
    //
    // If the first character in the filename is a period, that period is ignored (a filename like ".profile" is not
    // treated as an extension)
    //
    // If the pathname is either . or .., or if filename() does not contain the '.' character, then empty path is
    // returned.
    //
    // Additional behavior may be defined by the implementations for file systems which append additional elements (such
    // as alternate data streams or partitioned dataset names) to extensions.
    ///
    /// For example
    /// foo/bar/test.txt will return .txt
    /// foo/bar/.ssh will return .ssh
    /// foo/bar will return empty
    /// foo/bar.what.ever will return .ever
    /// foo/bar/.. will return empty
    Path Extension() const noexcept;

    /// \brief Returns the filename without extension.
    ///
    /// \details
    /// Returns substring of filename, the characters since last period (.) character are not included.
    /// Exceptions:
    /// 1. If filename starts with period (.) characted then it will be ignored (.bar.foo => .foo).
    /// 2. For special filenames (dot and dot-dot), the full filename is returned.
    ///
    /// For example
    /// foo/bar/filename.ext => filename
    /// foo/bar/filename.ext.ext2 => filename.ext
    /// foo/bar/.zzz => .zzz
    /// foo/bar/..zzz => .
    /// foo/bar/. => .
    /// foo/bar/.. => ..
    Path Stem() const noexcept;

    /// \brief Replaces the extension.
    /// \param replacement new extension (the default value is empty)
    /// \return *this
    Path& ReplaceExtension(const Path& replacement = Path{}) noexcept;

    /// \brief Removes a filename (as returned by Path::Filename()).
    /// \return *this
    Path& RemoveFilename() noexcept;

    // queries

    /// \brief Checks if the path in generic format is empty.
    bool Empty() const noexcept;

    /// \brief Checks whether RootPath() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasRootPath() const noexcept;

    /// \brief Checks whether RootName() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasRootName() const noexcept;

    /// \brief Checks whether RootDirectory() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasRootDirectory() const noexcept;

    /// \brief Checks whether RelativePath() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasRelativePath() const noexcept;

    /// \brief Checks whether ParentPath() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasParentPath() const noexcept;

    /// \brief Checks whether Filename() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasFilename() const noexcept;

    /// \brief Checks whether Extension() is empty.
    /// \returns true if the corresponding path component is not empty, false otherwise.
    bool HasExtension() const noexcept;

    /// \brief Checks whether the path is absolute or relative. An absolute path is a path that unambiguously identifies
    /// the location of a file without reference to an additional starting location.
    ///
    /// \return true if the path is absolute, false otherwise.
    bool IsAbsolute() const noexcept;

    /// \brief Checks whether the path is absolute or relative. An absolute path is a path that unambiguously identifies
    /// the location of a file without reference to an additional starting location.
    ///
    /// \return false if the path is absolute, true otherwise.
    bool IsRelative() const noexcept;

    // non-member functions

    friend bool operator==(const Path& lhs, const Path& rhs) noexcept;
    friend bool operator!=(const Path& lhs, const Path& rhs) noexcept;

    /// \brief Iterator for the parts of the path separated by the preferred path separator.
    ///
    /// \details Implements the iterator returned by Path::begin(), Path::end() methods.
    /// We try to implement the same behavior as described in https://en.cppreference.com/w/cpp/filesystem/path/begin
    /// Notes:
    /// 1. Empty path has zero parts.
    /// 2. The root path is a separate part of the path.
    /// 3. The filename and extension are contained in the same part of the path.
    class iterator final
    {
      public:
        using value_type = Path;
        using difference_type = std::ptrdiff_t;
        using pointer = const Path*;
        using reference = const Path&;
        using iterator_category = std::bidirectional_iterator_tag;

        /// \brief Constructs empty iterator.
        iterator() noexcept = default;

        /// \brief Copy constructor.
        iterator(const iterator&) noexcept;

        /// \brief Move constructor.
        iterator(iterator&&) noexcept = default;

        /// \brief copy assignment operator.
        iterator& operator=(const iterator&) noexcept;

        /// \brief move assignment operator.
        iterator& operator=(iterator&&) & noexcept;

        /// \brief Destructor.
        ~iterator() noexcept = default;

        /// @brief Accesses the pointed-to Path
        reference operator*() const noexcept;

        /// @brief Accesses the pointed-to Path
        pointer operator->() const noexcept;

        // Pre increment/decrement
        iterator& operator++() noexcept;
        iterator& operator--() noexcept;

        // Post increment/decrement
        iterator operator--(int) noexcept;
        iterator operator++(int) noexcept;

        friend bool operator==(const iterator& l, const iterator& r) noexcept;
        friend bool operator!=(const iterator& l, const iterator& r) noexcept;

      private:  // private methods
        // Paths end() and begin() utilize Iterators private constructors
        // coverity[autosar_cpp14_a11_3_1_violation]
        friend class Path;

        /// \brief Creates an iterator for a path that has multiple parts
        iterator(const Path& path, const std::vector<Path>::const_iterator cur) noexcept;

        /// \brief Creates an iterator for an empty path or a one-part path.
        iterator(const Path& path, const bool is_at_end) noexcept;

        /// \brief Compares iterators. Returns true if the iterators are equal.
        bool equals(const iterator& r) const noexcept;

      private:  // private fields
        std::optional<std::reference_wrapper<const Path>> path_;
        std::vector<Path>::const_iterator cur_;
        bool is_at_end_{false};  // only used when path::parts_ is empty
    };

    /// \brief  Returns an iterator to the first element of the path parts.
    iterator begin() const noexcept;
    /// \brief Returns an iterator one past the last element of the path parts.
    iterator end() const noexcept;

  private:  // private members
    Path(string_type&& path, const bool do_parsing) noexcept;

    /// \brief Splits the path (native_path_) using the preferred separator and fills the parts-array.
    void Parse(const string_type& path) noexcept;

    /// \brief Returns position of filename.
    /// \note Returns string_type::npos if corresponding position not found and filename is empty.
    std::size_t FilenamePosition() const noexcept;

    /// \brief Returns position of extension separator.
    /// \param position_of_filename Should be result of FilenamePosition().
    /// \note Returns string_type::npos if corresponding position not found and filename is empty.
    std::size_t ExtensionPosition(const std::size_t position_of_filename) const noexcept;

    /// \brief Returns position of extension separator.
    /// \note Returns string_type::npos if corresponding position not found and extension is empty.
    std::size_t ExtensionPosition() const noexcept;

    void AddPreferredSeperatorIfNeeded(std::list<std::string>& new_parts, std::string& normalized_path) const noexcept;

    void AddPathPartToParts(std::stringstream& stringstream_path, std::string& path_part) noexcept;

    /// \brief Stream output operator.
    template <typename OutputStream>
    // coverity[autosar_cpp14_a11_3_1_violation] defining as a non-member causes ambiguity in operator resolution
    // coverity[autosar_cpp14_a13_2_2_violation] false-positive it's neither binary nor arithmetic op
    friend OutputStream& operator<<(OutputStream& out, const Path& path)
    {
        return out << path.Native();
    }

  private:  // fields
    std::string native_path_;
    std::vector<Path> parts_;
};

bool operator==(const Path& lhs, const Path& rhs) noexcept;
bool operator!=(const Path& lhs, const Path& rhs) noexcept;
Path operator/(const Path& lhs, const Path& rhs) noexcept;
bool operator<(const Path& lhs, const Path& rhs) noexcept;

bool operator==(const Path::iterator& l, const Path::iterator& r) noexcept;
bool operator!=(const Path::iterator& l, const Path::iterator& r) noexcept;

}  // namespace filesystem
}  // namespace score

#endif  // SCORE_LIB_FILESYSTEM_PATH_H
