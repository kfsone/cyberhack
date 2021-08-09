// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#include "hack.h"
#include "cyberhack.h"
#include "error.h"
#include "log.h"

#include <numeric>

void
Hack::populate(std::string_view matrix, std::string_view goals) noexcept(false)
{
    constexpr auto populateMatrix = [](std::string_view text, Matrix& into) {
        bool needRow{true};
        while (!text.empty()) {
            if (isspace(text.front()) || text.front() == 0)
                text.remove_prefix(1);
            else {
                uint8_t byte{0};
                if (auto [_, ec] = std::from_chars(text.data(), text.data() + 2, byte, 16);
                    ec != std::errc())
                    throw Error("Unable to read matrix");
                text.remove_prefix(2);
                if (needRow)
                    into.resize(into.size() + 1);
                into.back().push_back(byte);
                needRow = (!text.empty() && text.front() == '\n');
            }
        }
    };

    matrix_.clear();
    goals_.clear();
    matches_.clear();

    if (matrix.empty())
        throw Error("Please input a problem matrix first.");
    if (goals.empty())
        throw Error("Please input a goals matrix first.");

    populateMatrix(matrix, matrix_);

    // Make sure the matrix is regular.
    const size_t matrixDim = matrix_.size();
    for (const auto& row : matrix_) {
        if (row.size() != matrixDim)
            throw Error("Matrix is irregular");
    }

    populateMatrix(goals, goals_);
}

void
Hack::solve(const size_t bufferSize) noexcept(false)
{
    opened_.clear();
    opened_.reserve(bufferSize * bufferSize);
    matches_.clear();
    matches_.resize(goals_.size());
    winner_ = Result{};

    for (size_t i = 0; i < matrixDim(); i++)
        opened_.emplace_back(Candidate{{i, 0}, {}});

    while (!opened_.empty()) {
        auto [target, sequence] = opened_.back();
        opened_.pop_back();
        sequence.push_back(target);

        size_t completed{0};
        for (size_t i = 0; i < matches_.size(); i++) {
            matches_[i] = testPattern(goals_[i], sequence, bufferSize);
            if (matches_[i] == goals_[i].size())
                ++completed;
        }

        if (completed) {
            size_t score =
                std::accumulate(matches_.cbegin(), matches_.cend(), bufferSize - sequence.size());

            bool wins = completed > winner_.completed_;
            if (!wins && completed == winner_.completed_) {
                if (score == winner_.score_) {
                    if (sequence.size() < winner_.sequence_.size())
                        wins = true;
                } else {
                    wins = score > winner_.score_;
                }
            }
            if (wins) {
                Log("completed: ", completed, ", matches:");
                for (const auto& match : matches_) {
                    Log(" ", match);
                }
                Log(", score: ", score, ", len: ", sequence.size(), "\n");
                if (winner_.completed_ > 0) {
                    Log("+-> beat ", winner_.completed_, ",");
                    for (const auto& match : winner_.matches_) {
                        Log(" ", match);
                    }
                    Log(", ", winner_.score_, ", ", winner_.sequence_.size(), "\n");
                } else {
                    Log("+-> First winner\n");
                }

                winner_ = Result{sequence, matches_, completed, score};
            }
        }

        if (sequence.size() == bufferSize || completed == goals_.size())
            continue;

        // each odd - numbered turn is vertical, even - numbered horizontal
        bool verticalMove = (sequence.size() & 1) == 1;
        for (size_t i = 0; i < matrixDim(); i++) {
            if (verticalMove)
                target.y_ = i;
            else
                target.x_ = i;

            if (std::find(sequence.cbegin(), sequence.cend(), target) == sequence.cend())
                opened_.emplace_back(target, sequence);
        }
    }

    if (winner_.completed_ == 0)
        throw Error("No solution found.");
}
