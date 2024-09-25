add_library(ast)
target_sources(ast PRIVATE
  ast/expr.cc
)

add_library(parser)
target_sources(parser PRIVATE
    parsing/parser.cpp
)
add_executable(parse)
target_sources(parse PRIVATE
    parsing/parse.cpp
)
target_link_libraries(parse PRIVATE parser)

# add_library(serde)
# target_sources(serde PRIVATE
#   serde/serialize.cpp
#   serde/deserialize.cpp
# )
