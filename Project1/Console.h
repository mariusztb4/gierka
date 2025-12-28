#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Logger.h"
#include "Command.h"

class Shader;
class Text;
struct GLFWwindow;

struct LogEntry
{
    MessageType type;
    std::string text;
};

enum class LogFilter
{
    All,
    Info,
    Warning,
    Error
};

class ConsoleRenderer {
public:
	ConsoleRenderer() {}
    ~ConsoleRenderer() {}
    ConsoleRenderer(GLFWwindow* window, int screenWidth, int screenHeight);
    void Render(Text& text);
    void Update(float dt);
    void Toggle();

    void AddLog(const std::string& text, MessageType type);
    void SetScreenSize(int w, int h);

    void OnChar(unsigned int codepoint);
    void OnKey(int key, int action);
    void ExecuteCommand(const std::string& command);
    void SetFilter(LogFilter filter);

    void SubmitInput();
    void Scrolling(int delta);
    bool IsOpen() const;
    void AddLine(const std::string& text);

public:
    void UpdateSuggestions();
    void Backspace();
    void Autocomplete();
    void NavigateSuggestions(int dir);

private:
    void InitQuad();

    GLFWwindow* m_Window;

    unsigned int m_VAO;

    bool m_Visible = false;
    GLuint m_QuadVAO;
    int m_LogMaxLines = 50;

    int m_Width = 0;
    int m_Height = 0;

    GLuint m_FontTexture;
    GLuint m_TextVAO, m_TextVBO;

    Shader* shader;

    std::string m_Input;
    std::vector<ConsoleCommand> m_Commands;
    std::vector<int> m_Suggestions;
    int m_SelectedSuggestion = -1;

    float m_CursorTimer = 0.0f;
    bool  m_CursorVisible = true;
    float consoleHeight = 300.0f;

    bool m_BlockNextChar = false;

private:
    std::vector<LogEntry> m_Logs;
    LogFilter m_Filter = LogFilter::All;

    std::vector<std::string> lines;


    bool isOpen = false;

private:
    // scroll state
    int scrollOffset = 0;
    int visibleLines = 10;

    bool draggingScrollbar = false;
    float dragStartMouseY = 0.0f;
    int dragStartOffset = 0;

    // do hit-testu paska
    float lastBarY = 0.0f;
    float lastBarHeight = 0.0f;
};