#include <iostream>
#include <string>
#include <map>

// Define a type for our function pointer (functions that take no arguments and return void)
typedef void (*CommandFunction)();

// --- Command Functions ---
void sayHello() {
    std::cout << "Bot: Hello! Nice to meet you." << std::endl;
}

void sayStatus() {
    std::cout << "Bot: Systems operational." << std::endl;
}

void sayGoodbye() {
    std::cout << "Bot: Goodbye!" << std::endl;
}

int main() {
    // Create the "dictionary" mapping string commands to function pointers
    std::map<std::string, CommandFunction> commandMap;
    
    // Register commands
    commandMap["hello"] = &sayHello;
    commandMap["status"] = &sayStatus;
    commandMap["exit"] = &sayGoodbye;

    std::string userInput;
    std::cout << "--- Command Bot Ready (Try: hello, status, exit) ---" << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, userInput);

        // Check if the command exists in our "dictionary"
        if (commandMap.find(userInput) != commandMap.end()) {
            // Call the function associated with the input
            commandMap[userInput]();
            
            if (userInput == "exit") break;
        } else {
            std::cout << "Bot: Unknown command." << std::endl;
        }
    }

    return 0;
}
