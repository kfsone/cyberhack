// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

// Standard types
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <iterator>
#include <optional>
#include <string_view>
#include <vector>

// Type aliases
using std::vector;

// Row describes a variably sized array of uint8s.
using Row = vector<uint8_t>;

// Matrix is a variably sized array of Rows, forming a 2dim matrix of uint8s.
using Matrix = vector<Row>;

// Point represents a matrix coordinate.
struct Point final
{
    size_t x_{0}, y_{0};

    // Equality comparison.
    constexpr bool operator==(const Point& rhs) const noexcept
    {
        return x_ == rhs.x_ && y_ == rhs.y_;
    }
};

// Sequence is a variably sized array representing a sequence of moves.
struct Sequence : public vector<Point>
{
    std::optional<size_t> find(Point p) const noexcept
    {
        if (auto it = std::find(cbegin(), cend(), p); it != cend()) {
            return std::distance(cbegin(), it);
        }
        return {};
    }
};
// using Sequence = vector<Point>;

// Candidate is a
struct Candidate final
{
    Point    target_;
    Sequence sequence_;
};

struct Result final
{
    Sequence       sequence_;
    vector<size_t> matches_;
    size_t         completed_;
    size_t         score_;

    constexpr bool operator<(const Result& rhs) const noexcept
    {
        return completed_ < rhs.completed_ || (completed_ == rhs.completed_ && score_ < rhs.score_);
    }
};
