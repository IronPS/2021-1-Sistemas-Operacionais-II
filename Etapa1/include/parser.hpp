#pragma once

#include <regex>

#include <cxxopts.hpp>

/*
 * Parses program parameters and returns a parser object.
 * It is also responsible for sanity checking input parameters.
 * 
 */
cxxopts::ParseResult parseClient(int argc, char* argv[]);
cxxopts::ParseResult parseServer(int argc, char* argv[]);
