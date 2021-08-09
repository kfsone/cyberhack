// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

#include <string_view>
using std::string_view;

struct Problem
{
    string_view name_;
    size_t      buffer_;
    string_view matrix_;
    string_view goals_;
};
