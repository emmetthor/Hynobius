    cmake_minimum_required(VERSION 3.16)
    project(Hynobius)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
        engine/src/move/Make_BitMove.cpp

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
        engine/src/pgn/Pgn_Transformer.cpp

        engine/src/search/TT.cpp
        engine/src/search/Zobrist.cpp
        engine/src/search/Killer_Move.cpp
        engine/src/search/Search.cpp

        engine/src/fen/FEN_Parser.cpp

        engine/src/UCI/UCI.cpp
        engine/src/UCI/UCI_Move_Parser.cpp

        engine/src/Engine.cpp
    )

    # 正式引擎
    add_executable(Hynobius engine/src/main.cpp)
    target_link_libraries(Hynobius PUBLIC engine-core)
    set_target_properties(Hynobius PROPERTIES WIN32_EXECUTABLE OFF)

    # test mate

    enable_testing()

    # test board consistency
    add_executable(test-board-consistency tests/Board_Consistency.cpp)
    target_link_libraries(test-board-consistency PRIVATE engine-core)
    set_target_properties(test-board-consistency PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME BoardConsistencyTest COMMAND test-board-consistency)

    # test perft
    add_executable(test-perft tests/Perft.cpp)
    target_link_libraries(test-perft PRIVATE engine-core)
    set_target_properties(test-perft PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME PerftTest COMMAND test-perft)

    # test M1
    add_executable(test-m1 tests/bench/src/Mate_In_One.cpp)
    target_link_libraries(test-m1 PRIVATE engine-core)
    set_target_properties(test-m1 PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME M1Test COMMAND test-m1)

    # test M2
    add_executable(test-m2 tests/bench/src/Mate_In_One.cpp)
    target_link_libraries(test-m2 PRIVATE engine-core)
    set_target_properties(test-m2 PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME M2Test COMMAND test-m2)

    # test BitMove
    add_executable(test-bitmove tests/Bit_Move_Correctness.cpp)
    target_link_libraries(test-bitmove PRIVATE engine-core)
    set_target_properties(test-bitmove PROPERTIES WIN32_EXECUTABLE OFF)

    add_test(NAME BitMoveTest COMMAND test-bitmove)