cmake_minimum_required(VERSION 3.20)
project(myapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Общие настройки для всех платформ
if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Поиск LibArchive с поддержкой Homebrew для macOS
if(APPLE)
    # Пути для Homebrew на Apple Silicon
    set(HOMEBREW_PREFIX "/opt/homebrew")
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}/opt/libarchive")
endif()

find_package(LibArchive REQUIRED)

# Исходные файлы
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Настройка путей для заголовков
target_include_directories(${PROJECT_NAME} PRIVATE
    ${LibArchive_INCLUDE_DIRS}
)

# Линковка библиотек
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${LibArchive_LIBRARIES}
)

# Обработка std::filesystem
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

# Установка для Unix-систем
if(UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()