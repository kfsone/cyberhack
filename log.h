// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

#include <string>
#include <string_view>

#include "imgui.h"

struct Logger final
{
    Logger() noexcept {}

    void Clear() noexcept;

    Logger& operator<<(std::string_view s) noexcept;

    void Draw() noexcept;

    void DrawShowMenuOption() noexcept;

public:
    bool            open_{false};
    ImGuiTextBuffer buffer_{};
    ImVector<int>   offsets_{};
};

extern Logger gLogger;

template<size_t N>
std::string
format(const char (&t)[N])
{
    return t;
}

template<typename T>
std::string
format(std::string t)
{
    return t;
}

template<typename T>
std::string
format(T&& t)
{
    return std::to_string(std::forward<T>(t));
}

template<typename... Args>
void
Log(Args&&... args)
{
    (gLogger.operator<<(format(args)), ...);
}
