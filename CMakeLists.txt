cmake_minimum_required(VERSION 3.16)

project(raylib_tuto
    VERSION 0.1.0
    DESCRIPTION "Projeto de estudo de raylib gerenciado com CMake"
    LANGUAGES C CXX
)

# ---------------------------------------------------------------------------
# Padrões de linguagem
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gera compile_commands.json (usado por clangd/ferramentas de LSP e análise)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------------------
# Dependências via CMake FetchContent (gerenciador de dependências)
# ---------------------------------------------------------------------------
include(FetchContent)

# Evita compilar exemplos/jogos do próprio raylib (só precisamos da lib)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# ---------------------------------------------------------------------------
# Backend de janela no Linux: usamos Wayland por padrão, pois dispensa as
# bibliotecas de desenvolvimento do X11 (libxrandr-dev, libxinerama-dev, etc).
# Para voltar a usar X11, configure com:
#   -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=ON
# ---------------------------------------------------------------------------
set(GLFW_BUILD_WAYLAND ON  CACHE BOOL "Compila o GLFW do raylib com suporte a Wayland")
set(GLFW_BUILD_X11     OFF CACHE BOOL "Compila o GLFW do raylib com suporte a X11")

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(raylib)

# ---------------------------------------------------------------------------
# Executável do projeto
# ---------------------------------------------------------------------------
add_executable(fossil-waves
    src/main.c
)
target_link_libraries(fossil-waves PRIVATE raylib)

# Windows: subsistema gráfico (sem console)
if(WIN32)
    set_target_properties(raylib_tuto snake PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
