#include "Console.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Shader.h"
#include "Text.h"

#include <algorithm>
#include <iostream>

ConsoleRenderer::ConsoleRenderer(GLFWwindow* window, int screenWidth, int screenHeight)
    : m_Window(window), m_Width(screenWidth), m_Height(screenHeight)
{
    shader = new Shader("shaders/console.vert", "shaders/console.frag");

    InitQuad();
    AddLog("Konsola gotowa! Komendy: help, clear, god", MessageType::Info);

    m_Commands =
    {
        { "help",  "Show all commands" },
        { "clear", "Clear console" },
        { "fps",   "Toggle FPS display" },
        { "quit",  "Exit game" },
        { "god",   "niesmiertelnosc"}
    };
}

void ConsoleRenderer::InitQuad() {
    float quadVertices[] = {
        // Pos          TexCoords
        0.0f, 0.0f,    0.0f, 0.0f,
        1.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  1.0f,   1.0f, 1.0f,
        0.0f,  1.0f,   0.0f, 1.0f,
    };
    glGenVertexArrays(1, &m_QuadVAO);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
}

void ConsoleRenderer::Update(float dt)
{
    if (!m_Visible) return;

    m_CursorTimer += dt;

    if (m_CursorTimer >= 0.5f)   // pół sekundy
    {
        m_CursorVisible = !m_CursorVisible;
        m_CursorTimer = 0.0f;
    }
}

