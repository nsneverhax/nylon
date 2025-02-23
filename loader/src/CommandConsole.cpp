#include <Nylon/CommandConsole.hpp>
#include "Main.hpp"

#include "Imgui/ImGui.hpp"

extern nylon::CommandConsole nylon::Console = { };

//void in::ImGuiConsole::DrawLogWindow()
//{
//	float footerHeightReserved = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
//	if (ImGui::BeginChild("ScrollRegion", ImVec2(0, footerHeightReserved), false, 0))
//	{
//		ImGui::PushTextWrapPos();
//
//
//
//		ImGui::PopTextWrapPos();
//		ImGui::EndChild();
//	}
//}

bool HelpCommandCallback(nylon::CommandConsole& caller, std::vector<std::string> arguments)
{
	return true;
}

bool ColonThreeCallback(nylon::CommandConsole& caller, std::vector<std::string> arguments)
{
	caller.PushHistory(":3");

	return true;
}

int TextInputCallback_CallbackHistory(nylon::CommandConsole* console, ImGuiInputTextCallbackData* data)
{
	return 0;
}
int TextInputCallback_CallbackCompletion(nylon::CommandConsole* console, ImGuiInputTextCallbackData* data)
{
	return 0;
}
int TextInputCallback_EnterReturnsTrue(nylon::CommandConsole* console, ImGuiInputTextCallbackData* data)
{
	return 0;
}

int TextInputCallback(ImGuiInputTextCallbackData* data)
{
	nylon::CommandConsole* console = reinterpret_cast<nylon::CommandConsole*>(data->UserData);
	switch (data->EventFlag)
	{
	case ImGuiInputTextFlags_CallbackHistory:
		return TextInputCallback_CallbackHistory(console, data);
	case ImGuiInputTextFlags_CallbackCompletion:
		return TextInputCallback_CallbackCompletion(console, data);
	case ImGuiInputTextFlags_EnterReturnsTrue:
		return TextInputCallback_EnterReturnsTrue(console, data);
	default:
		break;
	}
	return 0;
}
void nylon::CommandConsole::PushHistory(std::string_view text)
{
	m_history.insert(m_history.begin(), std::string(text));

	if (m_history.size() >= CommandConsole::DefaultHistorySize)
		m_history.pop_back();

	m_shouldScroll = true;
}

void nylon::CommandConsole::DrawLog()
{
	const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

	ImGui::PushTextWrapPos();

	if (ImGui::BeginChild("##ScrollRegion", ImVec2(0, -footerHeightToReserve), false, 0))
	{
		if (!m_history.empty())
		{

			for (auto it = m_history.rbegin(); it != m_history.rend(); it++)
			{
				ImGui::Text(it->c_str());
			}
		}

		if (m_shouldScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		m_shouldScroll = false;

		ImGui::EndChild();
	}

	ImGui::PopTextWrapPos();
}

nylon::CommandConsole::CommandConsole()
{
	ZeroMemory(m_inputBuffer, sizeof(m_inputBuffer));
	m_inputHistory.reserve(CommandConsole::DefaultHistorySize);
	m_history.reserve(CommandConsole::DefaultHistorySize);

	RegisterCommand(ConsoleCommand("help", "Displays help for a given command.", nullptr));
	RegisterCommand(ConsoleCommand(":3", ":3", ColonThreeCallback));
}

bool nylon::CommandConsole::RegisterCommand(const ConsoleCommand& command)
{
	if (m_commands.contains(command.Name))
		return false;

	m_commands.insert({ std::string(command.Name), ConsoleCommand(command) });

	return true;
}
const char* nylon::CommandConsole::GetInputBuffer() const
{
	return m_inputBuffer;
}

void nylon::CommandConsole::SetHistorySize(uint32_t size)
{
	//m_history.resize(size);
}
uint32_t nylon::CommandConsole::GetHistorySize() const
{
	return m_history.size();
}
const nylon::ConsoleHistory& nylon::CommandConsole::GetHistory() const
{
	return m_history;
}
void nylon::CommandConsole::ClearHistory()
{
	m_history.clear();
}

void nylon::CommandConsole::Draw()
{
	if (!ImGui::Begin("Nylon Console"))
	{
		ImGui::End();
		return;
	}

	DrawLog();

	ImGui::Separator();

	ImGuiInputTextFlags inputTextFlags =
		ImGuiInputTextFlags_CallbackHistory  | ImGuiInputTextFlags_CallbackCompletion |
		ImGuiInputTextFlags_EnterReturnsTrue;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2));

	bool reclaimFocus = false;

	if (ImGui::InputText("##Input", m_inputBuffer, sizeof(m_inputBuffer) - 1, inputTextFlags, TextInputCallback, this))
	{
		if (m_inputBuffer[0] != '\0')
		{
			PushHistory(std::format("] {}", m_inputBuffer));


			m_shouldScroll = true;

			reclaimFocus = true;

			if (!m_commands.contains(m_inputBuffer))
				PushHistory(std::format("Unrecognized Command: '{}'", m_inputBuffer));
			else
			{
				ConsoleCommandCallback callback = m_commands[m_inputBuffer].Callback;

				if (callback == nullptr)
					PushHistory(std::format("Command: '{}' had a null callback!", m_inputBuffer));
				else
				{
					callback(*this, std::vector<std::string>());
				}
			}
		}
		ZeroMemory(m_inputBuffer, sizeof(m_inputBuffer));
	}

	ImGui::PopItemWidth();

	ImGui::SetItemDefaultFocus();
	if (reclaimFocus)
		ImGui::SetKeyboardFocusHere(-1);

	ImGui::End();
}