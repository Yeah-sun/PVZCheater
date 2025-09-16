#include "UI.h"

void Menu()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
    ImGui::SetCursorPos(ImVec2(YEAH::window_width/2-25,30));
    // 在菜单项之间添加分割线
	ImGui::Text(u8"公 告");
	ImGui::Text(u8"1.本软件仅提供新手入门参考,并非大佬级的辅助.");
	ImGui::Text(u8"2.本软件仅供学习交流使用,禁止用于任何商业用途.");
	ImGui::LabelText("   YEAH.s", u8"作者");
	ImGui::LabelText("   2025.8.9", u8"版本");
	ImGui::LabelText("   PVZ", "GAME");
    ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_ChildBg, 0x16417C);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
	ImGui::BeginChild("UI_1", ImVec2(YEAH::window_width / 2-20, 285), true);
	if (YEAH::UI_1::后台运行) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"后台运行", &YEAH::UI_1::后台运行);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"后台运行", &YEAH::UI_1::后台运行);

	if (YEAH::UI_1::无限阳光) { 
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
		ImGui::Checkbox(u8"将军", &YEAH::UI_1::无限阳光);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"无限阳光",&YEAH::UI_1::无限阳光);

	if (YEAH::UI_1::植物无CD) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"植物无CD", &YEAH::UI_1::植物无CD);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"植物无CD", &YEAH::UI_1::植物无CD);

	if (YEAH::UI_1::植物不死) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"无敌的坚果(未)", &YEAH::UI_1::植物不死);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"无敌的坚果(未)", &YEAH::UI_1::植物不死);

	if (YEAH::UI_1::秒杀僵尸) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"秒杀无防僵尸", &YEAH::UI_1::秒杀僵尸);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"秒杀无防僵尸", &YEAH::UI_1::秒杀僵尸);

	if (YEAH::UI_1::僵尸不死) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"超标锁血无头僵尸", &YEAH::UI_1::僵尸不死);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"超标锁血无头僵尸", &YEAH::UI_1::僵尸不死);


	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();



	ImGui::SameLine();



	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
	ImGui::BeginChild("UI_2", ImVec2(YEAH::window_width / 2-20, 285), true);
	// 保存原始的按钮颜色
	ImGuiStyle* my_style_UI_2 = &ImGui::GetStyle();
	ImVec4 originalButtonColor = my_style_UI_2->Colors[ImGuiCol_Button];
	ImVec4 originalButtonActiveColor = my_style_UI_2->Colors[ImGuiCol_ButtonActive];
	// 如果按钮被点击，设置按钮颜色为活动颜色
	if (YEAH::UI_2::初始化) {
		ImGui::PushStyleColor(ImGuiCol_Button, originalButtonActiveColor);
		ImGui::Button(u8"初始化", ImVec2(YEAH::window_width / 2 - 40, 50));
		ImGui::PopStyleColor();
	}
	else if (ImGui::Button(u8"初始化", ImVec2(YEAH::window_width / 2 - 40, 50)))
	{
		YEAH::UI_2::初始化 = true;
	}
	ImGui::ProgressBar(YEAH::UI_2::程序初始进度, ImVec2(-1, 3));
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("PID: %d", (int)YEAH::UI_2::PID);
	ImGui::Text("Address: 0x%08X",YEAH::UI_2::address);
	ImGui::Text("TIME: %d-%02d-%02d %02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::LabelText("       3509401748", u8"作者企鹅");
}