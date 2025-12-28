#include "Buttons.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>

GLFWwindow* Buttons::m_window = nullptr;

Buttons::Buttons()
{
}

Buttons::~Buttons()
{
	// Usuń VAO/VBO przycisków
	for (auto& button : m_buttons) {
		if (button.VAO) glDeleteVertexArrays(1, &button.VAO);
		if (button.VBO) glDeleteBuffers(1, &button.VBO);
	}
	
	// Usuń program shaderów
	if (m_uiProgram) {
		glDeleteProgram(m_uiProgram);
	}
}

void Buttons::AddButton(const Button& button)
{
	// === 1. UTWÓRZ PROGRAM JEŚLI JESZCZE NIE MA ===
	if (m_uiProgram == 0)
	{
		const char* vs_src = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos; // x,y w pikselach
		layout(location = 1) in vec2 aTexCoord;
		out vec2 TexCoord;
		uniform vec2 u_resolution;

		void main()
		{
			vec2 ndc;
			ndc.x = (aPos.x / u_resolution.x) * 2.0 - 1.0;
			ndc.y = (aPos.y / u_resolution.y) * 2.0 - 1.0; // BOTTOM-LEFT -> NDC
			gl_Position = vec4(ndc, 0.0, 1.0);
			TexCoord = aTexCoord;
		}
        )";

		const char* fs_src = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform vec3 u_color;
        uniform bool u_useTexture;
        uniform sampler2D texture1;

        void main()
        {
            if (u_useTexture)
                FragColor = texture(texture1, TexCoord);
            else
                FragColor = vec4(u_color, 1.0);
        }
        )";

		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vs_src, nullptr);
		glCompileShader(vs);

		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fs_src, nullptr);
		glCompileShader(fs);

		m_uiProgram = glCreateProgram();
		glAttachShader(m_uiProgram, vs);
		glAttachShader(m_uiProgram, fs);
		glLinkProgram(m_uiProgram);

		glDeleteShader(vs);
		glDeleteShader(fs);
	}


	// === 2. ZBUDUJ PRZYCISK (KOPIE), BO MUSIMY MU PRZYPISAĆ VAO ===
	Button b = button;

	// USUŃ TE LINIE - używaj wartości z Menu.cpp!
	// b.width = 300;
	// b.height = 60;
	// b.offsetY = 100;

	float x = b.x;
	float y = b.y;
	float w = b.width;
	float h = b.height;

	float vertices[] =
	{
		x,     y + h, 0.0f, 0.0f, 1.0f,
		x + w, y + h, 0.0f, 1.0f, 1.0f,
		x + w, y,     0.0f, 1.0f, 0.0f,

		x,     y + h, 0.0f, 0.0f, 1.0f,
		x + w, y,     0.0f, 1.0f, 0.0f,
		x,     y,     0.0f, 0.0f, 0.0f
	};

	// === 3. UTWÓRZ VAO/VBO DLA TEGO PRZYCISKU ===
	glGenVertexArrays(1, &b.VAO);
	glGenBuffers(1, &b.VBO);

	glBindVertexArray(b.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, b.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// === 4. DODAJ POPRAWNEGO BUTTON ===
	m_buttons.push_back(b);
}

Button Buttons::GetTransformedButton(const Button& button, int screenWidth, int screenHeight) const
{
    float originalScreenWidth = 1920.0f;
    float originalScreenHeight = 1080.0f;
    
    float widthScale = (float)screenWidth / originalScreenWidth;
    float heightScale = (float)screenHeight / originalScreenHeight;
    
    Button transformed = button;
    transformed.width = button.width * widthScale;
    transformed.height = button.height * heightScale;
    transformed.x = (screenWidth - transformed.width) / 2.0f;
    
    float yRatio = button.y / originalScreenHeight;
    
    // SKALUJ RÓWNIEŻ OFFSET -85px proporcjonalnie do wysokości okna
    float scaledOffset = -92.0f * heightScale;
    transformed.y = yRatio * screenHeight + scaledOffset;
    
    return transformed;
}

std::string Buttons::Update()
{
	std::string clicked = "";

	double mx, my;
	glfwGetCursorPos(m_window, &mx, &my);
	int sw, sh;
	glfwGetFramebufferSize(m_window, &sw, &sh);
	my = sh - my;  // invert Y for OpenGL

	// RESETUJ hover state
	m_hoveredButtonIndex = -1;

	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		auto& b = m_buttons[i];
		
		// UŻYJ WSPÓLNEJ FUNKCJI
		Button tempButton = GetTransformedButton(b, sw, sh);

		bool hovered = Button::IsHovered(tempButton, mx, my);
		
		// ZAPISZ INDEKS PIERWSZEGO HOVEROWANEGO PRZYCISKU
		if (hovered && m_hoveredButtonIndex == -1)
		{
			m_hoveredButtonIndex = (int)i;
		}

		bool pressed = Button::IsClicked(tempButton);

		if (hovered && pressed)
		{
			clicked = Button::GetText(b);
			break;  // only one click per frame
		}
	}

	return clicked;
}

