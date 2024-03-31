#include "bot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

struct ventana {
    std::string title;
    uint16_t id;
    std::vector<uint8_t> buff;
};

std::vector<ventana> ventanas_l;

void ShowMemoryList(std::string name, std::vector<uint8_t> buff, uint16_t id, int bytesPerRow = 32) {

    ventana aux = {
        .title = name,
        .id = id,
        .buff = buff,
    };

    auto ite =
    std::lower_bound(ventanas_l.begin(), ventanas_l.end(), aux, [](ventana a, ventana b) { return a.id < b.id; });
    if(ite == ventanas_l.end()) {
        ventanas_l.push_back(aux);
    } else if(ite->id != id) {
        ventanas_l.insert(ite, aux);
    } else {
        *ite = aux;
    }

    for(ventana vent : ventanas_l) {
        std::snprintf(vent.title.data(), sizeof(vent.title), "%s 0x%02X",vent.title.c_str(), vent.id);
        ImGui::Begin(vent.title.data());
        uint8_t memorySize = vent.buff.size();

        // Iterar sobre la memoria y mostrar cada byte
        for(int i = 0; i < memorySize; i += bytesPerRow) {
            ImGui::Text("0x%04X: ", i); // Dirección de memoria

            // Mostrar bytes de la fila actual
            for(int j = 0; j < bytesPerRow; j++) {
                if(i + j < memorySize) {
                    ImGui::SameLine();
                    ImGui::Text("%02X ", vent.buff[i + j]);
                }
            }
        }

        ImGui::End();
    }
}

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
    glfwSwapInterval(1); // Habilitar la sincronización vertical

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

        // Iniciar el marco de ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Dibujar
        ShowMemoryList("READ BUFF", bot.readBuff, bot.id);

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

    bot.th.detach(); // f bot

    return 0;
}
