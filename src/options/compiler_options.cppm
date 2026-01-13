module;

#include <concepts>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "global/global.hpp"

export module compiler_options;

export import options;

namespace ParaCL
{
namespace options
{
namespace compiler
{
using file = std::filesystem::path;

export template <typename CompilerOptions>
concept ICompilerOptions = requires(const CompilerOptions &co, size_t i) {
    { CompilerOptions::queryCompilerOptions() } -> std::convertible_to<const CompilerOptions &>;

    { co.i_source(i) } -> std::convertible_to<const file &>;
    { co.i_source_fast_unsafe(i) } noexcept -> std::convertible_to<const file &>;
    // { std::move(co).i_source(i) } -> std::convertible_to<file>; /* dont need in singleton */
    // { std::move(co).i_source_fast_unsafe(i) } noexcept -> std::convertible_to<file>; /* dont need in singleton */

    { co.i_object(i) } -> std::convertible_to<const file &>;
    { co.i_object_fast_unsafe(i) } noexcept -> std::convertible_to<const file &>;
    // { std::move(co).i_object(i) } -> std::convertible_to<file>; /* dont need in singleton */
    // { std::move(co).i_object_fast_unsafe(i) } noexcept -> std::convertible_to<file>; /* dont need in singleton */

    { co.sources() } -> std::convertible_to<const std::vector<file> &>;
    { co.objects() } -> std::convertible_to<const std::vector<file> &>;
    // { std::move(co).sources() } -> std::convertible_to<std::vector<file>>; /* dont need in singleton */
    // { std::move(co).objects() } -> std::convertible_to<std::vector<file>>; /* dont need in singleton */

    { co.executable() } -> std::convertible_to<const file &>;
    // { std::move(co).executable() } -> std::convertible_to<file>; /* dont need in singleton */

    { co.optimize_level() } -> std::same_as<options::optimize_level_t>;
    { co.input_files_quant() } -> std::convertible_to<size_t>;
    { co.compile() } -> std::convertible_to<bool>;

    ON_GRAPHVIZ(
        { co.ast_dump() } -> std::convertible_to<bool>; { co.dot_file() } -> std::convertible_to<const file &>;
        // { std::move(co).dot_file() } -> std::convertible_to<file>;  /* dont need in singleton */
    )
};

export template <options::IOptions Options = options::Options> class CompilerOptions
{
  private:
    explicit CompilerOptions(const Options &opt)
        : sources_(opt.sources()), objects_(opt.objects()), executable_file_(opt.executable()),
          optimize_level_(opt.optimize_level()),
          compile_(opt.compile()) ON_GRAPHVIZ(, ast_dump(opt.ast_dump_), dot_file_(opt.dot_file_))
    {
    }

  public:
    static const CompilerOptions &queryCompilerOptions()
    {
        static CompilerOptions opts{Options::queryOptions()};
        return opts;
    }

    CompilerOptions(const CompilerOptions &) = delete;
    CompilerOptions(CompilerOptions &&) = delete;
    CompilerOptions &operator=(const CompilerOptions &) = delete;
    CompilerOptions &operator=(CompilerOptions &&) = delete;

    const file &i_source(size_t i) const &
    {
        if (i >= sources_.size())
            throw std::runtime_error("try to get no existence source");
        return sources_[i];
    }
    const file &i_source_fast_unsafe(size_t i) const & noexcept
    {
        return sources_[i];
    }
    // file i_source(size_t i) const && { if (i >= sources_.size()) throw std::runtime_error("try to get no existence
    // source"); return sources_[i]; } /* dont need in singleton */ file i_source_fast_unsafe(size_t i) const &&
    // noexcept { return sources_[i]; } /* dont need in singleton */

    const file &i_object(size_t i) const &
    {
        if (i >= objects_.size())
            throw std::runtime_error("try to get no existence object");
        return objects_[i];
    }
    const file &i_object_fast_unsafe(size_t i) const & noexcept
    {
        return objects_[i];
    }
    // file i_object(size_t i) const && { if (i >= objects_.size()) throw std::runtime_error("try to get no existence
    // object"); return objects_[i]; } /* dont need in singleton */ file i_object_fast_unsafe(size_t i) const &&
    // noexcept { return objects_[i]; } /* dont need in singleton */

    const std::vector<file> &sources() const &
    {
        return sources_;
    }
    const std::vector<file> &objects() const &
    {
        return objects_;
    }
    // std::vector<file> sources() const && { return sources_; } /* dont need in singleton */
    // std::vector<file> objects() const && { return objects_; } /* dont need in singleton */

    const file &executable() const &
    {
        return executable_file_;
    }
    // file executable() const && { return executable_file_; } /* dont need in singleton */

    options::optimize_level_t optimize_level() const
    {
        return optimize_level_;
    }

    size_t input_files_quant() const
    {
        return sources_.size();
    }

    bool compile() const
    {
        return compile_;
    }

    ON_GRAPHVIZ(
        bool ast_dump() const { return ast_dump_; } const file &dot_file() const & { return dot_file_; }
        // file dot_file() const && { return dot_file_; }  /* dont need in singleton */
    )

  private:
    std::vector<file> sources_;
    std::vector<file> objects_;
    file executable_file_;
    options::optimize_level_t optimize_level_;
    bool compile_ : 1;

    ON_GRAPHVIZ(bool ast_dump_ : 1; file dot_file_;)
};

static_assert(ICompilerOptions<CompilerOptions<>>, "CompilerOptions must satisfy ICompilerOptions concept");

} /* namespace compiler*/
} /* namespace options */
} /* namespace ParaCL */
