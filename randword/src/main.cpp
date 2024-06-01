// #include <synchapi.h>
#include <array>
#include <expected>
#include <fstream>
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

static void read_line_and_type_char(std::ifstream& ifs, std::span<char> buffer, u64& lines_to_skip) noexcept {
    isize const len_excluding_newline = cast(isize, buffer.size()) - 1;
    if (ifs.getline(buffer.data(), len_excluding_newline)) {
        auto line_view = std::string_view(buffer.data(), cast(usize, ifs.gcount() - 1));
        // if (not line_view.empty() && line_view.back() == '\n') {
        //     line_view.remove_suffix(1); // Remove the newline character
        // }
        type_out_characters(line_view);
    }
    else {
        ifs.close();
        ifs = std::ifstream("./words.txt");
        lines_to_skip = 0;
    }
}

static void poll_even(HWND window_handle, std::ifstream& ifs, std::span<char> buffer, u64& lines_to_skip) noexcept {
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
                read_line_and_type_char(ifs, buffer, lines_to_skip);
                ++lines_to_skip;
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
    isize off = 0;
    auto file_line = std::fstream("./skipline.dat");
    if (not file_line.is_open()) {
        MessageBoxW(
            nullptr,
            L"Unable to find a file named `skipline.dat` in curent dir. Continuing from 0",
            L"File Open Error",
            MB_OK | MB_ICONEXCLAMATION
        );
    }
    else {
        auto const buffer =
            std::string((std::istreambuf_iterator<char>(file_line)), (std::istreambuf_iterator<char>()));
        off = cast(isize, buffer.size());
        auto [ptr, ec] = std::from_chars(buffer.data(), buffer.data() + buffer.size(), lines_to_skip);
        if (ec != std::errc()) {
            lines_to_skip = 0;
            MessageBoxW(
                nullptr,
                L"Can not convert the data in `kastkube.dat` to a number. Continuing from 0",
                L"Conversion error",
                MB_OK | MB_ICONEXCLAMATION
            );
        }
    }
    defer(file_line.close()); // comment out to close faster


    auto ifs = std::ifstream("./words.txt");
    if (not ifs.is_open()) {
        MessageBoxW(
            nullptr,
            L"Unable to find a file named `words.txt` in curent dir",
            L"File Open Error",
            MB_OK | MB_ICONEXCLAMATION
        );
        return EXIT_FAILURE;
    }
    defer(ifs.close()); // comment out to close faster


    auto buffer = std::array<char, BUFFER_SIZE>();
    for (usize i = 0; i < lines_to_skip && ifs.getline(buffer.data(), buffer.size()); ++i) {
        // Just read and discard the line
    }

    while (g_IS_RUNNING) {
        poll_even(window_res->h_window, ifs, buffer, lines_to_skip);
        Sleep(MILISECONDS_SLEEP);
    }

    UnregisterHotKey(window_res->h_window, 1);

    file_line.seekp(-static_cast<std::streamoff>(off), std::ios::cur);
    file_line << lines_to_skip;
}
#ifdef _CONSOLE
int main() {
    return WinMain(GetModuleHandleW(nullptr), nullptr, GetCommandLineA(), SW_SHOWNORMAL);
}

#endif
