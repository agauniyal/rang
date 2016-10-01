#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "rang.hpp"
#include <fstream>
#include <string>

TEST_CASE("Rang printing to non-terminals", "[file]")
{
	std::string s = "Hello World";

	SECTION("output is to a file")
	{
		rang::init();

		std::ofstream out("out.txt");
		std::streambuf *coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());

		std::cout << rang::fg::blue << s << rang::style::reset;

		std::cout.rdbuf(coutbuf);
		out.close();

		std::ifstream in("out.txt");
		std::string output;
		std::getline(in, output);

		REQUIRE(s == output);
	}
}

TEST_CASE("Rang printing to standard output", "[terminal]")
{
	std::string s = "Rang works with ";

	SECTION("output is to terminal")
	{

		std::cout << rang::fg::green << s << "std::cout" << rang::style::reset
		          << std::endl;

		std::clog << rang::fg::blue << s << "std::clog" << rang::style::reset
		          << std::endl;

		std::cerr << rang::fg::red << s << "std::cerr" << rang::style::reset
		          << std::endl;
		REQUIRE(1 == 1);
	}
}

TEST_CASE("Rang printing to non-terminals (force color)", "[file]")
{
	SECTION("output is to a file (force color)")
	{
		std::cout << rang::fg::blue << "to terminal" << rang::style::reset;

		std::ofstream out("out.txt");
		std::streambuf *coutbuf = std::cout.rdbuf();
		std::cout.rdbuf(out.rdbuf());

		std::cout << rang::control::forceColor << rang::fg::blue << "to file (force color)" << rang::style::reset << std::endl;
		std::cout << rang::control::autoColor << rang::fg::blue << "to file (don't force color)" << rang::style::reset;

		std::cout.rdbuf(coutbuf);
		out.close();

		std::ifstream in("out.txt");
		std::string output;
		std::getline(in, output);
		std::cout << " == " << output;
		std::getline(in, output);
		std::cout << " != " << output << std::endl;

		REQUIRE(1 == 1);
	}
}