void ConsoleRenderer::Toggle()
{
    m_Visible = !m_Visible;

    m_BlockNextChar = true;

    if (m_Visible)
    {
       // glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
        //glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void ConsoleRenderer::AddLog(const std::string& text, MessageType type) {
    m_Logs.push_back({ type, text });

    if (m_Logs.size() > m_LogMaxLines)
        m_Logs.erase(m_Logs.begin());
}

void ConsoleRenderer::Render(Text& text)
{
    if (!m_Visible) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);

    const float consoleHeight = 300.0f;

    // 1️⃣ narysuj tło konsoli
    shader->Use();

    shader->SetVec2("uPosition", glm::vec2(0.0f, 0.0f));
    shader->SetVec2("uSize", glm::vec2((float)m_Width, consoleHeight));
    shader->SetVec2("uScreen", glm::vec2((float)m_Width, (float)m_Height));
    shader->SetVec4("uColor", glm::vec4(0.05f, 0.05f, 0.05f, 0.85f));

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // 2️⃣ narysuj linie logów

    const float paddingTop = 10.0f;
    const float paddingBottom = 10.0f;
    const float inputHeight = 30.0f;
    const float lineHeight = 18.0f;

    const float logsTopY = paddingBottom + inputHeight;
    const float logsHeight = consoleHeight - logsTopY - paddingTop;
    const int visibleLines = std::max(1, (int)(logsHeight / lineHeight));

    float startX = 20.0f;
    float startY = logsTopY;
    float fontScale = 0.6f;

    int startIndex = (int)m_Logs.size() - 1 - scrollOffset;

    float y = startY;

    int shownLines = 0;

    int filteredLines = 0;

    for (const auto& log : m_Logs)
    {
        if (log.type == MessageType::Console)
            continue;

        if (m_Filter == LogFilter::All ||
            (m_Filter == LogFilter::Info && log.type == MessageType::Info) ||
            (m_Filter == LogFilter::Warning && log.type == MessageType::Warning) ||
            (m_Filter == LogFilter::Error && log.type == MessageType::Error))
        {
            filteredLines++;
        }
    }

    std::string msg;

    if (m_Filter == LogFilter::Error)   msg = "No errors found";
    if (m_Filter == LogFilter::Warning) msg = "No warnings found";
    if (m_Filter == LogFilter::Info)    msg = "No info messages";

    if (filteredLines == 0 && m_Filter != LogFilter::All)
    {
        text.RenderText(
            msg,
            startX,
            y,
            fontScale,
            glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)
        );
    }
    else
    {
        for (int i = 0; i < (int)m_Logs.size() && shownLines < visibleLines; ++i)
        {
            int index = startIndex - i;
            if (index < 0) break;

            const LogEntry& log = m_Logs[index];

            // 🔍 FILTR
            if (log.type != MessageType::Console && m_Filter != LogFilter::All)
            {
                if ((m_Filter == LogFilter::Info && log.type != MessageType::Info) ||
                    (m_Filter == LogFilter::Warning && log.type != MessageType::Warning) ||
                    (m_Filter == LogFilter::Error && log.type != MessageType::Error))
                {
                    continue;
                }
            }

            // 🎨 kolor wg typu
            glm::vec4 textColor = { 1,1,1,1 };
            if (log.type == MessageType::Warning) textColor = { 1,1,0,1 };
            if (log.type == MessageType::Error)   textColor = { 1,0,0,1 };

            text.RenderText(
                log.text,
                startX,
                y,
                fontScale,
                textColor
            );

            y += lineHeight;
            shownLines++;
        }
    }

    // ==============================
    // SUGESTIE KOMEND
    // ==============================

    float suggestionsY = 20.0f + inputHeight + 5.0f;
    float suggestionsX = startX;
    float suggestionScale = 0.7f;

    // max ilość podpowiedzi (żeby nie zasłoniły logów)
    const int maxSuggestions = 5;

    for (int i = 0; i < (int)m_Suggestions.size() && i < maxSuggestions; i++)
    {
        const auto& cmd = m_Commands[m_Suggestions[i]];

        glm::vec4 color =
            (i == m_SelectedSuggestion)
            ? glm::vec4(1.0f, 1.0f, 0.4f, 1.0f)
            : glm::vec4(1.0f, 0.8f, 0.5f, 1.0f);

        text.RenderText(
            cmd.name + " - " + cmd.description,
            suggestionsX,
            suggestionsY,
            suggestionScale,
            color
        );

        suggestionsY += lineHeight;
    }


    
    // 1. Renderuj ">" w kolorze szarym
    text.RenderText(
        "> ",
        10.0f,
        20.0f,
        0.5f,
        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)
    );
    
    // 2. Oblicz szerokość ">" aby przesunąć input
    float promptWidth = 15.0f;
    
    // 3. Renderuj input i kursor w kolorze zielonkawym
    std::string inputWithCursor = m_Input + (m_CursorVisible ? "|" : "");
    
    text.RenderText(
        inputWithCursor,
        10.0f + promptWidth,  // Przesuń w prawo o szerokość ">"
        20.0f,
        0.5f,
        glm::vec4(0.3f, 1.0f, 0.4f, 1.0f)  // Jasny zielonkawy dla tekstu
    );

    int totalLines = filteredLines;
    if (totalLines <= 0)
        totalLines = 1;

    if (totalLines > visibleLines)
    {
        const float scrollAreaY = logsTopY;
        const float scrollAreaHeight = logsHeight;

        float consoleBottomY = 0.0f;

        float ratio = (float)visibleLines / (float)totalLines;
        ratio = glm::clamp(ratio, 0.05f, 1.0f);

        float barHeight = scrollAreaHeight * ratio;

        int maxOffset = std::max(1, totalLines - visibleLines);

        float t = (float)scrollOffset / (float)maxOffset;
        float barY = scrollAreaY + t * (scrollAreaHeight - barHeight);

        float barWidth = 6.0f;
        float barX = m_Width - barWidth - 4.0f;

        shader->Use();
        shader->SetVec2("uPosition", { barX, barY });
        shader->SetVec2("uSize", { barWidth, barHeight });
        shader->SetVec4("uColor", { 0.6f, 0.6f, 0.6f, 0.8f });

        glBindVertexArray(m_QuadVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    if (totalLines > visibleLines)
    {
        // rysuj pasek
    }

    glEnable(GL_DEPTH_TEST);
}

void ConsoleRenderer::SetScreenSize(int w, int h)
{
    m_Width = w;
    m_Height = h;
}

void ConsoleRenderer::OnChar(unsigned int codepoint)
{
    if (m_BlockNextChar)
    {
        m_BlockNextChar = false;
        return;
    }

    if (!m_Visible) return;

    // printable ASCII + UTF-8 basic
    if (codepoint >= 32 && codepoint <= 126)
    {
        m_Input += static_cast<char>(codepoint);

        UpdateSuggestions();
    }
}

void ConsoleRenderer::Backspace()
{
    if (!m_Input.empty())
    {
        m_Input.pop_back();
    }

    UpdateSuggestions();
}

void ConsoleRenderer::Autocomplete()
{
    if (m_Suggestions.empty())
        return;

    int index = m_Suggestions[
        m_SelectedSuggestion >= 0 ? m_SelectedSuggestion : 0
    ];

    m_Input = m_Commands[index].name;

    UpdateSuggestions();
}

void ConsoleRenderer::NavigateSuggestions(int dir)
{
    if (m_Suggestions.empty())
        return;

    m_SelectedSuggestion += dir;

    if (m_SelectedSuggestion < 0)
        m_SelectedSuggestion = (int)m_Suggestions.size() - 1;

    if (m_SelectedSuggestion >= m_Suggestions.size())
        m_SelectedSuggestion = 0;
}

void ConsoleRenderer::OnKey(int key, int action)
{
    if (!m_Visible) return;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    if (key == GLFW_KEY_BACKSPACE)
    {
        Backspace();
        return;
    }

    if (key == GLFW_KEY_ENTER)
    {

        AddLog("> " + m_Input, MessageType::Info);

        m_Input.clear();

        m_Suggestions.clear();
        m_SelectedSuggestion = -1;

        return;
    }

    if (key == GLFW_KEY_TAB)
        Autocomplete();

    if (key == GLFW_KEY_UP)
        NavigateSuggestions(-1);

    if (key == GLFW_KEY_DOWN)
        NavigateSuggestions(1);
}

void ConsoleRenderer::ExecuteCommand(const std::string& command)
{
    AddLog("> " + command, MessageType::Info); // pokaż wpisaną komendę

    if (command == "help")
    {
        AddLog("Dostepne komendy:", MessageType::Info);
        AddLog(" help  - lista komend", MessageType::Info);
        AddLog(" clear - wyczysc konsolę", MessageType::Info);
        AddLog(" god   - tryb niesmiertelnosci", MessageType::Info);

    }
    else if (command == "clear")
    {
        m_Logs.clear();
    }
    else if (command == "god")
    {
        AddLog("[INFO] God mode ON", MessageType::Info);
        // engine->SetGodMode(true);
    }
    else if (command == "quit")
    {
        exit(0);
    }
    else if (command == "log all")
    {
        SetFilter(LogFilter::All);
        AddLog("Showing all logs", MessageType::Info);
    }
    else if (command == "log info")
    {
        SetFilter(LogFilter::Info);
        AddLog("Showing INFO logs", MessageType::Info);
    }
    else if (command == "log warning")
    {
        SetFilter(LogFilter::Warning);
        AddLog("Showing WARNING logs", MessageType::Info);
    }
    else if (command == "log error")
    {
        SetFilter(LogFilter::Error);
        AddLog("Showing ERROR logs", MessageType::Info);
    }
    else
    {
        AddLog("[ERROR] Nieznana komenda: " + command, MessageType::Info);
    }
}

void ConsoleRenderer::SubmitInput()
{
    if (m_Input.empty()) return;

    ExecuteCommand(m_Input);
    m_Input.clear();
}

void ConsoleRenderer::Scrolling(int delta)
{
    if (!m_Visible)
        return;

    scrollOffset += delta;

    int maxOffset = std::max(0, (int)m_Logs.size() - visibleLines);
    scrollOffset = std::clamp(scrollOffset, 0, maxOffset);
}

bool ConsoleRenderer::IsOpen() const
{
    return m_Visible;  // ZMIENIONE z isOpen na m_Visible
}

void ConsoleRenderer::AddLine(const std::string& text)
{
    lines.push_back(text);

    // po dodaniu nowej linii — zawsze na dół
    scrollOffset = 0;
}

void ConsoleRenderer::SetFilter(LogFilter filter)
{
    m_Filter = filter;
    scrollOffset = 0;
}

void ConsoleRenderer::UpdateSuggestions()
{
    m_Suggestions.clear();
    m_SelectedSuggestion = -1;

    if (m_Input.empty())
        return;

    for (int i = 0; i < m_Commands.size(); i++)
    {
        if (m_Commands[i].name.rfind(m_Input, 0) == 0)
        {
            m_Suggestions.push_back(i);
        }
    }
}