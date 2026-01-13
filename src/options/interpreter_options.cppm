module;

#include <concepts>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

export module interpreter_options;

export import options;

namespace ParaCL
{
namespace options
{
namespace interpreter
{

using file = std::filesystem::path;

export template <typename InterpreterOptions>
concept IInterpreterOptions = requires(const InterpreterOptions &io, size_t i) {
    { InterpreterOptions::queryInterpreterOptions() } -> std::convertible_to<const InterpreterOptions &>;

    { io.i_source(i) } -> std::convertible_to<const file &>;
    { io.i_source_fast_unsafe(i) } noexcept -> std::convertible_to<const file &>;
    // { std::move(io).i_source(i) } -> std::convertible_to<file>; /* dont need in singleton */
    // { std::move(io).i_source_fast_unsafe(i) } noexcept -> std::convertible_to<file>; /* dont need in singleton */

    { io.sources() } -> std::convertible_to<const std::vector<file> &>;
    // { std::move(io).sources() } -> std::convertible_to<std::vector<file>>; /* dont need in singleton */

    { io.input_files_quant() } -> std::convertible_to<size_t>;
};

export template <options::IOptions Options = options::Options> class InterpreterOptions
{
  public:
    static const InterpreterOptions &queryInterpreterOptions()
    {
        static InterpreterOptions opts{Options::queryOptions()};
        return opts;
    }

    InterpreterOptions(const InterpreterOptions &) = delete;
    InterpreterOptions(InterpreterOptions &&) = delete;
    InterpreterOptions &operator=(const InterpreterOptions &) = delete;
    InterpreterOptions &operator=(InterpreterOptions &&) = delete;

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

    const std::vector<file> &sources() const &
    {
        return sources_;
    }
    // std::vector<file> sources() const && { return sources_; } /* dont need in singleton */

    size_t input_files_quant() const
    {
        return sources_.size();
    }

  private:
    explicit InterpreterOptions(const Options &opt) : sources_(opt.sources())
    {
    }

    std::vector<file> sources_;
};

static_assert(IInterpreterOptions<InterpreterOptions<>>, "InterpreterOptions must satisfy IInterpreterOptions concept");

} /* namespace interpreter */
} /* namespace options */
} /* namespace ParaCL */
