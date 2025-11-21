module;

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

export module pineaplog;

//----------------------------------------------------------------------------------------------

#if not defined(LOG_DIR)
#warning "You dont give log-out dir to logger. Log-out directory will be made in one level with yout current directory."
#endif /* not defined(LOG_DIR) */

#if defined(LOG_DIR)
const std::string path_to_log_file = LOG_DIR;
#else  /* defined(LOG_DIR) */
const std::string path_to_log_file = "../log/"; // from 'build' directory
#endif /* defined(LOG_DIR) */
const std::string default_log_file_name = "deflog";
const std::string html_extension = ".html";
const std::string full_path_to_default_log =
    path_to_log_file + default_log_file_name + html_extension; // from 'build' directory

const std::string global_logger_name = "global";

//----------------------------------------------------------------------------------------------

export namespace PineapLog
{

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

enum class LogCallPlace : bool
{
    No = false,
    Yes = true,
};

//----------------------------------------------------------------------------------------------

class Logger
{
  public:
    // default ctor. background type = plain
    Logger(const std::string &logger_file_name, const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current());
    Logger(const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // ctor with choice of background type. BUT here you cant to choose image on background
    Logger(const std::string &logger_file_name, const LoggerBackground &background,
           const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current());
    Logger(const LoggerBackground &background, const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // ctor for background with image
    Logger(const std::string &logger_file_name, std::string_view path_to_image,
           const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current());
    Logger(const LoggerBackground &background, std::string_view path_to_image,
           const LogCallPlace &need_log_code_place = LogCallPlace::No,
           const std::source_location &location = std::source_location::current()); // same ctor, but with default name

    // dtor
    ~Logger();

    // not allowed to copy logger
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    // not allowed to move logger
    Logger(const Logger &&) = delete;
    Logger &operator=(const Logger &&) = delete;

    // logger public methods
    template <typename... Args> void log(const Args &...args);
    template <typename... Args> void logc(const LogColor &color, const Args &...args);

    void logc_in_line_begin(const LogColor &color = LogColor::White);
    void log_in_line_begin();
    template <typename... Args> void log_in_line(const Args &...args);
    void log_in_line_end();

    void title(const std::string &title, LogColor color = LogColor::White);

    void code_place(const std::source_location &location = std::source_location::current());

    void set_color(const LogColor &color);

    void log_endl();

    void log_image(std::string_view path_to_image, std::string_view description = "");

  private:
    // logger private variables
    std::ofstream log_file_;
    LogColor current_color_;

    // logger private methods
    template <typename... Args> void write_in_html(const Args &...args);

    void all_ctors_actions(std::string_view name, const LogCallPlace &need_to_log_call_place,
                           const std::source_location &location);

    bool try_to_create_log_dir_and_open_it_one_more_time(const std::string &log_file_name);

    std::string get_color_in_str_for_html(const LogColor &color);

    void date(const LogColor &color = LogColor::White);

    void log_call_place_if_need(const LogCallPlace &need_to_log_call_place, const std::source_location location,
                                std::string_view message = "");
    void check_that_open_success(const std::string &log_file_name, bool already_with_path = false);

    [[noreturn]]
    void failed_open_log_file(const std::string &file);
};

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// create global log file

// glog = global logger
#if defined(GLOBAL_LOGGER_DEFAULT)
Logger glog(global_logger_name);
#elif defined(GLOBAL_LOGGER_IMAGE)
Logger glog(global_logger_name, GLOBAL_LOGGER_IMAGE);
#elif defined(GLOBAL_LOGGER_GRADIENT)
Logger glog(global_logger_name, LoggerBackground::gradient);
#else
Logger glog(global_logger_name);
#endif

// ctors
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

Logger::Logger(const std::string &log_file_name, const LogCallPlace &need_to_log_call_place,
               const std::source_location &location)
    : log_file_(path_to_log_file + log_file_name + html_extension), current_color_(LogColor::White)
{
    check_that_open_success(log_file_name);

    write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);

    all_ctors_actions(path_to_log_file + log_file_name + html_extension, need_to_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

Logger::Logger(const LogCallPlace &need_to_log_call_place, const std::source_location &location)
    : log_file_(full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(full_path_to_default_log, true);

    write_in_html(PLAIN_BACKGROUND_HTML_SETTINGS);

    all_ctors_actions(full_path_to_default_log, need_to_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

Logger::Logger(const std::string &log_file_name, const LoggerBackground &background,
               const LogCallPlace &need_to_log_call_place, const std::source_location &location)
    : log_file_(path_to_log_file + log_file_name + html_extension), current_color_(LogColor::White)
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

    all_ctors_actions(path_to_log_file + log_file_name + html_extension, need_to_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

Logger::Logger(const LoggerBackground &background, const LogCallPlace &need_to_log_call_place,
               const std::source_location &location)
    : log_file_(full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(full_path_to_default_log, true);

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

    all_ctors_actions(full_path_to_default_log, need_to_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

Logger::Logger(const std::string &log_file_name, std::string_view path_to_image,
               const LogCallPlace &need_to_log_call_place, const std::source_location &location)
    : log_file_(path_to_log_file + log_file_name + html_extension), current_color_(LogColor::White)
{
    check_that_open_success(log_file_name);

    write_in_html(IMAGE_BACKGROUND_HTML_SETTINGS(path_to_image));

    all_ctors_actions(path_to_log_file + log_file_name + html_extension, need_to_log_call_place, location);
}

//----------------------------------------------------------------------------------------------

Logger::Logger(const LoggerBackground &background, std::string_view path_to_image,
               const LogCallPlace &need_to_log_call_place, const std::source_location &location)
    : log_file_(full_path_to_default_log), current_color_(LogColor::White)
{
    check_that_open_success(full_path_to_default_log, true);

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

    all_ctors_actions(full_path_to_default_log, need_to_log_call_place, location);
}

// dtor
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

Logger::~Logger()
{
    date();
    write_in_html(HTML_END);
    log_file_.close();
}

// PUBLIC methods
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

template <typename... Args> void Logger::logc(const LogColor &color, const Args &...args)
{
    set_color(color);
    write_in_html(ON_TAB("\t\t\t\t") "<p>", args..., "</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

template <typename... Args> void Logger::log(const Args &...args)
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>", args..., "</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

template <typename... Args> void Logger::log_in_line(const Args &...args)
{
    write_in_html(args...);
}

//----------------------------------------------------------------------------------------------

void Logger::log_endl()
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>\n</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

void Logger::logc_in_line_begin(const LogColor &color)
{
    set_color(color);
    write_in_html(ON_TAB("\t\t\t\t") "<p>");
}

//----------------------------------------------------------------------------------------------

void Logger::log_in_line_begin()
{
    write_in_html(ON_TAB("\t\t\t\t") "<p>");
}

//----------------------------------------------------------------------------------------------

void Logger::log_in_line_end()
{
    write_in_html("</p>" ON_TAB("\n"));
}

//----------------------------------------------------------------------------------------------

void Logger::title(const std::string &title, LogColor color)
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

void Logger::set_color(const LogColor &color)
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

void Logger::code_place(const std::source_location &code_place)
{
    logc(LogColor::Pink, code_place.file_name(), ":", code_place.line(), ":", code_place.column());

    logc(LogColor::Green, code_place.function_name());
}

//----------------------------------------------------------------------------------------------

void Logger::log_image(std::string_view path_to_image, std::string_view description)
{
    write_in_html(ON_TAB("\t\t\t\t") "<img src=\"", path_to_image, "\" alt=\"", description, "\">" ON_TAB("\n"));
}

// private methods
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

template <typename... Args> void Logger::write_in_html(const Args &...args)
{
    ((log_file_ << args), ...) << std::flush;
}

//----------------------------------------------------------------------------------------------

void Logger::all_ctors_actions(std::string_view name, const LogCallPlace &need_to_log_call_place,
                               const std::source_location &location)
{
    std::cerr << WHITE "path to log:\n" VIOLET BOLD << name << RESET_CONSOLE_OUT << std::endl;

    write_in_html(ON_TAB("\t\t\t") "<span class=\"color white\">" ON_TAB("\n"));
    date();
    log_call_place_if_need(need_to_log_call_place, location);
    log_endl();
}

//----------------------------------------------------------------------------------------------

void Logger::date(const LogColor &color)
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

void Logger::check_that_open_success(const std::string &log_file_name, bool already_with_path)
{
    if (!log_file_.fail())
        return;

    const std::string full_path_to_log_file =
        (already_with_path) ? (log_file_name) : (path_to_log_file + log_file_name + html_extension);

    if (try_to_create_log_dir_and_open_it_one_more_time(full_path_to_log_file))
        return;

    failed_open_log_file(full_path_to_log_file); // exit 1
}

//----------------------------------------------------------------------------------------------

bool Logger::try_to_create_log_dir_and_open_it_one_more_time(const std::string &log_file_name)
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

void Logger::log_call_place_if_need(const LogCallPlace &need_to_log_call_place, const std::source_location location,
                                    std::string_view message)
{
    if (need_to_log_call_place == LogCallPlace::No)
        return;

    if (message != "")
        log(message);

    code_place(location);
}

//----------------------------------------------------------------------------------------------

std::string Logger::get_color_in_str_for_html(const LogColor &color)
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
void Logger::failed_open_log_file(const std::string &file)
{
    std::cerr << RED BOLD "Failed open log file: `" ITALIC << file << "'" RESET_CONSOLE_OUT << std::endl;
    exit(EXIT_FAILURE);
}

//----------------------------------------------------------------------------------------------

} /* namespace PineapLog */

//----------------------------------------------------------------------------------------------
