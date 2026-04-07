cmake_minimum_required(VERSION 3.13)
project(VibyFarmingSimulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_FOR_WEB "Build for WebAssembly with Emscripten" OFF)

if(BUILD_FOR_WEB)
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
endif()

add_library(farm-core STATIC
    cpp-core/src/Crop.cpp
    cpp-core/src/Farm.cpp
    cpp-core/src/FarmBridge.cpp
)

target_include_directories(farm-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp-core/include
)

if(BUILD_FOR_WEB)
    add_executable(viby-farm bridge/main.cpp)
    target_link_libraries(viby-farm PRIVATE farm-core)
    
    target_compile_definitions(viby-farm PRIVATE BUILD_FOR_WEB)
    
    set_target_properties(viby-farm PROPERTIES
        LINK_FLAGS "\
            -s WASM=1 \
            -s EXPORT_ES6=1 \
            -s MODULARIZE=1 \
            -s EXPORT_NAME='createModule' \
            -s ALLOW_MEMORY_GROWTH=1 \
            -s EXPORTED_FUNCTIONS='[\"_initGame\",\"_updateWorld\",\"_plantSeed\",\"_harvestCrop\",\"_clearWitheredCrop\",\"_getGameStateJson\"]' \
            -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]' \
            -s NO_EXIT_RUNTIME=1 \
            -s ENVIRONMENT='web' \
        "
    )
    
    install(TARGETS viby-farm
        LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/react-ui/public
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/react-ui/public
    )
else()
    add_executable(viby-farm-desktop bridge/main.cpp)
    target_link_libraries(viby-farm-desktop PRIVATE farm-core)
endif()
