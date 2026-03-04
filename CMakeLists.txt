    cmake_minimum_required(VERSION 3.16)
    project(chess-analysis-engine)

    set(CMAKE_CXX_STANDARD 17)

    # 告訴 CMake include 目錄在哪
    include_directories(engine/include)

    # add_definitions(-D_GLIBCXX_DEBUG)
    # add_definitions(-DENGINE_DEBUG)

    # 目標檔案
    add_library(engine-core
        engine/src/board/Piece.cpp
        engine/src/board/Board.cpp
        engine/src/board/Attack.cpp
        engine/src/board/Check.cpp

        engine/src/move/Move.cpp
        engine/src/move/Move_Order.cpp
        engine/src/move/Generate_Position.cpp
        engine/src/move/Generate_Move.cpp
        engine/src/move/Make_Move.cpp

        engine/src/evaluate/Material_Point.cpp
        engine/src/evaluate/Evaluate.cpp
        engine/src/evaluate/Control_Center.cpp
        engine/src/evaluate/Tempo.cpp
        engine/src/evaluate/PST.cpp
        engine/src/evaluate/King_Safety.cpp
        engine/src/evaluate/SEE.cpp
        engine/src/evaluate/Mobility.cpp
        engine/src/evaluate/Castle_Rights.cpp
        engine/src/evaluate/Pawn_Structure.cpp

        engine/src/pgn/Pgn_Parser.cpp
        engine/src/pgn/Valid_Piece.cpp
        engine/src/pgn/Pgn_Transformer.cpp

        engine/src/search/TT.cpp
        engine/src/search/Zobrist.cpp
        engine/src/search/Killer_Move.cpp
        engine/src/search/Search.cpp

        engine/src/fen/FEN_Parser.cpp

        engine/src/UCI/UCI.cpp
        engine/src/UCI/UCI_Move_Parser.cpp

        engine/src/debug.cpp
        engine/src/Engine.cpp
    )

    add_library(bench-lib
        bench/src/Progress_Bar.cpp
        bench/src/Mate_In_One.cpp
        bench/src/Mate_In_Two.cpp
    )

    # 正式引擎
    add_executable(chess-analysis-engine engine/src/main.cpp)
    target_link_libraries(chess-analysis-engine PUBLIC engine-core)
    set_target_properties(chess-analysis-engine PROPERTIES WIN32_EXECUTABLE OFF)

    # test mate

    enable_testing()

    add_executable(test-bench bench/src/Bench.cpp)
    target_link_libraries(bench-lib PUBLIC engine-core)
    target_link_libraries(test-bench bench-lib)
    set_target_properties(test-bench PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME BenchTest COMMAND test-bench)

    # test correctness

    add_executable(test-correctness tests/Tests.cpp)
    target_link_libraries(test-correctness PRIVATE engine-core)
    set_target_properties(test-correctness PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME CorrectnessTest COMMAND test-correctness)