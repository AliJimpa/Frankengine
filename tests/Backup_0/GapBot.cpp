#include <iostream>
#include <string>
#include <algorithm>

// Function to convert string to lowercase to make input matching easier
std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

int main() {
    std::string userInput;

    std::cout << "--- GapBot Console ---" << std::endl;
    std::cout << "Bot: Hello! I am a simple console bot. Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, userInput);

        std::string input = toLower(userInput);

        if (input == "exit" || input == "quit") {
            std::cout << "Bot: Goodbye!" << std::endl;
            break;
        } 
        else if (input.find("hello") != std::string::npos || input.find("hi") != std::string::npos) {
            std::cout << "Bot: Hello there! How can I help you today?" << std::endl;
        } 
        else if (input.find("how are you") != std::string::npos) {
            std::cout << "Bot: I am just a program, but I am functioning perfectly!" << std::endl;
        } 
        else if (input.find("name") != std::string::npos) {
            std::cout << "Bot: You can call me GapBot." << std::endl;
        } 
        else {
            std::cout << "Bot: I'm not sure how to respond to '" << userInput << "'." << std::endl;
        }
    }

    return 0;
}
