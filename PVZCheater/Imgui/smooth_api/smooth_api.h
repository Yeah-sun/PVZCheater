#pragma once
#include "../imgui.h"
#include "../imgui_impl_dx11.h"
#include "../imgui_impl_win32.h"
#include "../imgui_internal.h"
#include "../imstb_rectpack.h"
#include "../imstb_textedit.h"
#include "../imstb_truetype.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <cmath>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <map>
#include <functional>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "dinput8.lib")

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace smooth_api
{
    namespace Toolbar
    {
        // ƽ�����ɵ�ѡ�������ࣨ���򲼾֣�
        // ���ܣ�ʵ�ֺ������еĵ�ѡ��ť��������֧��״̬���ɶ�������ͣЧ���͵ײ�����������
        class SmoothRadioToolbar {
        private:
            // ��ť״̬�붯����ر����Ľṹ��
            struct ButtonState {
                float transitionProgress = 0.0f; // ״̬���ɽ��ȣ�0.0 = δѡ��, 1.0 = ѡ�У�
                float hoverAlpha = 0.0f;         // ��ͣЧ��͸���ȣ�0.0 = ��Ч��, 1.0 = ���Ч����
            };

            // ��ʽ��״̬�洢����
            std::vector<ButtonState> buttonStates; // �洢���а�ť��״̬��Ϣ
            ImVec2 toolbarSize;                    // ����������ߴ磨����ߣ�
            ImVec2 buttonSize;                     // ������ť�ߴ磨����ߣ�
            float buttonSpacing;                   // ��ť֮��ļ��
            ImVec4 unselectedTitleColor;           // δѡ��״̬�µı���������ɫ
            ImVec4 selectedTitleColor;             // ѡ��״̬�µı���������ɫ
            ImVec4 unselectedBgColor;              // δѡ��״̬�µİ�ť����ɫ
            ImVec4 selectedBgColor;                // ѡ��״̬�µİ�ť����ɫ
            ImVec4 toolbarBgColor;                 // ���������屳��ɫ
            ImVec4 navBarColor;                    // ��������ɫ
            float toolbarRounding;                 // �������߽�Բ�ǰ뾶�����������º����£�
            float buttonRounding;                  // ��ť�߽�Բ�ǰ뾶
            float navBarCurrentX;                  // ��������ǰX���꣨���ڶ������ɣ�
            float titleOffsetY;                    // ���ⴹֱƫ����

            // ��������
            const float transitionSpeed = 5.0f;    // ״̬�л������ٶ�
            const float hoverSpeed = 12.0f;        // ��ͣЧ�������ٶ�
            const float navBarSpeed = 10.0f;       // �������ƶ��ٶ�

            // ��������
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

            // ��ʼ����ť״̬
            void InitButtonStates(int count, const bool* states) {
                buttonStates.resize(count);
                navBarCurrentX = 0.0f;

                for (int i = 0; i < count; ++i) {
                    buttonStates[i].transitionProgress = states[i] ? 1.0f : 0.0f;
                    buttonStates[i].hoverAlpha = 0.0f;

                    if (states[i]) {
                        navBarCurrentX = i * (buttonSize.x + buttonSpacing);
                    }
                }
            }

        public:
            // ���ƹ����������ķ�����
            static void Draw(
                const char* componentId,
                const char** titles,
                bool* states,
                int buttonCount,
                float titleOffsetY = 0.0f,
                float toolbarRounding = 0.0f,
                float buttonRounding = 0.0f,
                const ImVec2& toolbarSize = ImVec2(800, 60),
                const ImVec2& buttonSize = ImVec2(72, 60),
                float buttonSpacing = 3.0f,
                ImColor unselectedTitleColor = ImColor(141, 141, 141),
                ImColor selectedTitleColor = ImColor(205, 163, 173),
                ImColor unselectedBgColor = ImColor(37, 37, 37),
                ImColor selectedBgColor = ImColor(41, 41, 41),
                ImColor toolbarBgColor = ImColor(37, 37, 37),
                ImColor navBarColor = ImColor(207, 155, 166)
            ) {
                // ʵ������
                static std::unordered_map<std::string, SmoothRadioToolbar> instances;
                auto& instance = instances[componentId];

                // ����ʵ������
                instance.toolbarSize = toolbarSize;
                instance.buttonSize = buttonSize;
                instance.buttonSpacing = buttonSpacing;
                instance.toolbarRounding = toolbarRounding;
                instance.buttonRounding = buttonRounding;
                instance.unselectedTitleColor = unselectedTitleColor.Value;
                instance.selectedTitleColor = selectedTitleColor.Value;
                instance.unselectedBgColor = unselectedBgColor.Value;
                instance.selectedBgColor = selectedBgColor.Value;
                instance.toolbarBgColor = toolbarBgColor.Value;
                instance.navBarColor = navBarColor.Value;
                instance.titleOffsetY = titleOffsetY;

                // ��ʼ����ť״̬
                if (instance.buttonStates.size() != buttonCount) {
                    instance.InitButtonStates(buttonCount, states);
                }

                // ��ʼ����
                ImGui::PushID(componentId);
                ImVec2 toolbarStartPos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // ---------------------------
                // ���ƹ��������������ݾɰ汾ImGui����PathClose()��
                // �����º�����Ӧ��Բ�ǣ��Ϸ�����ֱ��
                // ---------------------------
                const float borderRadius = instance.toolbarRounding;
                const ImVec2 topLeft = toolbarStartPos;
                const ImVec2 bottomRight = ImVec2(
                    toolbarStartPos.x + toolbarSize.x,
                    toolbarStartPos.y + toolbarSize.y
                );

                // �ֶ�����·�������PathClose()�����ݾɰ汾��
                drawList->PathClear();

                // 1. ���Ͻǣ�ֱ�ǣ�
                const ImVec2 topLeftCorner = ImVec2(topLeft.x, topLeft.y);
                drawList->PathLineTo(topLeftCorner);

                // 2. ���Ͻǣ�ֱ�ǣ�
                const ImVec2 topRightCorner = ImVec2(bottomRight.x, topLeft.y);
                drawList->PathLineTo(topRightCorner);

                // 3. ���½ǣ�Բ�ǻ���
                drawList->PathArcTo(
                    ImVec2(bottomRight.x - borderRadius, bottomRight.y - borderRadius), // Բ������
                    borderRadius,                                                       // �뾶
                    0.0f,                                                                // ��ʼ�Ƕȣ��ҷ���
                    IM_PI * 0.5f,                                                       // �����Ƕȣ��Ϸ���
                    16                                                                   // �ֶ�����ƽ���ȣ�
                );

                // 4. ���½ǣ�Բ�ǻ���
                drawList->PathArcTo(
                    ImVec2(topLeft.x + borderRadius, bottomRight.y - borderRadius),     // Բ������
                    borderRadius,                                                       // �뾶
                    IM_PI * 0.5f,                                                       // ��ʼ�Ƕȣ��Ϸ���
                    IM_PI,                                                              // �����Ƕȣ�����
                    16                                                                   // �ֶ���
                );

                // �ֶ��պ�·�������PathClose()�������һ�������ӻ���㣩
                drawList->PathLineTo(topLeftCorner);

                // ���·����ʹ�ù���������ɫ��
                drawList->PathFillConvex(ImGui::ColorConvertFloat4ToU32(instance.toolbarBgColor));

                // ���㰴ť�鲼�֣��������У�
                const float totalButtonsWidth = buttonCount * buttonSize.x + (buttonCount - 1) * buttonSpacing;
                const float buttonsStartX = toolbarStartPos.x + ((toolbarSize.x - totalButtonsWidth) > 0 ? (toolbarSize.x - totalButtonsWidth) * 0.5f : 0);
                const float buttonsStartY = toolbarStartPos.y + (toolbarSize.y - buttonSize.y) * 0.5f;

                // ��¼���״̬��ѡ������
                int clickedIndex = -1;
                int selectedIndex = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedIndex = i;
                        break;
                    }
                }

                // ����ÿ����ť��������
                float currentX = buttonsStartX;
                for (int i = 0; i < buttonCount; ++i) {
                    if (currentX + buttonSize.x > toolbarStartPos.x + toolbarSize.x) break;

                    ImGui::PushID(i);
                    ImVec2 buttonPos = ImVec2(currentX, buttonsStartY);

                    // �������
                    ImGui::SetCursorScreenPos(buttonPos);
                    bool isClicked = ImGui::InvisibleButton("##button", buttonSize, ImGuiButtonFlags_PressedOnClick);
                    if (isClicked) {
                        clickedIndex = i;
                    }

                    // ״̬���ɶ���
                    const float targetProgress = states[i] ? 1.0f : 0.0f;
                    instance.buttonStates[i].transitionProgress = ImLerp(
                        instance.buttonStates[i].transitionProgress,
                        targetProgress,
                        ImGui::GetIO().DeltaTime * instance.transitionSpeed
                    );
                    const float progress = instance.SmoothStep(instance.buttonStates[i].transitionProgress);

                    // ��ͣ����
                    const bool isHovered = ImGui::IsItemHovered();
                    const float hoverTarget = isHovered ? 1.0f : 0.0f;
                    instance.buttonStates[i].hoverAlpha = ImLerp(
                        instance.buttonStates[i].hoverAlpha,
                        hoverTarget,
                        ImGui::GetIO().DeltaTime * instance.hoverSpeed
                    );

                    // ��ɫ����
                    const ImVec4 bgColor = ImLerp(
                        instance.unselectedBgColor,
                        instance.selectedBgColor,
                        progress
                    );
                    ImVec4 titleColor = ImLerp(
                        instance.unselectedTitleColor,
                        instance.selectedTitleColor,
                        progress
                    );

                    // ��ͣ����
                    if (instance.buttonStates[i].hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * instance.buttonStates[i].hoverAlpha);
                        titleColor.x = ImSaturate(titleColor.x * hoverFactor);
                        titleColor.y = ImSaturate(titleColor.y * hoverFactor);
                        titleColor.z = ImSaturate(titleColor.z * hoverFactor);
                    }

                    // ���ư�ť����
                    drawList->AddRectFilled(
                        buttonPos,
                        ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y),
                        ImGui::ColorConvertFloat4ToU32(bgColor),
                        instance.buttonRounding
                    );

                    // ���ư�ť����
                    const ImVec2 textSize = ImGui::CalcTextSize(titles[i]);
                    const ImVec2 textPos = ImVec2(
                        buttonPos.x + (buttonSize.x - textSize.x) * 0.5f,
                        buttonPos.y + (buttonSize.y - textSize.y) * 0.5f - instance.titleOffsetY
                    );
                    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(titleColor), titles[i]);

                    currentX += buttonSize.x + buttonSpacing;
                    ImGui::PopID();
                }

                // �������¼�����ѡ�߼���
                if (clickedIndex != -1) {
                    for (int j = 0; j < buttonCount; ++j) {
                        states[j] = (j == clickedIndex);
                    }
                }

                // ��ѡ״̬У��
                int selectedCount = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedCount++;
                        selectedIndex = i;
                    }
                }
                if (selectedCount != 1) {
                    for (int i = 0; i < buttonCount; ++i) states[i] = false;
                    states[0] = true;
                    selectedIndex = 0;
                }

                // ���������������
                const float targetNavBarX = buttonsStartX + selectedIndex * (buttonSize.x + buttonSpacing);
                instance.navBarCurrentX = ImLerp(
                    instance.navBarCurrentX,
                    targetNavBarX,
                    ImGui::GetIO().DeltaTime * instance.navBarSpeed
                );

                const float navBarHeight = 2.0f;
                const float navBarY = buttonsStartY + buttonSize.y - navBarHeight;
                drawList->AddRectFilled(
                    ImVec2(instance.navBarCurrentX, navBarY),
                    ImVec2(instance.navBarCurrentX + buttonSize.x, navBarY + navBarHeight),
                    ImGui::ColorConvertFloat4ToU32(instance.navBarColor),
                    navBarHeight * 0.5f
                );

                // �ƶ���굽�������·�
                ImGui::SetCursorScreenPos(ImVec2(
                    toolbarStartPos.x,
                    toolbarStartPos.y + toolbarSize.y + ImGui::GetStyle().ItemSpacing.y
                ));

                ImGui::PopID();
            }
        };

        // ƽ����������ѡ�������ࣨ����ѡ�б���ɫ����β����Ч����
        class SmoothVerticalRadioToolbar {
        private:
            // ��ť״̬�붯����ر����Ľṹ��
            struct ButtonState {
                float transitionProgress = 0.0f; // ״̬���ɽ��ȣ�0.0 = δѡ��, 1.0 = ѡ�У�
                float hoverAlpha = 0.0f;         // ��ͣЧ��͸����
            };

            // ��ʽ��״̬�洢����
            std::vector<ButtonState> buttonStates; // �����밴ť������Ӧ��0~buttonCount-1��
            ImVec2 toolbarSize;
            ImVec2 buttonSize;
            float baseButtonSpacing;
            float buttonSpacingOffset;
            ImVec4 unselectedTitleColor;
            ImVec4 selectedTitleColor;
            ImVec4 unselectedBgColor;
            ImVec4 selectedBgColor;       // ��β��ɫ����Ϊʹ��ѡ�б���ɫ��
            ImVec4 toolbarBgColor;
            ImVec4 navBarColor;
            float toolbarRounding;
            float buttonRounding;
            float navBarCurrentY;

            // ��������
            const float transitionSpeed = 5.0f;
            const float hoverSpeed = 12.0f;
            const float navBarSpeed = 10.0f;

            // ��������
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

            // ��ʼ����ť״̬
            void InitButtonStates(int count, const bool* states) {
                buttonStates.resize(count); // ȷ��������С�밴ť����һ��
                navBarCurrentY = 0.0f;

                for (int i = 0; i < count; ++i) {
                    buttonStates[i].transitionProgress = states[i] ? 1.0f : 0.0f;
                    buttonStates[i].hoverAlpha = 0.0f;

                    if (states[i]) {
                        const float actualSpacing = baseButtonSpacing + buttonSpacingOffset;
                        navBarCurrentY = i * (buttonSize.y + actualSpacing);
                    }
                }
            }

            // ����ѡ�а�ť����β���䱳��������selectedBgColor��
            // ������buttonIndex - ��ǰ��ť��������0~buttonCount-1��
            void DrawGradientBackground(ImDrawList* drawList, const ImVec2& buttonPos, int buttonIndex) {
                // ��β��ɫ��ʹ��ѡ��״̬�ı���ɫ��selectedBgColor��
                ImVec4 baseColor = selectedBgColor;
                // ������������ɵ���ϸ��ȣ�
                const int gradientSteps = 10;
                // ÿ�ο�� = ��ť��� / ���������������buttonSize.x��
                const float stepWidth = buttonSize.x / gradientSteps;

                // ��������Σ�segmentIdxΪ���������밴ť�������֣�
                for (int segmentIdx = 0; segmentIdx < gradientSteps; ++segmentIdx) {
                    // ���㵱ǰ�ε�λ��
                    float segmentX = buttonPos.x + segmentIdx * stepWidth;
                    ImVec2 segmentMin = ImVec2(segmentX, buttonPos.y);
                    ImVec2 segmentMax = ImVec2(segmentX + stepWidth, buttonPos.y + buttonSize.y);

                    // ����͸���ȣ��������𽥽��ͣ�0��=1.0�����һ�Ρ�0.0��
                    float alpha = 1.0f - (static_cast<float>(segmentIdx) / gradientSteps);
                    // �밴ťѡ�н����ںϣ����ɶ����ڼ佥��Ч�������֣�
                    alpha *= buttonStates[buttonIndex].transitionProgress;

                    // ���Ƶ�ǰ�Σ���ɫ=ѡ�б���ɫ��͸������λ��˥����
                    ImVec4 segmentColor = baseColor;
                    segmentColor.w = alpha; // ͸���Ȳ�ֵ��Խ����Խ͸����
                    drawList->AddRectFilled(
                        segmentMin,
                        segmentMax,
                        ImGui::ColorConvertFloat4ToU32(segmentColor),
                        buttonRounding
                    );
                }
            }

        public:
            static void Draw(
                const char* componentId,
                const char** titles,
                bool* states,
                int buttonCount,
                float buttonSpacingOffset = 0.0f,
                float toolbarRounding = 0.0f,
                float buttonRounding = 0.0f,
                const ImVec2& toolbarSize = ImVec2(170, 430),
                const ImVec2& buttonSize = ImVec2(170, 45),
                float baseButtonSpacing = 3.0f,
                ImColor unselectedTitleColor = ImColor(141, 141, 141),
                ImColor selectedTitleColor = ImColor(205, 163, 173),
                ImColor unselectedBgColor = ImColor(28, 28, 28),
                ImColor selectedBgColor = ImColor(41, 41, 41), // ��β��ɫ������ɫ�����ڽ��䣩
                ImColor toolbarBgColor = ImColor(28, 28, 28),
                ImColor navBarColor = ImColor(207, 155, 166)
            ) {
                static std::unordered_map<std::string, SmoothVerticalRadioToolbar> instances;
                auto& instance = instances[componentId];

                // ����ʵ���������ص㣺��selectedBgColor�洢Ϊ��β��ɫ��
                instance.toolbarSize = toolbarSize;
                instance.buttonSize = buttonSize;
                instance.baseButtonSpacing = baseButtonSpacing;
                instance.buttonSpacingOffset = buttonSpacingOffset;
                instance.toolbarRounding = toolbarRounding;
                instance.buttonRounding = buttonRounding;
                instance.unselectedTitleColor = unselectedTitleColor.Value;
                instance.selectedTitleColor = selectedTitleColor.Value;
                instance.unselectedBgColor = unselectedBgColor.Value;
                instance.selectedBgColor = selectedBgColor.Value; // �洢ѡ�б���ɫ��Ϊ�����ɫ
                instance.toolbarBgColor = toolbarBgColor.Value;
                instance.navBarColor = navBarColor.Value;

                // ȷ����ť״̬������С�밴ť����һ�£���Խ�磩
                if (instance.buttonStates.size() != buttonCount) {
                    instance.InitButtonStates(buttonCount, states);
                }

                ImGui::PushID(componentId);
                ImVec2 toolbarStartPos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // ���ƹ���������
                drawList->AddRectFilled(
                    toolbarStartPos,
                    ImVec2(toolbarStartPos.x + toolbarSize.x, toolbarStartPos.y + toolbarSize.y),
                    ImGui::ColorConvertFloat4ToU32(instance.toolbarBgColor),
                    instance.toolbarRounding
                );

                const float actualButtonSpacing = instance.baseButtonSpacing + instance.buttonSpacingOffset;
                const float buttonsStartX = toolbarStartPos.x;
                const float buttonsStartY = toolbarStartPos.y;

                int clickedIndex = -1;
                int selectedIndex = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) {
                        selectedIndex = i;
                        break;
                    }
                }

                // ������ť��iΪ��ť������
                float currentY = buttonsStartY;
                for (int i = 0; i < buttonCount; ++i) {
                    if (currentY + buttonSize.y > toolbarStartPos.y + toolbarSize.y) break;

                    ImGui::PushID(i);
                    ImVec2 buttonPos = ImVec2(buttonsStartX, currentY);

                    // �������
                    ImGui::SetCursorScreenPos(buttonPos);
                    bool isClicked = ImGui::InvisibleButton("##button", buttonSize, ImGuiButtonFlags_PressedOnClick);
                    if (isClicked) {
                        clickedIndex = i;
                    }

                    // ���¹��ɽ���
                    const float targetProgress = states[i] ? 1.0f : 0.0f;
                    instance.buttonStates[i].transitionProgress = ImLerp(
                        instance.buttonStates[i].transitionProgress,
                        targetProgress,
                        ImGui::GetIO().DeltaTime * instance.transitionSpeed
                    );
                    const float progress = instance.SmoothStep(instance.buttonStates[i].transitionProgress);

                    // ��ͣЧ��
                    const bool isHovered = ImGui::IsItemHovered();
                    const float hoverTarget = isHovered ? 1.0f : 0.0f;
                    instance.buttonStates[i].hoverAlpha = ImLerp(
                        instance.buttonStates[i].hoverAlpha,
                        hoverTarget,
                        ImGui::GetIO().DeltaTime * instance.hoverSpeed
                    );

                    // ���ƻ���������δѡ��->ѡ�й��ɣ�
                    const ImVec4 bgColor = ImLerp(
                        instance.unselectedBgColor,
                        instance.selectedBgColor,
                        progress
                    );
                    drawList->AddRectFilled(
                        buttonPos,
                        ImVec2(buttonPos.x + buttonSize.x, buttonPos.y + buttonSize.y),
                        ImGui::ColorConvertFloat4ToU32(bgColor),
                        instance.buttonRounding
                    );

                    // ѡ��ʱ������β���䣨����selectedBgColor�����밴ť����i��
                    if (progress > 0.1f) { // ���ɽ��ȳ���10%ʱ��ʾ
                        instance.DrawGradientBackground(drawList, buttonPos, i);
                    }

                    // ���Ʊ�������
                    ImVec4 titleColor = ImLerp(
                        instance.unselectedTitleColor,
                        instance.selectedTitleColor,
                        progress
                    );
                    if (instance.buttonStates[i].hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * instance.buttonStates[i].hoverAlpha);
                        titleColor.x = ImSaturate(titleColor.x * hoverFactor);
                        titleColor.y = ImSaturate(titleColor.y * hoverFactor);
                        titleColor.z = ImSaturate(titleColor.z * hoverFactor);
                    }

                    const ImVec2 textSize = ImGui::CalcTextSize(titles[i]);
                    const ImVec2 textPos = ImVec2(
                        buttonPos.x + (buttonSize.x - textSize.x) * 0.5f,
                        buttonPos.y + (buttonSize.y - textSize.y) * 0.5f
                    );
                    drawList->AddText(textPos, ImGui::ColorConvertFloat4ToU32(titleColor), titles[i]);

                    currentY += buttonSize.y + actualButtonSpacing;
                    ImGui::PopID();
                }

                // �������¼�����ѡ�߼���
                if (clickedIndex != -1) {
                    for (int j = 0; j < buttonCount; ++j) {
                        states[j] = (j == clickedIndex);
                    }
                }

                // У��ѡ��״̬
                int selectedCount = 0;
                for (int i = 0; i < buttonCount; ++i) {
                    if (states[i]) selectedCount++;
                }
                if (selectedCount != 1) {
                    for (int i = 0; i < buttonCount; ++i) states[i] = false;
                    states[0] = true;
                    selectedIndex = 0;
                }

                // ���Ƶ�����
                const float targetNavBarY = buttonsStartY + selectedIndex * (buttonSize.y + actualButtonSpacing);
                instance.navBarCurrentY = ImLerp(
                    instance.navBarCurrentY,
                    targetNavBarY,
                    ImGui::GetIO().DeltaTime * instance.navBarSpeed
                );

                const float navBarWidth = 2.0f;
                const float navBarX = buttonsStartX;
                drawList->AddRectFilled(
                    ImVec2(navBarX, instance.navBarCurrentY),
                    ImVec2(navBarX + navBarWidth, instance.navBarCurrentY + buttonSize.y),
                    ImGui::ColorConvertFloat4ToU32(instance.navBarColor),
                    navBarWidth * 0.5f
                );

                // �������λ�����������Ҳ�
                ImGui::SetCursorScreenPos(ImVec2(
                    toolbarStartPos.x + toolbarSize.x + ImGui::GetStyle().ItemSpacing.x,
                    toolbarStartPos.y
                ));

                ImGui::PopID();
            }
        };
    }

    namespace CheckBox
    {
        // ƽ������ѡ����ࣨ�Ż��Թ�����λ�ã�
        // ���ܣ�ʵ�ִ���ѡ���֧��״̬���ɶ�������ͣЧ�����Զ�����ʽ���Թ��ڿ��ھ�ȷ����
        class SmoothCheckbox {
        private:
            // ״̬�붯����ر����Ľṹ��
            struct CheckboxState {
                float transitionProgress = 0.0f; // ״̬���ɽ��ȣ�0.0 = δѡ��, 1.0 = ѡ�У�
                float hoverAlpha = 0.0f;         // ��ͣЧ��͸���ȣ�0.0 = ��Ч��, 1.0 = ���Ч����
            };

            // ��������
            const float transitionSpeed = 8.0f;  // ״̬�л������ٶ�
            const float hoverSpeed = 12.0f;      // ��ͣЧ�������ٶ�

            // ��������
            float SmoothStep(float t) {
                return t * t * (3.0f - 2.0f * t);
            }

        public:
            // ����ѡ��򣨺��ķ�����
            static void Draw(
                const char* title,                // ����(���Ϊ���򲻻��Ʊ���)
                const char* componentId,          // ���ID
                bool* value,                      // ����ֵ
                const ImVec2& size = ImVec2(18, 18), // �����С
                ImColor unselectedBgColor = ImColor(28, 28, 28),  // δѡ��ʱ�ı�����ɫ
                ImColor selectedBgColor = ImColor(205, 163, 173),    // ѡ��ʱ�ı�����ɫ
                ImColor unselectedCheckColor = ImColor(141, 141, 141), // δѡ��ʱ�ĶԹ���ɫ
                ImColor selectedCheckColor = ImColor(37, 37, 37),   // ѡ��ʱ�ĶԹ���ɫ
                ImColor unselectedTitleColor = ImColor(90, 90, 90), // δѡ��ʱ�ı�����ɫ
                ImColor selectedTitleColor = ImColor(255, 255, 255),   // ѡ��ʱ�ı�����ɫ
                ImColor borderColor = ImColor(60, 60, 60),           // �߿���ɫ
                float borderThickness = 1.0f,                         // �߿��ϸ
                float cornerRadius = 3.0f                             // Բ�ǰ뾶������ѡ���ͱ߿��Բ�ǣ�
            ) {
                // ʵ������
                static std::unordered_map<std::string, SmoothCheckbox> instances;
                auto& instance = instances[componentId];

                // ״̬����
                static std::unordered_map<std::string, CheckboxState> states;
                auto& state = states[componentId];

                // ��ʼ����
                ImGui::PushID(componentId);
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();

                // �������������ѡ���+���⣩�ĳߴ�
                ImVec2 totalSize = size;
                if (title && title[0] != '\0') {
                    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    const ImVec2 textSize = ImGui::CalcTextSize(title);
                    totalSize.x = size.x + spacing + textSize.x;
                    totalSize.y = std::max(size.y, textSize.y); // ȡ�߶����ֵ
                }

                // ���������������Ľ���
                ImGui::SetCursorScreenPos(cursorPos);
                bool isClicked = ImGui::InvisibleButton("##checkbox_total_area", totalSize, ImGuiButtonFlags_PressedOnClick);
                if (isClicked) {
                    *value = !(*value);
                }

                // �����������������ͣ״̬������ѡ���ͱ��⣩
                bool isHovered = ImGui::IsItemHovered();

                // ״̬���ɶ���
                const float targetProgress = *value ? 1.0f : 0.0f;
                state.transitionProgress = ImLerp(
                    state.transitionProgress,
                    targetProgress,
                    ImGui::GetIO().DeltaTime * instance.transitionSpeed
                );
                const float progress = instance.SmoothStep(state.transitionProgress);

                // ��ͣ���� - ʹ�������������ͣ״̬
                const float hoverTarget = isHovered ? 1.0f : 0.0f;
                state.hoverAlpha = ImLerp(
                    state.hoverAlpha,
                    hoverTarget,
                    ImGui::GetIO().DeltaTime * instance.hoverSpeed
                );

                // ������ɫ
                const ImVec4 bgColor = ImLerp(
                    unselectedBgColor.Value,
                    selectedBgColor.Value,
                    progress
                );
                const ImVec4 checkColor = ImLerp(
                    unselectedCheckColor.Value,
                    selectedCheckColor.Value,
                    progress
                );

                // ��ͣ����Ч�����߿�
                ImVec4 currentBorderColor = borderColor.Value;
                if (state.hoverAlpha > 0.01f) {
                    const float hoverFactor = 1.0f + (0.5f * state.hoverAlpha);
                    currentBorderColor.x = ImSaturate(currentBorderColor.x * hoverFactor);
                    currentBorderColor.y = ImSaturate(currentBorderColor.y * hoverFactor);
                    currentBorderColor.z = ImSaturate(currentBorderColor.z * hoverFactor);
                }

                // ���Ʊ�����ʹ��Բ�ǲ�����
                drawList->AddRectFilled(
                    cursorPos,
                    ImVec2(cursorPos.x + size.x, cursorPos.y + size.y),
                    ImGui::ColorConvertFloat4ToU32(bgColor),
                    cornerRadius // ʹ���Զ���Բ�ǰ뾶
                );

                // ���Ʊ߿�**�����޸�**��δѡ��ʱ���ƣ�ѡ��ʱ�����ƣ�������Ч����
                if (borderThickness > 0.0f) {
                    // ����߿�ɼ��ȣ�δѡ��ʱ��ȫ�ɼ�(1.0)��ѡ��ʱ��ȫ���ɼ�(0.0)
                    const float borderVisibility = 1.0f - progress;

                    // ֻ�б߿�ɼ��ȴ�����ֵʱ�Ż��ƣ��Ż�����
                    if (borderVisibility > 0.05f) {
                        drawList->AddRect(
                            cursorPos,
                            ImVec2(cursorPos.x + size.x, cursorPos.y + size.y),
                            ImGui::ColorConvertFloat4ToU32(currentBorderColor),
                            cornerRadius, // �뱳������һ�µ�Բ��
                            0,            // �ޱ߽�����
                            borderThickness * borderVisibility // �߿��ϸ��ѡ��״̬����
                        );
                    }
                }

                // ���ƶԹ����Ż�����ȷ���ж�λ��
                if (progress > 0.01f) {
                    // ����ѡ���ȷ���ģ����Ǳ߿��ȣ������Ӿ�ƫ�ƣ�
                    const float centerX = cursorPos.x + size.x * 0.5f;
                    const float centerY = cursorPos.y + size.y * 0.5f;

                    // ����ѡ���ߴ����Թ�������ȷ���ڲ�ͬ��С�¶��������ۣ�
                    const float baseSize = std::min(size.x, size.y); // ȡ����н�С��ֵ��Ϊ��׼
                    const float checkScale = baseSize * 0.40f * progress; // �Թ���СΪѡ����40%

                    // �Թ�����������꣨��ȷ����ȷ�����У�
                    // ��㣨���·���
                    const ImVec2 startPoint = ImVec2(
                        centerX - checkScale * 0.8f,
                        centerY + checkScale * 0.0f
                    );

                    // ת�۵㣨�м䣩
                    const ImVec2 midPoint = ImVec2(
                        centerX - checkScale * 0.4f,
                        centerY + checkScale * 0.55f
                    );

                    // �յ㣨���Ϸ���
                    const ImVec2 endPoint = ImVec2(
                        centerX + checkScale * 0.7f,
                        centerY - checkScale * 0.5f
                    );

                    // ��һ���ߣ�����㵽ת�۵㣩
                    drawList->AddLine(
                        startPoint,
                        midPoint,
                        ImGui::ColorConvertFloat4ToU32(checkColor),
                        2.0f * progress
                    );

                    // �ڶ����ߣ���ת�۵㵽�յ㣩- �ֽ׶ζ���
                    if (progress > 0.5f) {
                        const float adjustedProgress = (progress - 0.5f) * 2.0f;
                        const ImVec2 animatedEnd = ImVec2(
                            midPoint.x + (endPoint.x - midPoint.x) * adjustedProgress,
                            midPoint.y + (endPoint.y - midPoint.y) * adjustedProgress
                        );
                        drawList->AddLine(
                            midPoint,
                            animatedEnd,
                            ImGui::ColorConvertFloat4ToU32(checkColor),
                            2.0f * progress
                        );
                    }

                    // �������ߣ���ת�۵����СԲ�㲹��ȱ��)
                    if (progress > 0.3f) {
                        float dotSize = 0.7f * progress;
                        drawList->AddCircleFilled(
                            midPoint,
                            dotSize,
                            ImGui::ColorConvertFloat4ToU32(checkColor),
                            2.0f * progress
                        );
                    }
                }

                // ���Ʊ��⣨�����Ϊ�գ�
                if (title && title[0] != '\0') {
                    // �������λ�ã���ѡ����Ҳ࣬��ֱ���ж��룩
                    const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImVec2 textPos = cursorPos;
                    textPos.x += size.x + spacing;
                    textPos.y += (size.y - ImGui::GetTextLineHeight()) * 0.5f;

                    // ������ɫ
                    const ImVec4 titleColor = ImLerp(
                        unselectedTitleColor.Value,
                        selectedTitleColor.Value,
                        progress
                    );

                    // ��ͣʱ������������ڻ���ȷ��Ӧ�����������ͣ��
                    ImVec4 currentTitleColor = titleColor;
                    if (state.hoverAlpha > 0.01f) {
                        const float hoverFactor = 1.0f + (0.3f * state.hoverAlpha);
                        currentTitleColor.x = ImSaturate(currentTitleColor.x * hoverFactor);
                        currentTitleColor.y = ImSaturate(currentTitleColor.y * hoverFactor);
                        currentTitleColor.z = ImSaturate(currentTitleColor.z * hoverFactor);
                    }

                    drawList->AddText(
                        textPos,
                        ImGui::ColorConvertFloat4ToU32(currentTitleColor),
                        title
                    );
                }

                // �������λ��
                ImGui::SetCursorScreenPos(ImVec2(
                    cursorPos.x,
                    cursorPos.y + totalSize.y + ImGui::GetStyle().ItemSpacing.y
                ));

                ImGui::PopID();
            }
        };
    }

    namespace Colors
    {
        class SmoothColorPicker {
        private:
            // ��ɫ�洢
            ImColor currentColor;       // ��ǰѡ�е���ɫ
            bool isOpen = false;        // ��ɫ�崰���Ƿ��
            bool isHovered = false;     // Ԥ�������Ƿ���ͣ
            bool isInitialized = false; // ÿ��ʵ�������ĳ�ʼ��״̬

            // ��������
            float hoverAlpha = 0.0f;    // ��ͣЧ��͸����
            float hoverSpeed = 15.0f;   // ��ͣ�����ٶ�

            // ��ʽ����
            ImVec2 previewSize = ImVec2(24, 17.5f);  // Ԥ�������С
            ImVec2 pickerSize = ImVec2(220, 250); // ��ɫ�崰�ڴ�С
            float rounding = 0.0f;                 // Ԥ������Բ�ǣ�Ĭ��0��
            float borderThickness = 1.5f;          // �߿��ȣ��̶�Ϊ1���أ�

            // ����ʵ��
            bool DrawImpl(const char* identifier, ImColor initialColor) {
                ImGui::PushID(identifier);

                // ��ʼ����ɫ��ÿ��ʵ���״�ʹ��ʱ��
                if (!isInitialized) {
                    currentColor = initialColor;
                    isInitialized = true;
                }

                // ����Ԥ���������¼�
                bool clicked = ImGui::InvisibleButton("##preview", previewSize);
                isHovered = ImGui::IsItemHovered();

                // ��ȡԤ������λ�ã�����Ϊ�߿��ڲ࣬����߿򱻽ضϣ�
                ImVec2 previewPos = ImGui::GetItemRectMin();
                previewPos.x += borderThickness;
                previewPos.y += borderThickness;
                ImVec2 previewEndPos = ImVec2(
                    previewPos.x + previewSize.x - 2 * borderThickness,
                    previewPos.y + previewSize.y - 2 * borderThickness
                );

                // �����/�رյ�ɫ��
                if (clicked) {
                    isOpen = !isOpen;
                }

                // ����ɫ�崰���Ƿ��ѹرգ��û�������ⲿ����
                std::string popupId = "##ColorPickerWindow_" + std::string(identifier);
                if (isOpen) {
                    // �����ɫ�崰��λ�úʹ�С
                    ImVec2 popupPos = ImGui::GetItemRectMin(); // ʹ��ԭʼλ�ü��㵯��
                    popupPos.y += previewSize.y + ImGui::GetStyle().ItemSpacing.y;

                    // �������Ƿ��ڵ�ɫ�崰���ڲ�
                    ImVec2 mousePos = ImGui::GetMousePos();
                    bool isMouseInPicker = (mousePos.x >= popupPos.x &&
                        mousePos.x <= popupPos.x + pickerSize.x &&
                        mousePos.y >= popupPos.y &&
                        mousePos.y <= popupPos.y + pickerSize.y);

                    // ����˴����ⲿ�Ҳ���Ԥ������ʱ�ر�
                    bool isClickingOutside = ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
                        !isMouseInPicker &&
                        !ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(),
                            ImVec2(ImGui::GetItemRectMin().x + previewSize.x,
                                ImGui::GetItemRectMin().y + previewSize.y));

                    if (isClickingOutside) {
                        isOpen = false;
                    }
                }

                // ������ͣ����
                float targetAlpha = isHovered ? 0.2f : 0.0f;
                hoverAlpha = ImLerp(hoverAlpha, targetAlpha, ImGui::GetIO().DeltaTime * hoverSpeed);

                // ������ɫԤ������
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                const ImVec2 originalPos = ImGui::GetItemRectMin(); // ԭʼδ������λ��
                const ImVec2 originalEndPos = ImVec2(
                    originalPos.x + previewSize.x,
                    originalPos.y + previewSize.y
                );

                // 1. ���ƺ�ɫ�߿�ʹ��ԭʼλ�ú�Ԥ����С��Բ�Ǹ���previewRounding��
                drawList->AddRect(
                    originalPos,
                    originalEndPos,
                    IM_COL32(0, 0, 0, 255), // ����ɫ�߿�
                    rounding,                // �߿�Բ�Ǹ���previewRounding����
                    0,                       // Բ�Ƿֶ�����0Ϊ�Զ���
                    borderThickness          // �߿���
                );

                // 2. ���ƻ�����ɫ�飨������λ�ã����ⱻ�߿򸲸ǣ�
                drawList->AddRectFilled(
                    previewPos,
                    previewEndPos,
                    currentColor,
                    rounding  // ��ɫ��Բ����߿򱣳�һ��
                );

                // 3. ��ͣЧ������͸����ɫ���ǣ�������λ�ã�
                if (hoverAlpha > 0.01f) {
                    drawList->AddRectFilled(
                        previewPos,
                        previewEndPos,
                        IM_COL32(255, 255, 255, (int)(hoverAlpha * 255)),
                        rounding  // ��ͣЧ��Բ�������屣��һ��
                    );
                }

                // ���Ƶ�ɫ�崰�ڣ��ޱ��⣩
                if (isOpen) {
                    // ���ô���λ�ã�Ԥ�������·���
                    ImVec2 popupPos = ImGui::GetItemRectMin();
                    popupPos.y += previewSize.y + ImGui::GetStyle().ItemSpacing.y;
                    ImGui::SetNextWindowPos(popupPos);

                    // ���ô��ڴ�С
                    ImGui::SetNextWindowSize(pickerSize);
                    if (ImGui::Begin(popupId.c_str(), nullptr,
                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings)) {
                        // ��ɫѡ�������
                        ImVec4 color = currentColor;

                        ImGuiStyle& style = ImGui::GetStyle();
                        ImVec4 originalFrameBg = style.Colors[ImGuiCol_FrameBg];
                        ImVec4 originalFrameBgHovered = style.Colors[ImGuiCol_FrameBgHovered];
                        ImVec4 originalFrameBgActive = style.Colors[ImGuiCol_FrameBgActive];
                        float originalFrameRounding = style.FrameRounding;

                        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);       // ����ɫ����Ұ�͸����
                        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f); // ��ͣ����ɫ
                        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);  // �����ɫ
                        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // �ı���ɫ����ɫ��

                        // ����Բ�ǰ뾶
                        style.FrameRounding = 4.0f;

                        // ��ȡ������������ı߽����
                        ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
                        ImVec2 windowMax = ImGui::GetWindowContentRegionMax();
                        ImVec2 windowSize = ImVec2(windowMax.x - windowMin.x, windowMax.y - windowMin.y);

                        // ������ɫѡ������С�봰����ȫһ��
                        ImGui::SetNextItemWidth(windowSize.x);

                        // ɫ�໷�ͱ��Ͷ�/����ѡ����
                        ImGui::ColorPicker4(
                            "##picker",
                            &color.x,
                            ImGuiColorEditFlags_NoSidePreview |
                            ImGuiColorEditFlags_NoSmallPreview |
                            ImGuiColorEditFlags_NoLabel |
                            ImGuiColorEditFlags_AlphaBar
                        );

                        // �ָ�ԭʼ��ʽ
                        style.Colors[ImGuiCol_FrameBg] = originalFrameBg;
                        style.Colors[ImGuiCol_FrameBgHovered] = originalFrameBgHovered;
                        style.Colors[ImGuiCol_FrameBgActive] = originalFrameBgActive;
                        style.FrameRounding = originalFrameRounding;

                        // ������ɫ
                        currentColor = color;

                        ImGui::End();
                    }
                }

                ImGui::PopID();
                return clicked;
            }

        public:
            // ��̬�ӿ�
            static bool Draw(
                const char* identifier,      // Ψһ��ʶ��
                ImColor* color,              // ��ɫ���ָ��
                float previewRounding = 0.0f, // Ԥ������Բ�ǣ�Ĭ��0
                const ImVec2& previewSize = ImVec2(24, 17.5f),  // Ԥ�������С
                const ImVec2& pickerSize = ImVec2(220, 250) // ��ɫ�崰�ڴ�С                  
            ) {
                // ����ʵ��
                std::string key = identifier;
                static std::unordered_map<std::string, SmoothColorPicker> instances;
                auto& instance = instances[key];

                // ��ʼ����ʽ���״�ʹ��ʱ��
                static std::unordered_set<std::string> initialized;
                if (initialized.find(key) == initialized.end()) {
                    instance.previewSize = previewSize;
                    instance.pickerSize = pickerSize;
                    instance.rounding = previewRounding;  // ��ʼ��Բ�ǲ������߿��ʹ�ø�ֵ��
                    initialized.insert(key);
                }

                // ���Ʋ�ͬ����ɫ
                bool interacted = instance.DrawImpl(identifier, *color);
                *color = instance.currentColor;  // ͬ����ɫ���ⲿ
                return interacted;
            }
        };
    }

    namespace Slider 
    {
        class SmoothSliderfloat {
        private:
            // ����ֵ
            float currentValue = 0.0f;
            float displayValue = 0.0f;
            bool isDragging = false;
            bool isHovered = false;
            bool hasFocus = false;
            float fadeAlpha = 1.0f;
            float focusLostTime = 0.0f;
            const float fadeDelay = 3.0f;

            // ��������
            float animationSpeed = 10.0f;
            float hoverIntensity = 0.0f;
            float hoverAlpha = 0.0f;
            bool isActive = false;

            // ��ʽ����
            ImVec2 size = ImVec2(200, 20);
            float thumbWidth = 12.0f;
            float thumbHeight = 20.0f;
            float rounding = 4.0f;
            float thumbRounding = 4.0f;
            float textOffsetX = 20.0f;
            float textOffsetY = 1.0f;

            // ��ɫ����
            ImU32 bgColor = IM_COL32(70, 70, 70, 255);
            ImU32 fillColor = IM_COL32(255, 182, 193, 255);
            ImU32 thumbColor = IM_COL32(220, 220, 220, 255);
            ImU32 hoverColor = IM_COL32(255, 255, 255, 50);
            ImU32 textColor = IM_COL32(255, 255, 255, 255);
            ImU32 titleColor = IM_COL32(220, 220, 220, 255);

            // ��������
            float EaseOut(float t) {
                return 1.0f - (1.0f - t) * (1.0f - t);
            }

            // ����ʵ��
            float DrawImpl(const char* label, const char* identifier, float min, float max, float value) {
                ImGui::PushID(identifier);

                // ���µ�ǰֵ
                currentValue = value;

                // ���㲼��
                ImGui::BeginGroup();

                // ���Ʊ�ǩ������У� - �̶��ڻ������Ϸ�
                bool hasLabel = label && label[0] != '\0';
                if (hasLabel) {
                    ImGui::TextColored(ImGui::ColorConvertU32ToFloat4(titleColor), "%s", label);
                }

                // ����������
                ImVec2 p = ImGui::GetCursorScreenPos();
                ImGui::InvisibleButton("##slider", size);

                // ��������
                isActive = ImGui::IsItemActive();
                isHovered = ImGui::IsItemHovered();
                bool wasHasFocus = hasFocus;
                hasFocus = isActive || isHovered;

                ImVec2 thumbMin = ImVec2(p.x + (size.x - thumbWidth) * ((value - min) / (max - min)), p.y - (thumbHeight - size.y) / 2);
                ImVec2 thumbMax = ImVec2(thumbMin.x + thumbWidth, thumbMin.y + thumbHeight);

                bool isThumbHovered = ImGui::IsMouseHoveringRect(thumbMin, thumbMax);

                if ((isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) || (isThumbHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))) {
                    float mouseX = ImGui::GetMousePos().x - p.x;
                    currentValue = (mouseX / size.x) * (max - min) + min;
                    currentValue = ImClamp(currentValue, min, max);
                    isDragging = true;
                }
                else if (isDragging && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    isDragging = false;
                }

                // ���¶���
                float delta = ImGui::GetIO().DeltaTime;
                displayValue = ImLerp(displayValue, currentValue, delta * animationSpeed);

                // ������ͣЧ��
                float hoverTarget = isHovered || isActive ? 1.0f : 0.0f;
                hoverAlpha = ImLerp(hoverAlpha, hoverTarget, delta * 15.0f);

                // ������Ʋ���
                float fillRatio = (displayValue - min) / (max - min);
                thumbMin = ImVec2(p.x + fillRatio * (size.x - thumbWidth), p.y - (thumbHeight - size.y) / 2);
                thumbMax = ImVec2(thumbMin.x + thumbWidth, thumbMin.y + thumbHeight);

                // ���Ʊ���
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // ����
                drawList->AddRectFilled(
                    p,
                    ImVec2(p.x + size.x, p.y + size.y),
                    bgColor,
                    rounding
                );

                // ��䲿��
                drawList->AddRectFilled(
                    p,
                    ImVec2(p.x + fillRatio * size.x, p.y + size.y),
                    fillColor,
                    rounding
                );

                // ��ͣЧ��
                if (hoverAlpha > 0.01f) {
                    ImVec4 hoverCol = ImGui::ColorConvertU32ToFloat4(hoverColor);
                    hoverCol.w *= hoverAlpha;
                    drawList->AddRectFilled(
                        p,
                        ImVec2(p.x + size.x, p.y + size.y),
                        ImGui::GetColorU32(hoverCol),
                        rounding
                    );
                }

                // ���飨Ĵָ��
                drawList->AddRectFilled(
                    thumbMin,
                    thumbMax,
                    thumbColor,
                    thumbRounding,
                    ImDrawFlags_RoundCornersAll
                );

                ImGui::EndGroup();
                ImGui::PopID();
                return currentValue;
            }

        public:
            // ��̬�ӿ�
            static float Draw(
                const char* label,                            // �������ı�ǩ�ı�����ʾ�ڻ������Ա߻򻬶�����
                const char* identifier,                       // ��������Ψһ��ʶ�����������ֲ�ͬ�Ļ�����
                float* value,                                 // ָ�򻬶�����ǰֵ��ָ��
                float min,                                    // ����������Сֵ
                float max,                                    // �����������ֵ
                const ImVec2& size = ImVec2(260, 8),          // �������Ĵ�С
                float thumbWidth = 7.0f,                      // ����Ŀ��
                float thumbHeight = 13.0f,                    // ����ĸ߶�
                float thumbRounding = 20.0f,                  // �����Բ�ǰ뾶
                ImU32 bgCol = IM_COL32(29, 29, 29, 255),      // ������������ɫ
                ImU32 fillCol = IM_COL32(205, 163, 173, 255),  // �����������ɫ
                ImU32 thumbCol = IM_COL32(255, 255, 255, 255),// ������ɫ
                ImU32 textCol = IM_COL32(255, 255, 255, 255), // �ı���ɫ
                ImU32 titleCol = IM_COL32(220, 220, 220, 255) // ������ɫ
            ) {

                std::string key = identifier ? identifier : "";
                static std::unordered_map<std::string, SmoothSliderfloat> instances;
                auto& instance = instances[key];

                static std::unordered_set<std::string> initialized;
                if (initialized.find(key) == initialized.end()) {
                    instance.size = size;
                    instance.thumbWidth = thumbWidth;
                    instance.thumbHeight = thumbHeight;
                    instance.thumbRounding = thumbRounding;
                    instance.bgColor = bgCol;
                    instance.fillColor = fillCol;
                    instance.thumbColor = thumbCol;
                    instance.textColor = textCol;
                    instance.titleColor = titleCol;
                    initialized.insert(key);
                }

                float newValue = instance.DrawImpl(label, identifier, min, max, *value);
                if (newValue != *value) {
                    *value = newValue;
                }

                return newValue;
            }
        };
    }

    namespace Input
    {
        class SmoothInput {
        private:
            // ״̬����
            float hoverAlpha = 0.0f;
            float focusAlpha = 0.0f;
            bool hasFocus = false;

            // ��ʽ����
            ImVec2 size = ImVec2(200, 30);
            float rounding = 4.0f;
            float borderWidth = 1.0f;
            bool centerText = false;

            // ��ɫ����
            ImU32 bgColor = IM_COL32(40, 40, 40, 255);
            ImU32 borderColor = IM_COL32(100, 100, 100, 255);
            ImU32 hoverColor = IM_COL32(255, 255, 255, 30);
            ImU32 focusColor = IM_COL32(100, 150, 255, 150);

            // ���Ļ���ʵ��
            bool DrawImpl(const char* label, char* buf, size_t buf_size) {
                std::string thisId = label ? std::string(label) : "##input";
                ImGui::PushID(thisId.c_str());

                // 1. ��ȡλ�úͳߴ�
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImDrawList* drawList = ImGui::GetWindowDrawList();

                // 2. ������������
                ImGui::InvisibleButton("##input_area", size);
                bool hovered = ImGui::IsItemHovered();
                bool clicked = ImGui::IsItemClicked();

                // 3. �������
                if (clicked) {
                    hasFocus = true;
                }
                if (hasFocus && !ImGui::IsItemActive() && ImGui::IsMouseClicked(0)) {
                    hasFocus = false;
                }
                bool isActive = hasFocus;

                // 4. ���¶���״̬
                float delta = ImGui::GetIO().DeltaTime;
                hoverAlpha = ImLerp(hoverAlpha, hovered ? 1.0f : 0.0f, delta * 10.0f);
                focusAlpha = ImLerp(focusAlpha, isActive ? 1.0f : 0.0f, delta * 15.0f);

                // 5. ���Ʊ���
                drawList->AddRectFilled(
                    pos,
                    ImVec2(pos.x + size.x, pos.y + size.y),
                    bgColor,
                    rounding
                );

                // 6. ������ͣЧ��
                if (hoverAlpha > 0.01f) {
                    drawList->AddRectFilled(
                        pos,
                        ImVec2(pos.x + size.x, pos.y + size.y),
                        IM_COL32(255, 255, 255, (int)(30 * hoverAlpha)),
                        rounding
                    );
                }

                // 7. ׼���������ʽ - �ؼ��޸ģ���ȫ����Ĭ�ϱ߿�ͱ���
                ImGui::SetCursorScreenPos(pos);
                float verticalPadding = (size.y - ImGui::GetTextLineHeight()) * 0.5f;

                // ���浱ǰ��ʽ
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, verticalPadding));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, rounding);
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f); // �ؼ�������Ĭ�ϱ߿�
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                // �ؼ��������������ɫ����Ϊ͸��������Ĭ�ϻ���
                ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(0, 0, 0, 0)); // �߿���ɫ��Ϊ͸��
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

                // 8. �����������
                ImGui::PushItemWidth(size.x);

                if (isActive) {
                    ImGui::SetKeyboardFocusHere();
                }

                // 9. �ı����д���
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                const ImGuiStyle& style = ImGui::GetStyle();
                float originalCursorPosX = window->DC.CursorPos.x;

                if (centerText && buf != nullptr && buf[0] != '\0') {
                    float textWidth = ImGui::CalcTextSize(buf).x;
                    float inputWidth = size.x - (style.FramePadding.x * 2);

                    if (textWidth < inputWidth) {
                        float offsetX = (inputWidth - textWidth) * 0.5f;
                        window->DC.CursorPos.x += offsetX;
                    }
                }

                // 10. ��������򣨴�ʱ�������ı����룬�����Ʊ߿�
                bool changed = ImGui::InputText(
                    "##real_input",
                    buf,
                    buf_size,
                    ImGuiInputTextFlags_None
                );

                // �ָ����λ��
                window->DC.CursorPos.x = originalCursorPosX;

                // 11. ���������Լ��ı߿�ͽ���Ч��
                // ���������߿�
                drawList->AddRect(
                    ImVec2(pos.x + 0.5f, pos.y + 0.5f),  // ��΢ƫ�Ʊ��⿹��ݵ��µİ�ɫ��
                    ImVec2(pos.x + size.x - 0.5f, pos.y + size.y - 0.5f),
                    borderColor,
                    rounding,
                    0,
                    borderWidth
                );

                // ���ƽ���߿�
                if (focusAlpha > 0.01f) {
                    float focusBorderWidth = borderWidth * (1.0f + focusAlpha * 0.5f);
                    drawList->AddRect(
                        ImVec2(pos.x - focusBorderWidth * 0.5f, pos.y - focusBorderWidth * 0.5f),
                        ImVec2(pos.x + size.x + focusBorderWidth * 0.5f, pos.y + size.y + focusBorderWidth * 0.5f),
                        focusColor,
                        rounding,
                        0,
                        focusBorderWidth
                    );
                }

                // 12. �ָ���ʽ
                ImGui::PopItemWidth();
                ImGui::PopStyleColor(5);  // �ָ�5����ɫ����
                ImGui::PopStyleVar(4);    // �ָ�4����ʽ����
                ImGui::PopID();

                return changed;
            }

        public:
            static bool Draw(
                const char* label,
                char* buf,
                size_t buf_size,
                bool centerText = true,
                const ImVec2& size = ImVec2(28, 15),
                float rounding = 2.0f,
                float borderWidth = 1.0f,
                ImU32 bgCol = IM_COL32(28, 28, 28, 255),
                ImU32 borderCol = IM_COL32(60, 60, 60, 255),
                ImU32 hoverCol = IM_COL32(255, 255, 255, 30),
                ImU32 focusCol = IM_COL32(205, 163, 173, 255)
            ) {
                static std::unordered_map<std::string, SmoothInput> instances;
                std::string id = label ? std::string(label) : "##input";

                SmoothInput& instance = instances[id];
                instance.size = size;
                instance.rounding = rounding;
                instance.borderWidth = borderWidth;
                instance.bgColor = bgCol;
                instance.borderColor = borderCol;
                instance.hoverColor = hoverCol;
                instance.focusColor = focusCol;
                instance.centerText = centerText;

                return instance.DrawImpl(label, buf, buf_size);
            }
        };
    }

    namespace Button
    {
        class SmoothTextButton {
        private:
            // ״̬����
            bool isHovered = false;        // �����ͣ״̬
            bool isClicked = false;        // ����������
            bool wasActive = false;        // ��һ֡�ļ���״̬
            float hoverProgress = 0.0f;    // ��ͣ��������(0-1)
            float clickProgress = 0.0f;    // �����������(0-1)
            float resetProgress = 0.0f;    // ���ö�������(0-1)

            // ������������΢��Ч����
            const float hoverSpeed = 5.0f;    // ��ͣ�����ٶ�
            const float clickSpeed = 5.0f;    // ��������ٶ�
            const float resetSpeed = 5.0f;    // ���ö����ٶ�
            const float hoverBrightness = 0.25f; // ��ͣ��������
            const float clickBrightness = -0.15f; // ������ȱ仯
            const float resetBrightness = -0.2f;  // ���ð��ȱ仯

            // �������ߣ����뻺����
            float EaseInOut(float t) {
                return t < 0.5f ? 4.0f * t * t * t : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
            }

            // �������Ƿ���ָ�������ڣ�ʹ�ø����ݵ��ַ����Ƚϣ�
            bool IsMouseInWindow(const char* windowName) {
                if (!windowName) return false;

                ImGuiContext& g = *GImGui;
                for (ImGuiWindow* window : g.Windows) {
                    // ��鴰���Ƿ�����ҿɼ���ʹ��strcmp�����ַ����Ƚϣ�
                    if (window->Active && !window->Hidden &&
                        strcmp(window->Name, windowName) == 0) {
                        // �������Ƿ��ڴ��ھ�����
                        return ImGui::IsMouseHoveringRect(window->Pos, ImVec2(window->Pos.x + window->Size.x, window->Pos.y + window->Size.y));
                        
                    }
                }
                return false;
            }

            // �������Ƿ����κε��������ڣ�������IsMouseHoveringAnyWindow��
            bool IsMouseInPopup() {
                ImGuiContext& g = *GImGui;
                if (g.OpenPopupStack.Size == 0) return false;

                // �������Ƿ����κε�������������
                for (int i = 0; i < g.OpenPopupStack.Size; i++) {
                    ImGuiWindow* popupWindow = g.OpenPopupStack[i].Window;
                    if (popupWindow && popupWindow->Active && !popupWindow->Hidden) {
                        if (ImGui::IsMouseHoveringRect(popupWindow->Pos, ImVec2(popupWindow->Pos.x + popupWindow->Size.x, popupWindow->Pos.y + popupWindow->Size.y))) {
                            return true;
                        }
                    }
                }
                return false;
            }

            // ����ʵ��
            void DrawImpl(const char* label, const char* id, bool* isPressed,
                ImColor normalColor, ImColor selectedColor, const char* excludeWindowName) {
                ImGui::PushID(id);

                // 1. �����������ã����ΰ�ť��
                ImVec2 textSize = ImGui::CalcTextSize(label);
                ImVec2 clickArea(textSize.x + 6, textSize.y + 4); // �Դ����ı��ĵ����

                // ��ȫ͸���Ľ������ף������ڼ�����룩
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0, 0, 0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                bool clicked = ImGui::Button("##", clickArea);
                isHovered = ImGui::IsItemHovered();

                ImGui::PopStyleVar(2);
                ImGui::PopStyleColor(3);

                // 2. ���״̬�����붯������
                if (clicked) {
                    *isPressed = !*isPressed;
                    isClicked = true;    // �����������
                    clickProgress = 0.0f; // ���õ������
                    resetProgress = 0.0f; // �������ö���
                }

                // 3. ����ⲿ�����Զ�ȡ��ѡ�У��Ľ����߼���
                bool isMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
                bool isMouseInArea = ImGui::IsMouseHoveringRect(
                    ImGui::GetItemRectMin(),
                    ImGui::GetItemRectMax()
                );

                // ����Ƿ������ų�����
                bool isInExcludeWindow = IsMouseInWindow(excludeWindowName);

                // ����Ƿ����˵�������
                bool isInPopup = IsMouseInPopup();

                // ȡ��ѡ��������������ⲿ���Ҳ����ų������ڣ��Ҳ��ڵ���������
                if (*isPressed && isMouseClicked && !isMouseInArea &&
                    !isInExcludeWindow && !isInPopup) {
                    *isPressed = false;
                    resetProgress = 0.0f; // �������ö���
                }

                // 4. �������ȸ��£��������㣩
                const float dt = ImGui::GetIO().DeltaTime;

                // ��ͣ����������Ŀ��ֵ��0��1��
                float targetHover = isHovered ? 1.0f : 0.0f;
                hoverProgress = ImLerp(hoverProgress, targetHover, dt * hoverSpeed);

                // ����������������壨��0��1��0��
                if (isClicked) {
                    clickProgress += dt * clickSpeed;
                    if (clickProgress >= 1.0f) {
                        isClicked = false;
                    }
                }

                // ���ö�������0��1�����ڵ���ⲿȡ��ѡ��ʱ�Ĺ���Ч����
                if (resetProgress > 0.0f || (!*isPressed && wasActive)) {
                    resetProgress += dt * resetSpeed;
                    resetProgress = ImMin(resetProgress, 1.0f);
                }

                // ��¼��ǰ״̬������һ֡�Ա�
                wasActive = *isPressed;

                // 5. ��ɫ���㣨�������ֶ���Ч����
                ImVec4 base = *isPressed ? selectedColor : normalColor;

                // ��ͣЧ��������������Ӧ�û�����
                float easedHover = EaseInOut(hoverProgress);
                ImVec4 hoverColor = base;
                hoverColor.x = ImSaturate(base.x + hoverBrightness * easedHover);
                hoverColor.y = ImSaturate(base.y + hoverBrightness * easedHover);
                hoverColor.z = ImSaturate(base.z + hoverBrightness * easedHover);

                // ���Ч����˲�䰵���ٻָ���ʹ����������ģ�����壩
                ImVec4 clickColor = hoverColor;
                if (isClicked) {
                    float clickPulse = sinf(clickProgress * IM_PI); // 0��1��0����������
                    clickColor.x = ImSaturate(hoverColor.x + clickBrightness * clickPulse);
                    clickColor.y = ImSaturate(hoverColor.y + clickBrightness * clickPulse);
                    clickColor.z = ImSaturate(hoverColor.z + clickBrightness * clickPulse);
                }

                // ����Ч����ȡ��ѡ��ʱ�Ĺ��ɶ���
                ImVec4 finalColor = clickColor;
                if (resetProgress > 0.0f && resetProgress < 1.0f) {
                    float resetPulse = sinf(resetProgress * IM_PI * 0.5f); // 0��1����������
                    finalColor.x = ImSaturate(clickColor.x + resetBrightness * resetPulse);
                    finalColor.y = ImSaturate(clickColor.y + resetBrightness * resetPulse);
                    finalColor.z = ImSaturate(clickColor.z + resetBrightness * resetPulse);
                }

                // 6. �����ı���Ψһ�Ӿ�Ԫ�أ�
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 textPos = ImGui::GetItemRectMin();
                // �ı��ڵ�����ھ���
                textPos.x += (clickArea.x - textSize.x) * 0.5f;
                textPos.y += (clickArea.y - textSize.y) * 0.5f;

                drawList->AddText(
                    ImGui::GetFont(),
                    ImGui::GetFontSize(),
                    textPos,
                    ImColor(finalColor),
                    label
                );

                ImGui::PopID();
            }

        public:
            // ����excludeWindowName������ָ������ô��ڲ��رհ�ť״̬
            static void Draw(const char* label, const char* id, bool* isPressed,
                const char* excludeWindowName = nullptr,
                ImColor normalColor = ImColor(141, 141, 141),
                ImColor selectedColor = ImColor(205, 163, 173)
                ) {
                static std::unordered_map<std::string, SmoothTextButton> instances;
                instances[id].DrawImpl(label, id, isPressed, normalColor, selectedColor, excludeWindowName);
            }
        };
    }
}
