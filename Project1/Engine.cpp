#include "Engine.h"
#include "SceneManager.h"
#include "Terrain.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Engine& Engine::GetInstance(int width, int height)
{
    static Engine instance(width, height);
    return instance;
}

Engine::Engine(int width, int height)
    : sceneManager(width, height),
    game(&sceneManager, width, height),
    screenWidth(width),
    screenHeight(height),
    customCursor(nullptr),
    customCursor2(nullptr),
    activeCursor(0)
{
}

Engine::~Engine()
{
    if (customCursor)
    {
        glfwDestroyCursor(customCursor);
        customCursor = nullptr;
    }
    
    if (customCursor2)
    {
        glfwDestroyCursor(customCursor2);
        customCursor2 = nullptr;
    }
    
    if(window)
    {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

bool Engine::Init()
{
    if(!glfwInit())
    {
        return false;
    }

    // DODANE: Włącz MSAA (Multi-Sample Anti-Aliasing) 4x
    glfwWindowHint(GLFW_SAMPLES, 4);

    window = glfwCreateWindow(1920, 1080, "My Engine Window", nullptr, nullptr);

    if(!window)
    {
        glfwTerminate();
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, Engine::ScrollCallback);
    glfwSetKeyCallback(window, Engine::KeyCallback);
    glfwSetCharCallback(window, Engine::CharCallback);
    glfwMakeContextCurrent(window);
    glfwMaximizeWindow(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return false;
    }

    // DODANE: Włącz MSAA w OpenGL
    glEnable(GL_MULTISAMPLE);

    glfwSwapInterval(0);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    console = ConsoleRenderer(window, w, h);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    textRenderer = Text(screenWidth, screenHeight);
    textRenderer.Init();
    textRenderer.LoadFont("fonts/verdana.ttf", 20);
    textRenderer.SetScreenSize(screenWidth, screenHeight);
    textRenderer.SetScreenSize(w, h);
    console.SetScreenSize(w, h);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // DODANE: Ustaw niestandardowy kursor dla całej aplikacji
    CreateTestCursor();
    LoadCursor2();

    sceneManager.ChangeSceneWithLoading(SceneType::MainMenu);

    return true;
}

void Engine::SetCustomCursor(const std::string& imagePath, int cursorX, int cursorY, 
                              int width, int height, int hotspotX, int hotspotY)
{
    std::cout << "[Engine] ===== SetCustomCursor START =====" << std::endl;
    std::cout << "[Engine] Loading custom cursor from: " << imagePath << std::endl;
    
    if (!window)
    {
        std::cerr << "[Engine] ERROR: Window is NULL!" << std::endl;
        return;
    }
    
    // Wczytaj obraz - WYMUŚ 4 kanały (RGBA)
    int imgWidth, imgHeight, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load(imagePath.c_str(), &imgWidth, &imgHeight, &channels, 4); // <-- WYMUŚ 4
    
    if (!data)
    {
        std::cerr << "[Engine] Failed to load cursor image: " << imagePath << std::endl;
        std::cerr << "[Engine] STB Error: " << stbi_failure_reason() << std::endl;
        return;
    }
    
    std::cout << "[Engine] Cursor image loaded: " << imgWidth << "x" << imgHeight 
              << " original channels: " << channels << " (forced to RGBA)" << std::endl;
    
    // Sprawdź czy współrzędne są w granicach
    if (cursorX + width > imgWidth || cursorY + height > imgHeight)
    {
        std::cerr << "[Engine] Cursor coordinates out of bounds!" << std::endl;
        std::cerr << "[Engine] Image size: " << imgWidth << "x" << imgHeight << std::endl;
        std::cerr << "[Engine] Requested region: " << cursorX << "," << cursorY 
                  << " size: " << width << "x" << height << std::endl;
        stbi_image_free(data);
        return;
    }
    
    // Alokuj pamięć na wybrany fragment kursora
    unsigned char* cursorData = new unsigned char[width * height * 4];
    
    // Wytnij fragment z atlasu
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int srcX = cursorX + x;
            int srcY = cursorY + y;
            int srcIndex = (srcY * imgWidth + srcX) * 4;
            int dstIndex = (y * width + x) * 4;
            
            cursorData[dstIndex + 0] = data[srcIndex + 0]; // R
            cursorData[dstIndex + 1] = data[srcIndex + 1]; // G
            cursorData[dstIndex + 2] = data[srcIndex + 2]; // B
            cursorData[dstIndex + 3] = data[srcIndex + 3]; // A
            
            // DEBUG: Sprawdź czy alpha jest prawidłowa
            if (x == 0 && y == 0)
            {
                std::cout << "[Engine] First pixel RGBA: " 
                          << (int)cursorData[dstIndex + 0] << ", "
                          << (int)cursorData[dstIndex + 1] << ", "
                          << (int)cursorData[dstIndex + 2] << ", "
                          << (int)cursorData[dstIndex + 3] << std::endl;
            }
        }
    }
    
    // Stwórz GLFWimage
    GLFWimage cursorImage;
    cursorImage.width = width;
    cursorImage.height = height;
    cursorImage.pixels = cursorData;
    
    // Usuń stary kursor jeśli istnieje
    if (customCursor)
    {
        glfwDestroyCursor(customCursor);
        customCursor = nullptr;
    }
    
    // Stwórz nowy kursor
    customCursor = glfwCreateCursor(&cursorImage, hotspotX, hotspotY);
    
    if (customCursor)
    {
        glfwSetCursor(window, customCursor);
    }
    else
    {
    }
    
    // Zwolnij pamięć
    delete[] cursorData;
    stbi_image_free(data);
    
}

