CMD = emcc main.cpp \
    dependencies/ImGui/imgui.cpp \
    dependencies/imgui/imgui_draw.cpp \
    dependencies/imgui/imgui_tables.cpp \
    dependencies/imgui/imgui_widgets.cpp \
    dependencies/imgui/imgui_demo.cpp \
    renderer/shader_compilation.cpp \
    renderer/rendering_loop.cpp \
    renderer/mapload.cpp \
    dependencies/ImGui/backends/imgui_impl_glfw.cpp \
    dependencies/ImGui/backends/imgui_impl_opengl3.cpp \
    game/Player.cpp \
    game/Collider.cpp \
    -Idependencies/ImGui \
    -Idependencies/ImGui/backends \
    -Idependencies/glm \
    -Ires/shaders \
    -Irenderer \
    -Idependencies/OBJ_Loader \
    -Idependencies/json \
    -Idependencies/stb_image \
    -Idependencies/game \
    -s WASM=1 \
    -s USE_GLFW=3 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ASYNCIFY \
    -s INVOKE_RUN=0 \
    -s EXIT_RUNTIME=1 \
    -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','callMain'] \
    --preload-file res \
    -O3 \
    -o ../game-ui/public/test.js

run:
	@$(CMD)
