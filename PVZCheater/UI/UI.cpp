#include "UI.h"

void Menu()
{
	SYSTEMTIME st;
	GetLocalTime(&st);
    ImGui::SetCursorPos(ImVec2(YEAH::window_width/2-25,30));
    // �ڲ˵���֮����ӷָ���
	ImGui::Text(u8"�� ��");
	ImGui::Text(u8"1.��������ṩ�������Ųο�,���Ǵ��м��ĸ���.");
	ImGui::Text(u8"2.���������ѧϰ����ʹ��,��ֹ�����κ���ҵ��;.");
	ImGui::LabelText("   YEAH.s", u8"����");
	ImGui::LabelText("   2025.8.9", u8"�汾");
	ImGui::LabelText("   PVZ", "GAME");
    ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_ChildBg, 0x16417C);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
	ImGui::BeginChild("UI_1", ImVec2(YEAH::window_width / 2-20, 285), true);
	if (YEAH::UI_1::��̨����) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"��̨����", &YEAH::UI_1::��̨����);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"��̨����", &YEAH::UI_1::��̨����);

	if (YEAH::UI_1::��������) { 
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
		ImGui::Checkbox(u8"����", &YEAH::UI_1::��������);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"��������",&YEAH::UI_1::��������);

	if (YEAH::UI_1::ֲ����CD) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"ֲ����CD", &YEAH::UI_1::ֲ����CD);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"ֲ����CD", &YEAH::UI_1::ֲ����CD);

	if (YEAH::UI_1::ֲ�ﲻ��) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"�޵еļ��(δ)", &YEAH::UI_1::ֲ�ﲻ��);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"�޵еļ��(δ)", &YEAH::UI_1::ֲ�ﲻ��);

	if (YEAH::UI_1::��ɱ��ʬ) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"��ɱ�޷���ʬ", &YEAH::UI_1::��ɱ��ʬ);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"��ɱ�޷���ʬ", &YEAH::UI_1::��ɱ��ʬ);

	if (YEAH::UI_1::��ʬ����) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
		ImGui::Checkbox(u8"������Ѫ��ͷ��ʬ", &YEAH::UI_1::��ʬ����);
		ImGui::PopStyleColor();
	}
	else ImGui::Checkbox(u8"������Ѫ��ͷ��ʬ", &YEAH::UI_1::��ʬ����);


	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();



	ImGui::SameLine();



	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
	ImGui::BeginChild("UI_2", ImVec2(YEAH::window_width / 2-20, 285), true);
	// ����ԭʼ�İ�ť��ɫ
	ImGuiStyle* my_style_UI_2 = &ImGui::GetStyle();
	ImVec4 originalButtonColor = my_style_UI_2->Colors[ImGuiCol_Button];
	ImVec4 originalButtonActiveColor = my_style_UI_2->Colors[ImGuiCol_ButtonActive];
	// �����ť����������ð�ť��ɫΪ���ɫ
	if (YEAH::UI_2::��ʼ��) {
		ImGui::PushStyleColor(ImGuiCol_Button, originalButtonActiveColor);
		ImGui::Button(u8"��ʼ��", ImVec2(YEAH::window_width / 2 - 40, 50));
		ImGui::PopStyleColor();
	}
	else if (ImGui::Button(u8"��ʼ��", ImVec2(YEAH::window_width / 2 - 40, 50)))
	{
		YEAH::UI_2::��ʼ�� = true;
	}
	ImGui::ProgressBar(YEAH::UI_2::�����ʼ����, ImVec2(-1, 3));
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("PID: %d", (int)YEAH::UI_2::PID);
	ImGui::Text("Address: 0x%08X",YEAH::UI_2::address);
	ImGui::Text("TIME: %d-%02d-%02d %02d:%02d",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute);
	
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::LabelText("       3509401748", u8"�������");
}