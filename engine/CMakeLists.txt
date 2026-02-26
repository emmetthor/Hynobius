cmake_minimum_required(VERSION 3.16)
project(chess-analysis-engine)

set(CMAKE_CXX_STANDARD 17)

# 告訴 CMake include 目錄在哪
include_directories(include)

# add_definitions(-D_GLIBCXX_DEBUG)
# add_definitions(-DENGINE_DEBUG)

# 目標檔案
add_library(engine-core
    src/board/Piece.cpp
    src/board/Board.cpp
    src/board/Attack.cpp
    src/board/Check.cpp

    src/move/Move.cpp
    src/move/Move_Order.cpp
    src/move/Generate_Position.cpp
    src/move/Generate_Move.cpp
    src/move/Make_Move.cpp

    src/evaluate/Material_Point.cpp
    src/evaluate/Evaluate.cpp
    src/evaluate/Control_Center.cpp
    src/evaluate/Tempo.cpp
    src/evaluate/PST.cpp
    src/evaluate/King_Safety.cpp
    src/evaluate/SEE.cpp
    src/evaluate/Mobility.cpp
    src/evaluate/Castle_Rights.cpp
    src/evaluate/Pawn_Structure.cpp

    src/pgn/Pgn_Parser.cpp
    src/pgn/Valid_Piece.cpp
    src/pgn/Pgn_Transformer.cpp

    src/search/TT.cpp
    src/search/Zobrist.cpp
    src/search/Killer_Move.cpp
    src/search/Search.cpp

    src/fen/FEN_Parser.cpp

    src/UCI/UCI.cpp
    src/UCI/UCI_Move_Parser.cpp

    src/debug.cpp
    src/Engine.cpp
)

add_library(bench-lib
    ../bench/src/Progress_Bar.cpp
    ../bench/src/Mate_In_One.cpp
    ../bench/src/Mate_In_Two.cpp
)

# 正式引擎
add_executable(chess-analysis-engine src/main.cpp)
target_link_libraries(chess-analysis-engine PRIVATE engine-core)
set_target_properties(chess-analysis-engine PROPERTIES WIN32_EXECUTABLE OFF)

# test mate

enable_testing()

add_executable(test-engine ../bench/src/Bench.cpp)
target_link_libraries(bench-lib PUBLIC engine-core)
target_link_libraries(test-engine bench-lib)
set_target_properties(test-engine PROPERTIES WIN32_EXECUTABLE OFF)

add_test(NAME EngineTest COMMAND test-engine)