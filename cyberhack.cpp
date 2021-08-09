// CyberPunk ICE/Hack solver
// Copyright (C) Oliver "kfsone" Smith <oliver@kfs.org> 2021

#include "cyberhack.h"
#include "error.h"
#include "hack.h"
#include "log.h"
#include "problem.h"

#include <optional>

// GUI headers
#include "imgui.h"
#include "imguiwrap.dear.h"
#include "imguiwrap.helpers.h"

const size_t     testBuffer = 6;
constexpr size_t maxDim     = 10;

static const char testProblem[] = R"(
1CE9BD1C1C
1C1CBDE9BD
E9E9BDE955
55BD1CE91C
BD1CE9BD1C
)";

static const char testGoals[] = R"(
E9BDBD
1C55E9
E9BD1C
)";

enum class Mode : int
{
    Edit,
    Solve,
};
static Mode        sMode{Mode::Edit};
static bool        sChanged{true};
static std::string sProblem{""};
static std::string sGoals{""};
static int         sBufferSize{testBuffer};
static bool        sSolved{false};
static Error       sSolutionError;
static bool        sExit{false};

void
mainMenuBar() noexcept
{
    dear::MainMenuBar() && [] {
        dear::Menu("File", true) && [] {
            if (dear::MenuItem("Quit"))
                sExit = true;
        };
        dear::Menu("Debug", true) && [] {
            addLogDebugOption();
        };
    };
}

void
drawOptions() noexcept
{
    constexpr ImGuiTableFlags tableFlags{ImGuiTableFlags_SizingStretchProp |
                                         ImGuiTableFlags_Borders};
    bool                      wasEditing = sMode == Mode::Edit;
    ImGui::RadioButton("Edit", (int*)&sMode, (int)Mode::Edit);
    ImGui::SameLine();
    ImGui::RadioButton("Solve", (int*)&sMode, (int)Mode::Solve);
    ImGui::SameLine();
    if (wasEditing && sMode == Mode::Solve)
        sChanged = true;

    int bufferSize{sBufferSize};
    ImGui::DragInt("Buffer Size", &bufferSize, 0.5f, 3, 10, "%d", ImGuiSliderFlags_AlwaysClamp);
    if (bufferSize != sBufferSize) {
        sBufferSize = bufferSize;
        sChanged    = true;
    }
}

void
drawOrder(const vector<ImVec2>& steps)
{
    static const ImVec2    StartOffset(8.0f, 20.0f);
    static const ImVec2    EndOffset(8.0f, 20.0f);
    static constexpr float ArrowLen      = 3.0f;
    static constexpr float LineThickness = 1.0f;
    static constexpr auto  LineColor     = IM_COL32(255, 120, 80, 160);
    static constexpr auto  HeadColor     = IM_COL32(255, 110, 70, 120);

    if (steps.empty())
        return;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for (size_t i = 0; i < steps.size() - 1; i++) {
        auto start = steps[i], end = steps[i + 1];
        start.x += StartOffset.x;
        start.y += StartOffset.y;
        end.x += EndOffset.x;
        end.y += EndOffset.y;
        ImVec2 arrowLeft, arrowRight;
        if (end.x != start.x) {
            // horizontal
            float xfac = end.x < start.x ? -1.0f : 1.0f;
            start.x += xfac * 2.f;
            end.x -= xfac * 2.f;
            arrowLeft.x  = end.x - xfac * ArrowLen;
            arrowLeft.y  = end.y + ArrowLen;
            arrowRight.x = end.x - xfac * ArrowLen;
            arrowRight.y = end.y - ArrowLen;
        } else {
            float yfac = end.y < start.y ? -1.0f : 1.0f;
            start.y -= yfac * 2.f;
            end.y += yfac * 2.f;
            arrowLeft.x  = end.x + ArrowLen;
            arrowLeft.y  = end.y - yfac * ArrowLen;
            arrowRight.x = end.x - ArrowLen;
            arrowRight.y = end.y - yfac * ArrowLen;
        }
        drawList->AddLine(end, arrowLeft, HeadColor, LineThickness);
        drawList->AddLine(arrowLeft, arrowRight, HeadColor, LineThickness);
        drawList->AddLine(arrowRight, end, HeadColor, LineThickness);
        drawList->AddLine(end, start, LineColor, LineThickness);
    }
}

static int
InputCallback(ImGuiInputTextCallbackData* data) noexcept
{
    std::string* string = reinterpret_cast<std::string*>(data->UserData);
    string->resize(data->BufTextLen);
    data->Buf = const_cast<char*>(string->c_str());
    return 0;
}

static void
editString(ImVec2 size, const char* id, std::string& field) noexcept
{
    constexpr ImGuiInputTextFlags flags{ImGuiInputTextFlags_CharsHexadecimal |
                                        ImGuiInputTextFlags_CharsUppercase |
                                        ImGuiInputTextFlags_CallbackResize};
    ImGui::InputTextMultiline(id, const_cast<char*>(field.c_str()), field.capacity() + 1, size,
                              flags, InputCallback, &field);
}

