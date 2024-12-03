#ifndef UTILS_H
#define UTILS_H

#include <string>

// Validates and retrieves a correctly formatted account number
std::string getValidatedAccount(const std::string& prompt);

// Validates the account number structure
bool validateAccountNumber(const std::string& account);

// Displays the main menu
void displayMenu();

#endif // UTILS_H
