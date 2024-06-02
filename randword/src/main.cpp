// #include <synchapi.h>
#include <array>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <expected>
#include <span>
#include <string_view>
#include "defines.hpp"
#include "win_include.hpp"


// #define DEBUG(...) (void)fprintf(stderr, __VA_ARGS__) // NOLINT
#define DEBUG(...)


constexpr i32 const g_WIDTH = 330;
constexpr i32 const g_HEIGHT = 100;
constexpr i32 const MILISECONDS_SLEEP = 19 * 2;
constexpr usize const BUFFER_SIZE = 128;
constexpr usize const SKILINE_NUMBER_SIZE = 8;


static bool g_IS_RUNNING = true;

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, usize wParam, isize lParam) {
    switch (uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {
            g_IS_RUNNING = false;
            DEBUG("WM_DESTROY\n");
            return 0;
        } /* break; */

        default:
            return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}


struct Window {
    static constexpr const auto CLASS_NAME{L"DXWindow"};

    NODIS inline static auto init(HINSTANCE instance, i32 n_cmd_show, const i32 width, const i32 height)
        -> std::expected<Window, std::string_view> {
        auto wcex = WNDCLASSEXW{
            .style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW,
            .lpfnWndProc = WindowProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = instance,
            .hIcon = nullptr,
            .hCursor = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = CLASS_NAME,
            .hIconSm = nullptr,
        };

        wcex.cbSize = sizeof(wcex);

        if (RegisterClassExW(&wcex) == 0) {
            return std::unexpected(std::string_view("RegisterClassEx\n"));
        }
        constexpr i32 const OFFSET = 100;

        auto rect = RECT{
            .left = OFFSET,
            .top = OFFSET,
            .right = OFFSET + width,
            .bottom = OFFSET + height,
        };

        i64 const style{WS_CAPTION | WS_GROUP | WS_SYSMENU | WS_SIZEBOX};
        if (AdjustWindowRect(&rect, style, 0) == 0) {
            return std::unexpected(std::string_view("AdjustWindowRect failed\n"));
        }

        auto const window = Window{
            .h_window = CreateWindowExW(
                0,
                CLASS_NAME,
                L"Ctrl+Alt+X. Close When Done",
                style,
                rect.left,
                rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                nullptr,
                nullptr,
                instance,
                nullptr
            ),
            .h_instatnce = instance,
        };


        if (window.h_window == nullptr) {
            return std::unexpected(std::string_view("CreateWindowExA failed\n"));
        }

        (void)ShowWindow(window.h_window, n_cmd_show);

        return window;
    }

    void deinit() const noexcept {
        (void)UnregisterClassW(CLASS_NAME, this->h_instatnce);
        (void)DestroyWindow(this->h_window);
    }

    HWND h_window;
    HINSTANCE h_instatnce;
};


// chatGPT generated
// 21 is the maximum number of characters needed for 64-bit size_t (20 digits + sign)
template <usize N = 21>
constexpr auto size_t_to_array(usize num) noexcept -> std::array<char, N> {
    auto result = std::array<char, N>({});
    auto iter = std::rbegin(result);
    do {
        *iter++ = '0' + (num % 10);
        num /= 10;
    }
    while (num != 0);

    auto res = std::array<char, N>({});
    usize i = 0;
    for (char ele : result) {
        if (ele != '\0') {
            res[i] = ele;
            ++i;
        }
    }

    return res;
}

// chatGPT generated
static void type_out_characters(std::string_view const str_view) noexcept {
    for (char chr : str_view) {
        auto keyboard_input = KEYBDINPUT{};
        auto input = INPUT{};

        // Set the virtual key code
        SHORT vkey = VkKeyScan(chr);
        if (vkey == -1) {
            DEBUG("Cannot map character to virtual key: %c\n", chr);
            continue;
        }

        // Set up the INPUT structure for key press
        keyboard_input.wVk = LOBYTE(vkey);
        keyboard_input.wScan = cast(u16, MapVirtualKey(keyboard_input.wVk, MAPVK_VK_TO_VSC));
        keyboard_input.dwFlags = 0;
        input.type = INPUT_KEYBOARD;
        input.ki = keyboard_input;
        (void)SendInput(1, &input, sizeof(INPUT));

        // Set up the INPUT structure for key release
        keyboard_input.dwFlags = KEYEVENTF_KEYUP;
        input.ki = keyboard_input;
        (void)SendInput(1, &input, sizeof(INPUT));
    }
}

static void read_line_and_type_char(std::FILE* ifile, std::span<char> buffer, u64& lines_to_skip) noexcept {
    i32 const len_excluding_newline = cast(i32, buffer.size()) - 1;
    if (std::fgets(buffer.data(), len_excluding_newline, ifile) != nullptr) {
        usize const line_len = std::strlen(buffer.data()) - 1; // Remove the newline character
        auto const line_view = std::string_view(buffer.data(), line_len);

        type_out_characters(line_view);
        ++lines_to_skip;
    }
    else {
        std::rewind(ifile);
        lines_to_skip = 0;
    }
}

static void poll_even(HWND window_handle, std::FILE* ifile, std::span<char> buffer, u64& lines_to_skip) noexcept {
    MSG msg;
    while (PeekMessageW(&msg, window_handle, 0, 0, PM_REMOVE) != 0) {
        if (msg.message == WM_QUIT) {
            g_IS_RUNNING = false;
            return;
        }
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == 1) {
                // MessageBoxW(nullptr, L"Ctrl+Alt+X pressed!", L"Global Hotkey", MB_ICONINFORMATION);
                Sleep(300);
                read_line_and_type_char(ifile, buffer, lines_to_skip);
                break; // Exit the message loop after handling the hotkey
            }
        }
        // TranslateMessage(&msg);
        (void)DispatchMessageW(&msg);
        DEBUG("loop innter\n");
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int nShowCmd) {
    auto window_res = Window::init(hInstance, nShowCmd, g_WIDTH, g_HEIGHT);
    if (not window_res.has_value()) {
        auto const err = window_res.error();
        MessageBoxW(nullptr, reinterpret_cast<LPCWSTR>(err.data()), L"Window Error", MB_OK | MB_ICONEXCLAMATION);
        return EXIT_FAILURE;
    }
    // defer(window_res->deinit()); // comment out to close faster

    // Register Ctrl+Alt+X as a global hotkey
    if (RegisterHotKey(window_res->h_window, 1, MOD_CONTROL | MOD_ALT, 'X') == 0) {
        MessageBoxW(
            nullptr,
            L"Unable to register a global hotkey",
            L"RegisterHotKey Error",
            MB_OK | MB_ICONEXCLAMATION
        );
        return EXIT_FAILURE;
    }

    u64 lines_to_skip = 0;
    usize off = 0;
    gsl::owner<std::FILE*> file_line = std::fopen("./skipline.dat", "r+");
    if (file_line == nullptr) {
        MessageBoxW(
            nullptr,
            L"Unable to find a file named `skipline.dat` in curent dir. Continuing from 0",
            L"File Open Error",
            MB_OK | MB_ICONEXCLAMATION
        );
    }
    else {
        (void)std::fseek(file_line, 0, SEEK_END);
        off = cast(usize, std::ftell(file_line));
        std::rewind(file_line);

        auto file_line_buff = std::array<char, SKILINE_NUMBER_SIZE>();
        usize const n_read_successfully = std::fread(file_line_buff.data(), 1, off, file_line);
        if (n_read_successfully == static_cast<size_t>(off)) {
            // Convert the data in the buffer to a number
            auto [ptr, ec] =
                std::from_chars(file_line_buff.data(), file_line_buff.data() + file_line_buff.size(), lines_to_skip);
            if (ec != std::errc()) {
                lines_to_skip = 0;
                MessageBoxW(
                    nullptr,
                    L"Cannot convert the data in `skipline.dat` to a number. Continuing from 0",
                    L"Conversion Error",
                    MB_OK | MB_ICONEXCLAMATION
                );
            }
        }
        else {
            lines_to_skip = 0;
            MessageBoxW(
                nullptr,
                L"Error reading the file `skipline.dat`. Continuing from 0",
                L"File Read Error",
                MB_OK | MB_ICONEXCLAMATION
            );
        }
    }
    defer(std::fclose(file_line)); // comment out to close faster

    DEBUG("lines_to_skip %llu\n", lines_to_skip);

    gsl::owner<std::FILE*> ifile = std::fopen("./words.txt", "r");
    if (ifile == nullptr) {
        MessageBoxW(
            nullptr,
            L"Unable to find a file named `words.txt` in curent dir",
            L"File Open Error",
            MB_OK | MB_ICONEXCLAMATION
        );
        return EXIT_FAILURE;
    }
    defer(std::fclose(ifile)); // comment out to close faster


    auto buffer = std::array<char, BUFFER_SIZE>();
    for (usize i = 0; i < lines_to_skip && (std::fgets(buffer.data(), buffer.size(), ifile) != nullptr); ++i) {
        DEBUG("%zu\n", i + 1);
        // Just read and discard the line
    }


    while (g_IS_RUNNING) {
        poll_even(window_res->h_window, ifile, buffer, lines_to_skip);
        Sleep(MILISECONDS_SLEEP);
    }

    UnregisterHotKey(window_res->h_window, 1);


    auto const skpline_array = size_t_to_array<SKILINE_NUMBER_SIZE>(lines_to_skip);
    (void)std::fseek(file_line, 0, SEEK_SET);
    (void)fwrite(skpline_array.data(), sizeof(char), skpline_array.size(), file_line);
}
#ifdef _CONSOLE
int main() {
    return WinMain(GetModuleHandleW(nullptr), nullptr, GetCommandLineA(), SW_SHOWNORMAL);
}

#endif
