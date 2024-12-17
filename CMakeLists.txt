project(starshooter_v2)
add_executable(${PROJECT_NAME} main.cpp camera.cpp  model.cpp bgmodel.cpp window.cpp)
enable_abcg(${PROJECT_NAME})
