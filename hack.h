// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#pragma once

#include "cyberhack.h"

struct Hack
{
    Matrix matrix_{};
    Matrix goals_{};

    Result winner_{};

protected:
    using OpenSet    = vector<Candidate>;
    using MatchCount = vector<size_t>;

    OpenSet    opened_{};
    MatchCount matches_{};

public:
    Hack()
    {
        matrix_.reserve(8 * 8);
        opened_.reserve(8 * 8);
    }

    void populate(std::string_view matrix, std::string_view goals) noexcept(false);

    void solve(const size_t bufferSize) noexcept(false);

    size_t matrixDim() const noexcept { return matrix_[0].size(); }

    size_t testPattern(const Row& goal, const Sequence& sequence, size_t bufferSize) noexcept
    {
        size_t progress{0};
        for (size_t turn = 0; turn < sequence.size() && turn < bufferSize; turn++) {
            const auto& point  = sequence[turn];
            const auto& target = matrix_[point.y_][point.x_];
            if (target == goal[progress]) {
                progress++;
                if (progress == goal.size())
                    break;
            } else {
                progress = 0;
            }
        }
        return progress;
    }
};