void Buttons::RenderButton(const Button& button)
{
	glUseProgram(m_uiProgram);

	int sw, sh;
	glfwGetFramebufferSize(glfwGetCurrentContext(), &sw, &sh);
	glUniform2f(glGetUniformLocation(m_uiProgram, "u_resolution"), (float)sw, (float)sh);

	double mx, my;
	glfwGetCursorPos(glfwGetCurrentContext(), &mx, &my);
	my = sh - my; // Odwróć Y

	// UŻYJ WSPÓLNEJ FUNKCJI
	Button tempButton = GetTransformedButton(button, sw, sh);
	
	float x = tempButton.x;
	float y = tempButton.y;
	float w = tempButton.width;
	float h = tempButton.height;

	bool hovered = Button::IsHovered(tempButton, mx, my);

	// SPRAWDŹ CZY TEN PRZYCISK JEST AKTUALNIE HOVEROWANY (z globalnego stanu)
	int currentButtonIndex = -1;
	for (size_t i = 0; i < m_buttons.size(); ++i)
	{
		if (m_buttons[i].VAO == button.VAO) // Porównaj po VAO (unikalny identyfikator)
		{
			currentButtonIndex = (int)i;
			break;
		}
	}
	
	// PODŚWIETL TYLKO JEŚLI TO JEST AKTUALNIE HOVEROWANY PRZYCISK
	bool shouldHighlight = (currentButtonIndex == m_hoveredButtonIndex);

	if (!button.hasHoverTexture && button.invisibleNormal)
	{
		return;
	}

	// Jeśli mamy teksturę
	if (button.hasHoverTexture)
	{
		glUniform1i(glGetUniformLocation(m_uiProgram, "u_useTexture"), 1);
		glActiveTexture(GL_TEXTURE0);

		if (shouldHighlight) // ZMIENIONE: używaj shouldHighlight zamiast hovered
			glBindTexture(GL_TEXTURE_2D, button.textureHover);
		else
			return;
	}
	else
	{
		glUniform1i(glGetUniformLocation(m_uiProgram, "u_useTexture"), 0);
		float bgCol[3] = { 0.2f, 0.25f, 0.35f };
		// zmień kolor przy hover np.
		if (shouldHighlight) { bgCol[0] = 0.35f; bgCol[1] = 0.0f; bgCol[2] = 0.0f; }
		glUniform3fv(glGetUniformLocation(m_uiProgram, "u_color"), 1, bgCol);
	}

	// zbuduj dynamiczne verteksy z nową pozycją i rozmiarem
	float vertices[] =
	{
		x,     y + h, 0.0f, 0.0f, 1.0f,
		x + w, y + h, 0.0f, 1.0f, 1.0f,
		x + w, y,     0.0f, 1.0f, 0.0f,

		x,     y + h, 0.0f, 0.0f, 1.0f,
		x + w, y,     0.0f, 1.0f, 0.0f,
		x,     y,     0.0f, 0.0f, 0.0f
	};

	// aktualizujemy verteksy TYLKO TEGO PRZYCISKU
	glBindBuffer(GL_ARRAY_BUFFER, button.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

	// RENDER BUTTONA
	glBindVertexArray(button.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glUseProgram(0);
}

void Buttons::ClearButtons()
{
	m_buttons.clear();
	// Opcjonalnie: wyczyść VAO/VBO jeśli są przypisane do przycisków
	// for (auto& button : m_buttons) {
	//     if (button.VAO) glDeleteVertexArrays(1, &button.VAO);
	//     if (button.VBO) glDeleteBuffers(1, &button.VBO);
	// }
}

void Buttons::RenderButtons()
{
    for (const auto& button : m_buttons)
    {
        RenderButton(button);
    }
}

bool Button::IsHovered(const Button& button, double mouseX, double mouseY)
{
    // Zakładamy, że (0,0) to lewy górny róg okna
    // mouseX, mouseY - współrzędne kursora w przestrzeni okna
    // button.x, button.y - lewy górny róg przycisku
    // button.width, button.height - rozmiary przycisku

    return mouseX >= button.x &&
           mouseX <= button.x + button.width &&
           mouseY >= button.y &&
           mouseY <= button.y + button.height;
}

bool Button::IsClicked(const Button& button)
{
    if (!Buttons::m_window)
        return false;

    double mouseX, mouseY;
    glfwGetCursorPos(Buttons::m_window, &mouseX, &mouseY);
    
    int sw, sh;
    glfwGetFramebufferSize(Buttons::m_window, &sw, &sh);
    mouseY = sh - mouseY;  // Odwróć Y dla OpenGL (0,0 na dole)

    // UŻYJ PRZEKAZANYCH WSPÓŁRZĘDNYCH (już przeskalowanych w Update/RenderButton)
    bool hovered = mouseX >= button.x && mouseX <= button.x + button.width &&
                   mouseY >= button.y && mouseY <= button.y + button.height;

    // Sprawdź czy wciśnięty lewy przycisk myszy
    if (hovered && glfwGetMouseButton(Buttons::m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        return true;
    }
    return false;
}

std::string Button::GetText(const Button& button)
{
	return std::string(button.text);
}