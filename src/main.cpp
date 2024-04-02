#include "bot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdio>
#include <string>

#include "imgui_memory_editor.h"

static MemoryEditor mem_edit;

int main() {
    using namespace std;

    boost::asio::io_context Boost_IO;
    Bot bot(Boost_IO);
    bot.init("127.0.0.1", 25565, "tute_BOT", "d42755f0f7be48ec9abc21fc1b1f567d", 765);

    // Inicializar GLFW
    if(!glfwInit()) { return -1; }

    // Crear una ventana
    GLFWwindow* window = glfwCreateWindow(800, 600, "TCP_BOT", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }

    // Configurar contexto de OpenGL
    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); // Habilitar la sincronización vertical

    // Inicializar ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while(!glfwWindowShouldClose(window)) {
        // Procesar eventos
        glfwPollEvents();

        std::string title;
        sprintf(title.data(), "TCP_BOT FPS: %.1f", io.Framerate);
        glfwSetWindowTitle(window, title.data());

        // Iniciar el marco de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dibujar
        ImGui::Begin("Bot CreateContext");
        ImGui::Text("IP:  ");
        ImGui::SameLine();
        ImGui::InputText("##ID1", bot.addr.data(), bot.addr.size() + 1);

        ImGui::SameLine();

        int Port = bot.port;
        ImGui::Text("Port:");
        ImGui::SameLine();
        ImGui::InputInt("##ID2", &Port);

        ImGui::Text("Name:");
        ImGui::SameLine();
        ImGui::InputText("##ID3", bot.name.data(), bot.name.size() + 1);

        ImGui::Text("uuid:");
        ImGui::SameLine();
        ImGui::InputText("##ID4", bot.uuid.data(), bot.uuid.size() + 1);

        if(ImGui::Button("connect")) bot.connect();
        if(ImGui::Button("disconnect")) bot.disconnect();

        ImGui::End();

        // mem_edit.DrawWindow("Memory Editor", bot.addr.data(), bot.addr.size());


        // Renderizar
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Limpiar ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Limpiar GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
