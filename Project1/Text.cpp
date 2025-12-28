#include "Text.h"
#include "Shader.h"
#include <iostream>

Text::Text(int screenWidth, int screenHeight)
{
    m_Projection = glm::ortho(
        0.0f,
        (float)screenWidth,
        0.0f,
        (float)screenHeight
	);
}

Text::~Text()
{
    for (auto& pair : m_Characters)
    {
        glDeleteTextures(1, &pair.second.TextureID);
    }
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

bool Text::Init()
{
    // === VAO / VBO ===
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // 6 vertices * 4 floats (x, y, u, v)
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * 6 * 4,
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // location = 0
        4,                  // vec4
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // === Shader ===
    shader.LoadFromFiles("shaders/text.vert", "shaders/text.frag");

    shader.Use();
    shader.setInt("text", 0);

    return true;
}



GLuint Text::LoadShader(const char* vs, const char* fs) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER); glShaderSource(v, 1, &vs, NULL); glCompileShader(v);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(f, 1, &fs, NULL); glCompileShader(f);
    GLuint p = glCreateProgram(); glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}


bool Text::LoadFont(const std::string& fontPath, unsigned int fontSize)
{
    m_Characters.clear();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "FreeType init FAILED\n";
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "Font NOT found: " << fontPath << std::endl;
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "Glyph load failed: " << (int)c << std::endl;
            continue;
        }

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character2 ch;
        ch.TextureID = texture;
        ch.Size = glm::ivec2(
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows
        );
        ch.Bearing = glm::ivec2(
            face->glyph->bitmap_left,
            face->glyph->bitmap_top
        );
        ch.Advance = face->glyph->advance.x; // FT_Pos / long

        m_Characters.emplace(c, ch);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    std::cout << "Font loaded. Glyph count: "
        << m_Characters.size() << std::endl;

    return !m_Characters.empty();

    std::cout << "[Text] Glyph count = " << m_Characters.size() << std::endl;
}

void Text::SetProjection(const glm::mat4& projection)
{
    m_Projection = projection;
}

void Text::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(m_Projection));
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(shader.ID, "textAlpha"), 1.0f);  // pełna nieprzezroczystość

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    for (char c : text)
    {
        auto it = m_Characters.find(c);
        if (it == m_Characters.end())
            continue;

        Character2 ch = it->second;

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::RenderText(const std::string& text, float x, float y, float scale, glm::vec4 color)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shader.Use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(m_Projection));
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(shader.ID, "textAlpha"), color.w);  // przezroczystość z vec4

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    for (char c : text)
    {
        auto it = m_Characters.find(c);
        if (it == m_Characters.end())
            continue;

        Character2 ch = it->second;

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::SetScreenSize(int screenWidth, int screenHeight)
{
    m_Projection = glm::ortho(
        0.0f,
        (float)screenWidth,
        0.0f,
        (float)screenHeight
	);
}

void Text::Begin()
{
    glDisable(GL_SCISSOR_TEST);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader.Use();
    glUniformMatrix4fv(
        glGetUniformLocation(shader.ID, "projection"),
        1, GL_FALSE,
        glm::value_ptr(m_Projection)
    );

    glActiveTexture(GL_TEXTURE0);
}

void Text::End()
{
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Text::RenderTextOutlined(
    const std::string& text,
    float x, float y,
    float scale,
    glm::vec3 textColor,
    glm::vec3 outlineColor,
    float thickness
)
{
    // Render outline
    for (float angle = 0; angle < 2 * 3.14159f; angle += 3.14159f / 4)
    {
        float offsetX = cos(angle) * thickness;
        float offsetY = sin(angle) * thickness;
        RenderText(text, x + offsetX, y + offsetY, scale, glm::vec4(outlineColor, 1.0f));
    }
    // Render main text
    RenderText(text, x, y, scale, glm::vec4(textColor, 1.0f));
}

float Text::GetTextWidth(const std::string& text, float scale)
{
    float width = 0.0f;

    for (char c : text)
    {
        auto it = m_Characters.find(c);
        if (it == m_Characters.end()) continue;

        const Character2& ch = it->second;
        width += (ch.Advance >> 6) * scale;
    }

    return width;
}