#include "bot.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <GLFW/glfw3.h>

int main() {
    using namespace std;

    boost::asio::io_context Boost_IO;
    Bot bot(Boost_IO);
    bot.init("127.0.0.1", 25565, "tute_BOT", "d42755f0f7be48ec9abc21fc1b1f567d", 765);

    // while(true) { sleep(5); }
    // getchar();

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "TCP BOT", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Create window with graphics context
    if(window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    getchar();

    bot.th.detach(); // f bot

    return 0;
}
