cmake_minimum_required(VERSION 3.1)
project(cell)


# find_package()

set (CMAKE_CXX_STANDARD 11)

include_directories(.)

add_library(cell
  cell/camera/camera_frustum.cpp
  cell/camera/camera.cpp
  cell/camera/fly_camera.cpp
  cell/imgui/imgui_demo.cpp
  cell/imgui/imgui_draw.cpp
  cell/imgui/imgui.cpp
  cell/mesh/circle.cpp
  cell/mesh/cube.cpp
  cell/mesh/mesh.cpp
  cell/mesh/plane.cpp
  cell/mesh/quad.cpp
  cell/mesh/sphere.cpp
  cell/mesh/torus.cpp
  cell/renderer/command_buffer.cpp
  cell/renderer/gl_cache.cpp
  cell/renderer/MaterialLibrary.cpp
  cell/renderer/pbr_capture.cpp
  cell/renderer/PostProcessor.cpp
  cell/renderer/PBR.cpp
  cell/renderer/render_target.cpp
  cell/renderer/renderer.cpp
  cell/resources/mesh_loader.cpp
  cell/resources/resources.cpp
  cell/resources/shader_loader.cpp
  cell/resources/texture_loader.cpp
  cell/stb/stb_image.cpp
  cell/scene/background.cpp
  cell/scene/scene_node.cpp
  cell/scene/scene.cpp
  cell/shading/material.cpp
  cell/shading/shader.cpp
  cell/shading/texture_cube.cpp
  cell/shading/Texture.cpp
  cell/stb/stb_image.cpp
  cell/cell.cpp
)

add_library(math
  math/geometry/plane.cpp
  math/trigonometry/spherical.cpp
  math/trigonometry/polar.cpp
)

add_library(utility
  utility/logging/log.cpp
  utility/random/random.cpp
  utility/timing/time.cpp
)

add_executable(demo
  demo/scenes/pbr_test.cpp
  demo/program.cpp
)

target_link_libraries(utility math)
target_link_libraries(cell math utility)
target_link_libraries(demo cell)