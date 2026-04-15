module;

#include <string>
#include <string_view>

export module node_code_location;

//--------------------------------------------------------------------------------------------------------------------------------------
namespace ParaCL::ast::node
{
//--------------------------------------------------------------------------------------------------------------------------------------

export
class CodeLocation final
{
public:
    using code_place_uint_t = unsigned int;
private:
    std::string file_;

    code_place_uint_t line_begin_   = 0;
    code_place_uint_t line_end_     = 0;
    code_place_uint_t column_begin_ = 0;
    code_place_uint_t column_end_   = 0;

    std::string       code_excerpt_ = "";

public: /* getters */
    std::string_view file() const noexcept
    { return file_; }

    code_place_uint_t column_begin() const noexcept
    { return column_begin_; }
    code_place_uint_t column_end() const noexcept
    { return column_end_; }
    code_place_uint_t line_begin() const noexcept
    { return line_begin_; }
    code_place_uint_t line_end() const noexcept
    { return line_end_; }

    std::string_view code_excerpt() const & noexcept
    { return code_excerpt_;  }

    std::string&& code_excerpt() && noexcept
    { return std::move(code_excerpt_); }

public: /* setters */
    void set_file(std::string_view file)
    { file_ = file; }

    void set_line_begin(code_place_uint_t line_begin) noexcept
    { line_begin_ = line_begin; }

    void set_line_end(code_place_uint_t line_end) noexcept
    { line_end_ = line_end; }

    void set_column_begin(code_place_uint_t column_begin) noexcept
    { column_begin_ = column_begin; }

    void set_column_end(code_place_uint_t column_end) noexcept
    { column_end_ = column_end; }

    void set_code_excerpt(std::string_view code_excerpt) 
    { code_excerpt_ = code_excerpt; }

    void set_position(code_place_uint_t line_begin, code_place_uint_t line_end,
                                code_place_uint_t column_begin, code_place_uint_t column_end) noexcept
    {
        line_begin_ = line_begin;
        line_end_ = line_end;
        column_begin_ = column_begin;
        column_end_ = column_end;
    }

    void set_all(std::string_view file, code_place_uint_t line_begin, code_place_uint_t line_end,
                           code_place_uint_t column_begin, code_place_uint_t column_end,
                           std::string_view code_excerpt)
    {
        file_ = file;
        line_begin_ = line_begin;
        line_end_ = line_end;
        column_begin_ = column_begin;
        column_end_ = column_end;
        code_excerpt_ = code_excerpt;
    }

public:
    CodeLocation()
    {}

    CodeLocation(std::string_view file, code_place_uint_t lb, code_place_uint_t le, code_place_uint_t cb, code_place_uint_t ce, std::string_view code_excerpt = "") :
        file_(file),
        line_begin_(lb), line_end_(le), column_begin_(cb), column_end_(ce),
        code_excerpt_(code_excerpt)
    {}
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace ParaCL::ast::node */
//--------------------------------------------------------------------------------------------------------------------------------------