void
solveProblem(ImVec2 size, const Hack& hack) noexcept
{
    constexpr ImGuiTableFlags tableFlags{ImGuiTableFlags_SizingStretchSame |
                                         ImGuiTableFlags_Borders};
    dear::Table("#problem", maxDim, tableFlags, size, 0) && [&]() {
        if (hack.matrix_.empty())
            return;
        const float  TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing() * 2;
        const auto&  solution         = hack.winner_.sequence_;
        const size_t dim              = hack.matrixDim();
        char         value[16];

        vector<ImVec2> steps{};
        steps.resize(solution.size());

        for (size_t y = 0; y < maxDim; y++) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, TEXT_BASE_HEIGHT);
            for (size_t x = 0; x < dim; x++) {
                if (ImGui::TableNextColumn()) {
                    if (y < dim && x < maxDim) {
                        ImVec2 here = ImGui::GetCursorScreenPos();
                        snprintf(value, sizeof(value), "%02X", hack.matrix_[y][x]);
                        auto       stepNo   = solution.find(Point{x, y});
                        const bool selected = sSolved && stepNo.has_value();
                        dear::WithStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f)) &&
                            [&]() {
                                dear::Selectable(value, selected);
                                if (selected) {
                                    steps[stepNo.value()] = here;
                                    snprintf(value, sizeof(value), "  %zu", stepNo.value() + 1);
                                    ImGui::Text(value);
                                }
                            };
                    } else {
                        dear::Text("");
                    }
                }
            }
        }

        drawOrder(steps);
    };
}

void
drawProblem(const Hack& hack) noexcept
{
    const float TEXT_BASE_WIDTH  = ImGui::CalcTextSize("W00W").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing() * 2;

    auto size = ImVec2(TEXT_BASE_WIDTH * maxDim, TEXT_BASE_HEIGHT * maxDim);
    if (sMode == Mode::Edit)
        editString(size, "##problemtext", sProblem);
    else
        solveProblem(size, hack);
}

void
showGoals(const Hack& hack, ImVec2 size) noexcept
{
    constexpr ImGuiTableFlags tableFlags{ImGuiTableFlags_SizingStretchSame |
                                         ImGuiTableFlags_Borders};
    dear::Table("#goals", 10, tableFlags, size, 0) && [&]() {
        const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing() * 2;
        char        value[8];
        for (size_t i = 0; i < hack.goals_.size(); i++) {
            const bool goalSolved = sSolved && hack.winner_.matches_[i] == hack.goals_[i].size();
            ImGui::TableNextRow(ImGuiTableRowFlags_None, TEXT_BASE_HEIGHT);
            for (const auto& col : hack.goals_[i]) {
                if (ImGui::TableNextColumn()) {
                    snprintf(value, sizeof(value), "%02X", col);
                    dear::WithStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f)) &&
                        [&]() {
                            ImGui::Selectable(value, sSolved && goalSolved);
                        };
                }
            }
        }
    };
}

void
drawGoals(const Hack& hack) noexcept
{
    const float TEXT_BASE_WIDTH  = ImGui::CalcTextSize("W00W").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing() * 2;

    ImVec2 size(TEXT_BASE_WIDTH * maxDim, TEXT_BASE_HEIGHT * maxDim);
    if (sMode == Mode::Edit)
        editString(size, "##goalstext", sGoals);
    else
        showGoals(hack, size);
}

void
refreshProblem(Hack& hack)
{
    sSolutionError.what_.clear();
    sSolved  = false;
    sChanged = false;
    try {
        hack.populate(sProblem, sGoals);
        hack.solve(sBufferSize);
        sSolved = true;
    } catch (const Error& e) {
        sSolutionError.what_ = e.what_;
        ImGui::OpenPopup("WHOOPS");
    }
}

int
WinMain()
{
    static Hack hack{};

    ImGuiWrapConfig config{
        .windowTitle_ = "CyberHack2077",
        .width_       = 640,
        .height_      = 480,
    };

    sProblem.reserve(5 * 10 * 10 + 16);
    sProblem = testProblem;
    sGoals.reserve(5 * 10 * 8 + 16);
    sGoals = testGoals;

    imgui_main(config, []() -> ImGuiWrapperReturnType {
        if (sExit)
            return 0;

        if (sMode == Mode::Solve && sChanged)
            refreshProblem(hack);

        constexpr static ImGuiWindowFlags windowFlags{
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse};
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y),
                                 ImGuiCond_Always);
        bool open = dear::Begin("##main", nullptr, windowFlags) && []() {
            mainMenuBar();
            drawOptions();
            ImGui::NewLine();
            drawProblem(hack);
            ImGui::SameLine();
            drawGoals(hack);
        };

		showLog();

        if (!sSolutionError.what_.empty()) {
            if (ImGui::BeginPopup("WHOOPS")) {
                dear::Text(sSolutionError.what_);
                ImGui::EndPopup();
            }
        }

        return {};
    });
}
