#include "bot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <bits/types/struct_rusage.h>
#include <cstdio>
#include <string>
#include <sys/resource.h>

#include "imgui_memory_editor.h"

static MemoryEditor mem_edit;

int main() {
    using namespace std;

    boost::asio::io_context Boost_IO;
    Bot bot(Boost_IO);
    bot.init("127.0.0.1", 25565, "tute_BOT", "d42755f0f7be48ec9abc21fc1b1f567d", 765);
    bot.connect();

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

        rusage usage;
        getrusage(RUSAGE_SELF, &usage);

        std::string title;
        sprintf(title.data(), "TCP_BOT FPS: %.1f Mem: %zuMb stime: %.2f", io.Framerate, usage.ru_maxrss / 1024, usage.ru_stime.tv_usec / 1000.0);
        glfwSetWindowTitle(window, title.data());

        // Iniciar el marco de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dibujar
        ImGui::Begin("Bot CreateContext", (bool*)false, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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
        ImGui::SameLine();
        if(ImGui::Button("disconnect")) bot.disconnect();
        ImGui::SameLine();
        ImGui::Text(bot.getConnectedStatus() ? "Connected" : "Disconnected");
        ImGui::End();

        mem_edit.DrawWindow("Memory Editor", bot.debugBuff.data(), bot.debugBuff.size());

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

    bot.disconnect();

    return 0;
}
