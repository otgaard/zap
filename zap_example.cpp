/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#include <iostream>
#include <GLFW/glfw3.h>

static void on_error(int error, const char* description) {
    std::cerr << "Error code: " << error << "desc:" << description << std::endl;
}

int main(int argc, char* argv[]) {
    if(!glfwInit()) return -1;

    auto p = &::on_error;

    glfwWindowHint(GLFW_SAMPLES, 8);
}
