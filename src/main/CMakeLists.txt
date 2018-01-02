cmake_minimum_required(VERSION 3.9)

add_library(joinjs STATIC joinjs.cpp joinjs.h schema_parser.h exceptions.h map.h json_parser.h types.h object_scaffold.h)