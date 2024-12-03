#include "utils.h"
#include <iostream>
#include <regex>
#include <thread>
#include <chrono>

bool validateAccountNumber(const std::string& account) {
    if (account.length() != 20 || !std::regex_match(account, std::regex("\\d{20}"))) {
        std::cout << "Account number must be a 20-digit integer.\n";
        return false;
    }

    std::string ownerCode = account.substr(0, 3);
    if (ownerCode != "408" && ownerCode != "407" && ownerCode != "405" && ownerCode != "406") {
        std::cout << "Invalid owner code. Must be 408 (individuals), 407 (companies), 405/406 (government).\n";
        return false;
    }

    std::string structureCode = account.substr(3, 2);
    if (structureCode != "02" && structureCode != "01" && structureCode != "03") {
        std::cout << "Invalid structure code. Must be 02 (commercial), 01 (physical institution), or 03 (non-commercial).\n";
        return false;
    }

    std::string currencyCode = account.substr(5, 3);
    if (currencyCode != "810" && currencyCode != "840" && currencyCode != "978" &&
        currencyCode != "156" && currencyCode != "344") {
        std::cout << "Invalid currency code. Must be 810 (RUB), 840 (USD), 978 (EUR), 156 (CNY), or 344 (HKD).\n";
        return false;
    }

    return true;
}

std::string getValidatedAccount(const std::string& prompt) {
    std::string account;
    while (true) {
        std::cout << prompt;
        std::cin >> account;

        if (validateAccountNumber(account)) {
            break;
        }

        std::cout << "Please enter a valid account number.\n";
    }
    return account;
}

void displayMenu() {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\nOrders Management Menu:\n";
    std::cout << "1. Show all orders\n";
    std::cout << "2. Add a new order\n";
    std::cout << "3. Delete an order\n";
    std::cout << "4. Check if account exists\n";
    std::cout << "5. Find total amount by payer\n";
    std::cout << "6. Find transactions by payer\n";
    std::cout << "7. Save database to file\n";
    std::cout << "8. Open database from file\n";
    std::cout << "9. Exit\n";
    std::cout << "Choose an option: ";
}
