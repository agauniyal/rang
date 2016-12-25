#include "rang.hpp"

int main()
{
	std::cout << std::endl

	          << rang::control::autoColor << rang::bg::blue << rang::fg::gray
	          << "Default autoColor mode - can auto detect color capablities"
	          << rang::fg::reset << rang::bg::reset << std::endl
	          << rang::control::forceColor << rang::bg::green << rang::fg::gray
	          << "Forced color mode - if you see colored text then rang works!"
	          << rang::fg::reset << rang::bg::reset << std::endl
	          << rang::control::offColor << rang::fg::red << rang::bg::gray
	          << rang::style::crossed
	          << "No colors/attributes should be visible on this text."
	          << rang::fg::reset << rang::bg::reset << rang::style::reset

	          << std::endl;

	return 0;
}
