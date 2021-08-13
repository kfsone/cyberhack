// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#include "log.h"

#include "imguiwrap.dear.h"

Logger gLogger{};

void
Logger::Clear() noexcept
{
    buffer_.clear();
    offsets_.clear();
    offsets_.push_back(0);
}

Logger&
Logger::operator<<(std::string_view s) noexcept
{
    buffer_.append(s.data(), s.data() + s.length());
    return *this;
}

void
Logger::Draw() noexcept
{
    if (!open_ || buffer_.empty())
        return;

    static constexpr ImGuiWindowFlags LogWindowFlags{ImGuiWindowFlags_None};

    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    dear::Begin("Log", &open_, LogWindowFlags) && [&] {
        ImGui::TextUnformatted(buffer_.begin(), buffer_.end());

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
    };
}

void
Logger::DrawShowMenuOption() noexcept
{
    dear::MenuItem("Show Log", &open_, buffer_.empty() == false);
}
