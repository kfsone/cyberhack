// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#include "log.h"

#include "imgui.h"
#include "imguiwrap.dear.h"

#include <string_view>

struct Logger final
{
    bool            open_{false};
    ImGuiTextBuffer buffer_{};
    ImVector<int>   offsets_{};

    Logger() noexcept {}

    void Clear() noexcept
    {
        buffer_.clear();
        offsets_.clear();
        offsets_.push_back(0);
    }

    void operator<<(std::string_view s) noexcept { buffer_.append(s.data(), s.data() + s.length()); }

    static constexpr ImGuiWindowFlags logWindowFlags{ImGuiWindowFlags_None};

    void Draw() noexcept
    {
        if (!open_ || buffer_.empty())
            return;

        // static WindowFlagEditor flagEditor("Log Window"s, logwindow_flags);
        // flagEditor.draw();

        ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
        dear::Begin("Log", &open_, logWindowFlags) && [&] {
            ImGui::TextUnformatted(buffer_.begin(), buffer_.end());

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        };
    }
};

static Logger sLogger{};

void
guiLog(std::string text) noexcept
{
    sLogger << text;
}

void showLog() noexcept
{
	sLogger.Draw();
}

void
addLogDebugOption() noexcept
{
    dear::MenuItem("Show Log", &sLogger.open_, sLogger.buffer_.empty() == false);
}
            