void Engine::RestoreDefaultCursor()
{
    if (customCursor)
    {
        glfwDestroyCursor(customCursor);
        customCursor = nullptr;
    }
    glfwSetCursor(window, nullptr);
}

void Engine::Run()
{
    const float DESIRED_FPS = 144.0f;
    const float DESIRED_FRAME_TIME = 1.0f / DESIRED_FPS;

    float lastFrameTime = static_cast<float>(glfwGetTime());

    int frameCount = 0;
    float fpsTimer = 0.0f;

	while(true)
	{
		if(glfwWindowShouldClose(window))
		{
			break;
		}

        glfwPollEvents();

        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lastFrameTime;

        // === Ręczny limit FPS ===
        float elapsedSinceLastFrame = currentTime - lastFrameTime;
        if (elapsedSinceLastFrame < DESIRED_FRAME_TIME)
        {
            float sleepTime = DESIRED_FRAME_TIME - elapsedSinceLastFrame;
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
            currentTime = static_cast<float>(glfwGetTime());
            dt = currentTime - lastFrameTime;
        }

        lastFrameTime = currentTime;

        // Liczenie rzeczywistego FPS
        frameCount++;
        fpsTimer += dt;
        if (fpsTimer >= 1.0f)
        {
            currentFPS = frameCount;
            frameCount = 0;
            fpsTimer -= 1.0f;
        }

        sceneManager.Update(dt);
        console.Update(dt);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneManager.Render();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        console.Render(textRenderer);
        Render();
        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	Engine* e = (Engine*)glfwGetWindowUserPointer(window);
	if (e) e->OnResize(width, height);
}

void Engine::OnResize(int w, int h)
{
	screenWidth = w;
	screenHeight = h;

    console.SetScreenSize(w, h);
    textRenderer.SetScreenSize(w, h);
}

void Engine::Render()
{
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
    std::string fpsText = "FPS: " + std::to_string(currentFPS);
    textRenderer.RenderText(
        fpsText,
        20.0f,
        900.0f,
        1.0f,
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
    );
}

void Engine::KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(w));
    if (!engine) return;

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
    {
        engine->console.Toggle();
        return;
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        engine->console.SubmitInput();
        return;
    }

    engine->console.OnKey(key, action);
}

void Engine::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    if (!engine) return;

    engine->console.Scrolling((int)yoffset);
}

Text& Engine::GetText() {
    return textRenderer;
}

void Engine::CharCallback(GLFWwindow* w, unsigned int codepoint)
{
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(w));
    if (!engine) return;

    engine->console.OnChar(codepoint);
}

void Engine::CreateTestCursor()
{
    const int size = 32;
    unsigned char* cursorData = new unsigned char[size * size * 4];
    
    // Stwórz czerwony kursor z czarną obwódką
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            int idx = (y * size + x) * 4;
            
            // Obwódka (czarna)
            if (x == 0 || y == 0 || x == size-1 || y == size-1)
            {
                cursorData[idx + 0] = 0;   // R
                cursorData[idx + 1] = 0;   // G
                cursorData[idx + 2] = 0;   // B
                cursorData[idx + 3] = 255; // A
            }
            // Środek (czerwony)
            else if (x > 4 && x < size - 4 && y > 4 && y < size - 4)
            {
                cursorData[idx + 0] = 255; // R
                cursorData[idx + 1] = 180; // G
                cursorData[idx + 2] = 0;   // B
                cursorData[idx + 3] = 255; // A
            }
            // Tło (przezroczyste)
            else
            {
                cursorData[idx + 0] = 0;
                cursorData[idx + 1] = 0;
                cursorData[idx + 2] = 0;
                cursorData[idx + 3] = 0;   // Przezroczyste
            }
        }
    }
    
    GLFWimage cursorImage;
    cursorImage.width = size;
    cursorImage.height = size;
    cursorImage.pixels = cursorData;
    
    if (customCursor)
        glfwDestroyCursor(customCursor);
    
    customCursor = glfwCreateCursor(&cursorImage, size/2, size/2);
    glfwSetCursor(window, customCursor);
    
    delete[] cursorData;
    std::cout << "[Engine] Test cursor created" << std::endl;
}

void Engine::LoadCursor2()
{
    const int size = 32;
    
    // Wczytaj z pliku lub stwórz programowo
    int imgWidth, imgHeight, channels;
    stbi_set_flip_vertically_on_load(false);
    unsigned char* data = stbi_load("img/cursor2.png", &imgWidth, &imgHeight, &channels, 4);
    
    if (data)
    {
        GLFWimage cursorImage;
        cursorImage.width = imgWidth;
        cursorImage.height = imgHeight;
        cursorImage.pixels = data;
        
        customCursor2 = glfwCreateCursor(&cursorImage, size/2, size/2);
        stbi_image_free(data);
        
        std::cout << "[Engine] Cursor2 loaded successfully" << std::endl;
    }
    else
    {
        std::cerr << "[Engine] Failed to load cursor2.png" << std::endl;
    }
}

void Engine::SwitchCursor(int cursorIndex)
{
    if (cursorIndex == 0 && customCursor)
    {
        glfwSetCursor(window, customCursor);
        activeCursor = 0;
    }
    else if (cursorIndex == 1 && customCursor2)
    {
        glfwSetCursor(window, customCursor2);
        activeCursor = 1;
    }
}