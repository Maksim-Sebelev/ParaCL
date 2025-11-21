#pragma once

//----------------------------------------------------------------------------------------------

#include <cassert>
#include <cstdarg>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <source_location>
#include <string>

#include "custom_console_output.hpp"
#include "global.hpp"
#include "log_background_settings.hpp"

//----------------------------------------------------------------------------------------------

#if defined(SAVE_HTML_STYLE)
#define ON_TAB(...) __VA_ARGS__
#else /* defined(SAVE_HTML_STYLE) */
#define ON_TAB(...)
#endif /* defined(SAVE_HTML_STYLE) */

//----------------------------------------------------------------------------------------------

namespace PineapLog
{

//----------------------------------------------------------------------------------------------

namespace Detail
{

//----------------------------------------------------------------------------------------------

#if defined(LOG_DIR)
inline const std::string path_to_log_file = LOG_DIR;
#else /* defined(LOG_DIR) */
#warning "You dont give log-out dir to logger. Log-out directory will be made in one level with yout current directory."
inline const std::string path_to_log_file = "../log/";
#endif /* defined(LOG_DIR) */
inline const std::string default_log_file_name = "deflog";
inline const std::string html_extension = ".html";
inline const std::string full_path_to_default_log = path_to_log_file + default_log_file_name + html_extension;

inline const std::string global_logger_name = "global";

//----------------------------------------------------------------------------------------------

} /* namespace Detail */

//----------------------------------------------------------------------------------------------

enum class LoggerBackground
{
    plain,
    image,
    gradient,
};

//----------------------------------------------------------------------------------------------

enum class LogColor : char
{
    White,
    Red,
    Green,
    Pink,
    Yellow,
    Black,
    Blue,
};

//----------------------------------------------------------------------------------------------

class Logger
{
  public:
    // default ctor. background type = plain
    inline Logger(const std::string &logger_file_name, bool need_log_call_place = false,
                  const std::source_location &location = std::source_location::current());
    inline Logger(
        bool need_log_call_place = false,
        const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // ctor with choice of background type. BUT here you cant to choose image on background
    inline Logger(const std::string &logger_file_name, const LoggerBackground &background,
                  bool need_log_call_place = false,
                  const std::source_location &location = std::source_location::current());
    inline Logger(
        const LoggerBackground &background, bool need_log_call_place = false,
        const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // ctor for background with image
    inline Logger(const std::string &logger_file_name, std::string_view path_to_image, bool need_log_call_place = false,
                  const std::source_location &location = std::source_location::current());
    inline Logger(
        const LoggerBackground &background, std::string_view path_to_image, bool need_log_call_place = false,
        const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // dtor
    inline ~Logger();

    // rule of 5 (redefine default dtor)
    // not allowed to copy logger
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    // not allowed to move logger
    Logger(const Logger &&) = delete;
    Logger &operator=(const Logger &&) = delete;

    // logger public methods
    template <typename... Args> inline void log(const Args &...args);
    template <typename... Args> inline void logc(const LogColor &color, const Args &...args);

    inline void logc_in_line_begin(const LogColor &color = LogColor::White);
    inline void log_in_line_begin();
    template <typename... Args> inline void log_in_line(const Args &...args);
    inline void log_in_line_end();

    inline void title(const std::string &title, LogColor color = LogColor::White);

    inline void code_place(const std::source_location &location = std::source_location::current());

    inline void set_color(const LogColor &color);

    inline void log_endl();

    inline void log_image(std::string_view path_to_image, std::string_view description = "");

  private:
    // logger private variables
    std::ofstream log_file_;
    LogColor current_color_;

    // logger private methods
    template <typename... Args> inline void write_in_html(const Args &...args);

    inline void all_ctors_actions([[maybe_unused]] std::string_view name, bool need_log_call_place,
                                  const std::source_location &location);

    inline bool try_to_create_log_dir_and_open_it_one_more_time(const std::string &log_file_name);

    std::string get_color_in_str_for_html(const LogColor &color);

    inline void date(const LogColor &color = LogColor::White);

    inline void log_call_place_if_need(bool need_log_call_place, const std::source_location location,
                                       std::string_view message = "");
    inline void check_that_open_success(const std::string &log_file_name, bool already_with_path = false);

    [[noreturn]]
    inline void failed_open_log_file(const std::string &file);
};

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// create global log file

/* FIXME: remove define*/
#define GLOBAL_LOGGER_IMAGE std::string_view("/home/ananasik/Pictures/mmm/anna/photo_5_2025-10-21_11-55-45.jpg")
// glog = global logger
#if defined(GLOBAL_LOGGER_DEFAULT)
inline Logger glog(Detail::global_logger_name);
#elif defined(GLOBAL_LOGGER_IMAGE)
inline Logger glog(Detail::global_logger_name, GLOBAL_LOGGER_IMAGE);
#elif defined(GLOBAL_LOGGER_GRADIENT)
inline Logger glog(Detail::global_logger_name, LoggerBackground::gradient);
#else
inline Logger glog(Detail::global_logger_name);
#endif

// ctors
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

inline Logger::Logger(const std::string &log_file_name, bool need_log_call_place, const std::source_location &location)
    : log_file_(Detail::path_to_log_file + log_file_name + Detail::html_extension), current_color_(LogColor::White)
{
    check_that_open_success(log_file_name);

    write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);

    all_ctors_actions(Detail::path_to_log_file + log_file_name + Detail::html_extension, need_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

inline Logger::Logger(bool need_log_call_place, const std::source_location &location)
    : log_file_(Detail::full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(Detail::full_path_to_default_log, true);

    write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);

    all_ctors_actions(Detail::full_path_to_default_log, need_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

inline Logger::Logger(const std::string &log_file_name, const LoggerBackground &background, bool need_log_call_place,
                      const std::source_location &location)
    : log_file_(Detail::path_to_log_file + log_file_name + Detail::html_extension), current_color_(LogColor::White)
{
    check_that_open_success(log_file_name);

    switch (background)
    {
    case LoggerBackground::plain:
        write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);
        break;

    case LoggerBackground::gradient:
        write_in_html(GRADIENT_BACKGROUND_HTML_SETTINGS);
        break;

    case LoggerBackground::image:
        builtin_unreachable_wrapper("we cant parse it without path to image. this situation in another ctor");
        break;

    default:
        builtin_unreachable_wrapper(
            "undefined 'LoggerBackground' value. maybe you add new type and forgot to add parsing of this new value");
        break;
    }

    all_ctors_actions(Detail::path_to_log_file + log_file_name + Detail::html_extension, need_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

inline Logger::Logger(const LoggerBackground &background, bool need_log_call_place,
                      const std::source_location &location)
    : log_file_(Detail::full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(Detail::full_path_to_default_log, true);

    switch (background)
    {
    case LoggerBackground::plain:
        write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);
        break;

    case LoggerBackground::gradient:
        write_in_html(GRADIENT_BACKGROUND_HTML_SETTINGS);
        break;

    case LoggerBackground::image:
        builtin_unreachable_wrapper("we cant parse it without path to image. this situation in another ctor");
        break;

    default:
        builtin_unreachable_wrapper(
            "undefined 'LoggerBackground' value. maybe you add new type and forgot to add parsing of this new value");
        break;
    }

    all_ctors_actions(Detail::full_path_to_default_log, need_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

inline Logger::Logger(const std::string &log_file_name, std::string_view path_to_image, bool need_log_call_place,
                      const std::source_location &location)
    : log_file_(Detail::path_to_log_file + log_file_name + Detail::html_extension), current_color_(LogColor::White)
{
    check_that_open_success(log_file_name);

    write_in_html(IMAGE_BACKGROUND_HTML_SETTINGS(path_to_image));

    all_ctors_actions(Detail::path_to_log_file + log_file_name + Detail::html_extension, need_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

inline Logger::Logger(const LoggerBackground &background, std::string_view path_to_image, bool need_log_call_place,
                      const std::source_location &location)
    : log_file_(Detail::full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(Detail::full_path_to_default_log, true);

    switch (background)
    {
    case LoggerBackground::image:
        break;

    case LoggerBackground::gradient:
    case LoggerBackground::plain:
    default:
        builtin_unreachable_wrapper("here can be only image type");
    }

    write_in_html(IMAGE_BACKGROUND_HTML_SETTINGS(path_to_image));

    all_ctors_actions(Detail::full_path_to_default_log, need_log_call_place, location);
}

// dtor
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

inline Logger::~Logger()
{
    date();
    write_in_html(HTML_END);
    log_file_.close();
}

// PUBLIC methods
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

template <typename... Args> inline void Logger::logc(const LogColor &color, const Args &...args)
{
    set_color(color);
    write_in_html(ON_TAB("\t\t\t\t") "<p>", args..., "</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

template <typename... Args> inline void Logger::log(const Args &...args)
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>", args..., "</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

template <typename... Args> inline void Logger::log_in_line(const Args &...args)
{
    write_in_html(args...);
}

//----------------------------------------------------------------------------------------------

inline void Logger::log_endl()
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>\n</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

inline void Logger::logc_in_line_begin(const LogColor &color)
{
    set_color(color);
    write_in_html(ON_TAB("\t\t\t\t") "<p>");
}

//----------------------------------------------------------------------------------------------

inline void Logger::log_in_line_begin()
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>");
}

//----------------------------------------------------------------------------------------------

inline void Logger::log_in_line_end()
{
    write_in_html("</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

inline void Logger::title(const std::string &title, LogColor color)
{
    static constexpr size_t ScreenSize = 45; // count in char's, that size is like h2
    size_t title_len = title.size() + 2;

    size_t free_place_len = 0;
    if (title_len < ScreenSize - 2)
        free_place_len = (ScreenSize - (title_len + 2)) / 2;
    if (free_place_len > 0 && (ScreenSize - title_len) % 2 == 0)
        --free_place_len;

    set_color(color);
    write_in_html(ON_TAB("\t\t\t\t") "<h2>");

    for (size_t i = 0; i < free_place_len; i++)
        write_in_html("=");

    write_in_html(" ", title, " ");

    for (size_t i = 0; i < free_place_len; i++)
        write_in_html("=");

    write_in_html("</h2>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

inline void Logger::set_color(const LogColor &color)
{
    if (color == current_color_) // жертуем одним сравнением, чтобы не делать дорогой write_in_html
        return;

    current_color_ = color;

    std::string color_html = get_color_in_str_for_html(color);

    write_in_html(ON_TAB("\t\t\t") "</span>" ON_TAB("\n") // end old color
                  ON_TAB("\t\t\t") "<span class=\"color ",
                  color_html, "\">" ON_TAB("\n"));

    return;
}

//----------------------------------------------------------------------------------------------

inline void Logger::code_place(const std::source_location &code_place)
{
    logc(LogColor::Pink, code_place.file_name(), ":", code_place.line(), ":", code_place.column());

    logc(LogColor::Green, code_place.function_name());
}

//----------------------------------------------------------------------------------------------

inline void Logger::log_image(std::string_view path_to_image, std::string_view description)
{
    write_in_html(ON_TAB("\t\t\t\t") "<img src=\"", path_to_image, "\" alt=\"", description, "\">" ON_TAB("\n"));
}

// private methods
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

template <typename... Args> inline void Logger::write_in_html(const Args &...args)
{
    ((log_file_ << args), ...) << std::flush;
}

//----------------------------------------------------------------------------------------------

inline void Logger::all_ctors_actions([[maybe_unused]] std::string_view name, bool need_log_call_place,
                                      const std::source_location &location)
{
    // std::cerr << WHITE "path to log:\n" VIOLET BOLD << name << RESET_CONSOLE_OUT << std::endl;

    write_in_html(ON_TAB("\t\t\t") "<span class=\"color white\">" ON_TAB("\n"));
    date();
    log_call_place_if_need(need_log_call_place, location);
    log_endl();
}

//----------------------------------------------------------------------------------------------

inline void Logger::date(const LogColor &color)
{
    time_t raw_time;
    struct tm *time_info;
    static const size_t date_len = 64;
    char date[date_len] = {};

    time(&raw_time);
    time_info = localtime(&raw_time);
    strftime(date, date_len, "%H:%M:%S %Y-%m-%d", time_info);

    title(date, color);
}

//----------------------------------------------------------------------------------------------

inline void Logger::check_that_open_success(const std::string &log_file_name, bool already_with_path)
{
    if (!log_file_.fail())
        return;

    const std::string full_path_to_log_file =
        (already_with_path) ? (log_file_name) : (Detail::path_to_log_file + log_file_name + Detail::html_extension);

    if (try_to_create_log_dir_and_open_it_one_more_time(full_path_to_log_file))
        return;

    failed_open_log_file(full_path_to_log_file); // exit 1
}

//----------------------------------------------------------------------------------------------

inline bool Logger::try_to_create_log_dir_and_open_it_one_more_time(const std::string &log_file_name)
{
    std::filesystem::path path(log_file_name);
    std::string dir_path = path.parent_path().string();

#if defined(_WIN32) || defined(_WIN64)
    std::string command = "mkdir \"" + dir_path + "\" 2>nul";
#else
    std::string command = "mkdir -p \"" + dir_path + "\" 2>/dev/null";
#endif

    int result = system(command.c_str());

    if (result == EXIT_FAILURE)
        return false;

    log_file_.open(log_file_name);
    return !(log_file_.fail());
}

//----------------------------------------------------------------------------------------------

inline void Logger::log_call_place_if_need(bool need_log_call_place, const std::source_location location,
                                           std::string_view message)
{
    if (not need_log_call_place)
        return;

    if (message != "")
        log(message);

    code_place(location);
}

//----------------------------------------------------------------------------------------------

std::string inline Logger::get_color_in_str_for_html(const LogColor &color)
{
    switch (color)
    {
    case LogColor::Red:
        return "red";
    case LogColor::Green:
        return "green";
    case LogColor::Pink:
        return "pink";
    case LogColor::Yellow:
        return "yellow";
    case LogColor::Black:
        return "black";
    case LogColor::Blue:
        return "blue";
    case LogColor::White:
        return "white";
    default:
        builtin_unreachable_wrapper("undef color type. maybe you forgot to add something color");
    }

    builtin_unreachable_wrapper("we must return in switch");
    return "";
}

//----------------------------------------------------------------------------------------------

[[noreturn]]
inline void Logger::failed_open_log_file(const std::string &file)
{
    std::cerr << RED BOLD "Failed open log file: `" ITALIC << file << "'" RESET_CONSOLE_OUT << std::endl;
    exit(EXIT_FAILURE);
}

//----------------------------------------------------------------------------------------------

} /* namespace PineapLog */

//----------------------------------------------------------------------------------------------

// undef all logger detail-macroses
#undef ON_TAB
#undef LOG_DIR
#undef endl1_str
#undef endl2_str
#undef tab1_str
#undef tab2_str
#undef tab3_str
#undef tab4_str
#undef tab5_str
#undef GLOBAL_HTML_SETTINGS_1
#undef GLOBAL_HTML_SETTINGS_2
#undef PLAIN_BACKGROUND_HTML_SETTINGS
#undef GRADIENT_BACKGROUND_HTML_SETTINGS
#undef IMAGE_BACKGROUND_HTML_SETTINGS
#undef HTML_END
#undef builtin_unreachable_wrapper
#undef msg_assert
#undef ON_DEBUG
#undef OFF_DEBUG

//----------------------------------------------------------------------------------------------
