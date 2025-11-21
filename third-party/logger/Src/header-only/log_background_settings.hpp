#pragma once

//-----------------------------------------------------------------------------------------------------------------

// private macsros, undef in end of this file
#define endl1_str ON_TAB("\n")
#define endl2_str ON_TAB("\n\n")

#define tab1_str ON_TAB("\t")
#define tab2_str ON_TAB("\t\t")
#define tab3_str ON_TAB("\t\t\t")
#define tab4_str ON_TAB("\t\t\t\t")
#define tab5_str ON_TAB("\t\t\t\t\t")

//-----------------------------------------------------------------------------------------------------------------

#define GLOBAL_HTML_SETTINGS_1                                                                                         \
    "<html>" endl1_str tab1_str "<head>" endl1_str tab2_str                                                            \
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">" endl1_str tab2_str                    \
    "<style>" endl1_str tab3_str

//-----------------------------------------------------------------------------------------------------------------

#define GLOBAL_HTML_SETTINGS_2                                                                                         \
    ".text-section {" endl1_str tab4_str "padding: 30px;" endl1_str tab4_str "border-radius: 10px;" endl1_str tab4_str \
    "max-width: 800px;" endl1_str tab4_str "margin: 20px auto;" endl1_str tab3_str "}" endl2_str tab3_str              \
    "h1 {" endl1_str tab4_str "text-align: center;" endl1_str tab4_str "margin-bottom: 5px;" endl1_str tab3_str        \
    "}" endl2_str tab3_str "h2 {" endl1_str tab4_str "margin-bottom: 5px;" endl1_str tab3_str "}" endl2_str tab3_str   \
    "p {" endl1_str tab4_str "line-height: 0.5;" endl1_str tab4_str "text-align: justify;" endl1_str tab4_str          \
    "margin: 10px 0;" endl1_str tab3_str "}" endl2_str tab3_str ".color {" endl1_str tab4_str                          \
    "padding: 0px 10px;" endl1_str tab4_str "margin: 1px 0;" endl1_str tab4_str "line-height: 0.5;" endl1_str tab3_str \
    "}" endl2_str tab3_str ".color p {" endl1_str tab4_str "White-space: pre;" endl1_str tab4_str                      \
    "padding: 0;" endl1_str tab4_str "margin: 0;" endl1_str tab4_str "line-height: 1.1;" endl1_str tab3_str            \
    "}" endl2_str tab3_str "body::before {" endl1_str tab4_str "content: \"\";" endl1_str tab4_str                     \
    "position: fixed;" endl1_str tab4_str "top: 0;" endl1_str tab4_str "left: 0;" endl1_str tab4_str                   \
    "right: 0;" endl1_str tab4_str "bottom: 0;" endl1_str tab4_str                                                     \
    "background: rgba(0, 0, 0, 0.5);" endl1_str tab4_str "z-index: -1;" endl1_str tab4_str                             \
    "pointer-events: none;" endl1_str tab3_str "}" endl2_str tab3_str ".red { color: #ff4444; }" endl1_str tab3_str    \
    ".green { color: #0bf80b; }" endl1_str tab3_str ".pink { color: #f605c7; }" endl1_str tab3_str                     \
    ".yellow { color: #ecc40b; }" endl1_str tab3_str ".blue { color: #0c89e8; }" endl1_str tab3_str                    \
    ".black { color: #000000; }" endl1_str tab3_str ".white { color: #ffffff; }" endl1_str tab2_str                    \
    "</style>" endl1_str tab1_str "</head>" endl2_str tab1_str "<body>" endl1_str tab2_str                             \
    "<div class=\"text-section\">" endl1_str

//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------

// public macro
#define PLAIN_BACKGROUND_HTML_SETTINGS                                                                                 \
    GLOBAL_HTML_SETTINGS_1                                                                                             \
    "body {" endl1_str tab4_str "margin: 0;" endl1_str tab4_str "padding: 0;" endl1_str tab4_str                       \
    "height: 100vh;" endl1_str tab4_str "position: relative;" endl1_str tab4_str                                       \
    "background-color: #000000;" endl1_str tab4_str "background-attachment: fixed;" endl1_str tab4_str                 \
    "background-size: 400\% 400\%;" endl1_str tab4_str "font-family: Arial, sans-serif;" endl1_str tab4_str            \
    "color: White;" endl1_str tab3_str "}" endl2_str tab3_str GLOBAL_HTML_SETTINGS_2

//-----------------------------------------------------------------------------------------------------------------

// public macro
#define GRADIENT_BACKGROUND_HTML_SETTINGS                                                                              \
    GLOBAL_HTML_SETTINGS_1                                                                                             \
    "body {" endl1_str tab4_str "margin: 0;" endl1_str tab4_str "padding: 0;" endl1_str tab4_str                       \
    "height: 100vh;" endl1_str tab4_str "position: relative;" endl1_str tab4_str                                       \
    "background: linear-gradient(135deg, #000000, #000000, #6600ff, #6600ff);" endl1_str tab4_str                      \
    "animation: gradient 15s ease infinite;" endl1_str tab4_str "background-attachment: fixed;" endl1_str tab4_str     \
    "background-size: 400\% 400\%;" endl1_str tab4_str "font-family: Arial, sans-serif;" endl1_str tab4_str            \
    "color: White;" endl1_str tab3_str "}" endl2_str tab3_str "@keyframes gradient {" endl1_str tab4_str               \
    "0% {" endl1_str tab5_str "background-position: 0% 50%;" endl1_str tab4_str "}" endl1_str tab4_str                 \
    "50% {" endl1_str tab4_str "background-position: 100% 50%;" endl1_str tab4_str "}" endl1_str tab4_str              \
    "100% {" endl1_str tab5_str "background-position: 0% 50%;" endl1_str tab4_str "}" endl1_str tab3_str               \
    "}" endl2_str tab3_str GLOBAL_HTML_SETTINGS_2

//-----------------------------------------------------------------------------------------------------------------

// public macro
#define IMAGE_BACKGROUND_HTML_SETTINGS(path_to_image)                                                                  \
    GLOBAL_HTML_SETTINGS_1                                                                                             \
    "body {" endl1_str tab4_str "margin: 0;" endl1_str tab4_str "padding: 0;" endl1_str tab4_str                       \
    "height: 100vh;" endl1_str tab4_str "background-image: url('",                                                     \
        path_to_image,                                                                                                 \
        "');" endl1_str tab4_str "background-size: cover;" endl1_str tab4_str                                          \
        "background-position: center;" endl1_str tab4_str "background-repeat: no-repeat;" endl1_str tab4_str           \
        "background-attachment: fixed;" endl1_str tab4_str "font-family: Arial, sans-serif;" endl1_str tab4_str        \
        "color: White;" endl1_str tab3_str "}" endl2_str tab3_str GLOBAL_HTML_SETTINGS_2

//-----------------------------------------------------------------------------------------------------------------

// public macro
#define HTML_END                                                                                                       \
    tab3_str "</span>" endl1_str tab2_str "</div>" endl1_str tab1_str "</body>" endl1_str "</html>" endl1_str

//-----------------------------------------------------------------------------------------------------------------
