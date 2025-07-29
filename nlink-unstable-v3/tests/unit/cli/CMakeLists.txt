# CLI component unit tests
add_executable(test_cli_parser test_cli_parser.c)
target_link_libraries(test_cli_parser nlink_cli_static)
add_test(NAME test_cli_parser COMMAND test_cli_parser)
