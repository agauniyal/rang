#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "rang.hpp"

#include <string>

SCENARIO("formatted input", "[input]") {
    rang::init();

    WHEN("the user enters a line") {
        std::string in;
        std::cout << "Type \"Yes\" exactly as shown and press Return: ";
        std::cin >> in;

        THEN("input is buffered") {
            REQUIRE(in == "Yes");
        }
    }

    WHEN("the user enters a line after blocking unformatted input") {
        rang::Key key;
        std::string in;

        std::cout << "Press Any Key to Continue";
        std::cin
            >> rang::control::flush
            >> rang::control::blocking
            >> key;
        std::cout
            << std::endl
            << "Type \"Yes\" exactly as shown and press Return: ";
        std::cin >> in;

        THEN("input is buffered") {
            REQUIRE(in == "Yes");
        }
    }

    WHEN("the user enters a line after non-blocking unformatted input") {
        rang::Key key;
        std::string in;

        std::cout << "Press Any Key to Continue";
        std::cin
            >> rang::control::flush
            >> rang::control::nonBlocking
            >> key;
        while(key.isEmpty()) {
            std::cin >> key;
        }
        std::cout
            << std::endl
            << "Type \"Yes\" exactly as shown and press Return: ";
        std::cin >> in;

        THEN("input is buffered") {
            REQUIRE(in == "Yes");
        }
    }
}

SCENARIO("blocking unformatted input", "[input]") {
    rang::init();

    WHEN("input is given") {
        rang::Key in;
        std::cout << "Press the 'a' key: " << std::endl;
        std::cin >> rang::control::flush >> in;

        THEN("a single character is read") {
            REQUIRE(in == 'a');
        }
    }

    WHEN("input is given after formated input") {
        rang::Key in;
        std::string str;
        std::cout
            << "Type \"Yes\" exactly as shown and press Return: ";
        std::cin
            >> str;
        std::cout << "Press the 'a' key: " << std::endl;
        std::cin >> rang::control::flush >> in;

        THEN("a single character is read") {
            REQUIRE(in == 'a');
        }
    }
}

SCENARIO("non-blocking unformatted input", "[input]") {
    rang::init();
    std::cin >> rang::control::nonBlocking;

    WHEN("no input is given") {
        rang::Key in;
        std::cout << "Not waiting for user input..." << std::endl;
        std::cin >> rang::control::flush >> in;

        THEN("the empty key is returned") {
            REQUIRE(in.isEmpty() == true);
        }
    }

    WHEN("input is given") {
        int emptyCount = 0;
        rang::Key in;
        std::cout << "Press the 'a' key: " << std::endl;
        std::cin >> rang::control::flush >> in;

        while(in.isEmpty()) {
            ++emptyCount;
            std::cin >> in;
        }

        THEN("empty keys are seen") {
            REQUIRE(emptyCount > 0);
        }

        AND_THEN("a single key is read") {
            REQUIRE(in == 'a');
        }
    }

    WHEN("input is given after formated input") {
        int emptyCount = 0;
        rang::Key in;
        std::string str;
        std::cout
            << "Type \"Yes\" exactly as shown and press Return: ";
        std::cin
            >> str;
        std::cout << "Press the 'a' key: " << std::endl;
        std::cin >> rang::control::flush >> in;

        while(in.isEmpty()) {
            ++emptyCount;
            std::cin >> in;
        }

        THEN("empty keys are seen") {
            REQUIRE(emptyCount > 0);
        }

        AND_THEN("a single key is read") {
            REQUIRE(in == 'a');
        }
    }
}

SCENARIO("special keys", "[input]") {
    rang::init();

    std::cin
        >> rang::control::flush
        >> rang::control::blocking;

#define KEY(name) \
    WHEN( #name " is pressed") { \
        rang::Key in; \
        std::cout << "\nPress <" #name "> or <Space> to Skip: "; \
        std::cin >> in; \
        THEN( #name " is returned") { \
            if(in != ' ') { \
                REQUIRE(in == rang::Key::Which::name); \
            } \
        } \
    }

    KEY(Escape) KEY(Up) KEY(Down) KEY(Right) KEY(Left)
    KEY(Insert) KEY(Delete) KEY(PageUp) KEY(PageDown) KEY(Home) KEY(End)
    KEY(F1) KEY(F2) KEY(F3) KEY(F4) KEY(F5) KEY(F6) 
    KEY(F7) KEY(F8) KEY(F9) KEY(F10) KEY(F11) KEY(F12) 
    KEY(Backspace)
}

