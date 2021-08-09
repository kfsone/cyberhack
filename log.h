// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

#include <string>

extern void showLog() noexcept;
extern void guiLog(std::string) noexcept;
extern void addLogDebugOption() noexcept;

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
    (guiLog(format(args)), ...);
}
