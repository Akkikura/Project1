#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <sqlite3.h>

// Opens or creates a database
sqlite3* openDatabase(const std::string& dbName);

// Creates the Orders table
void createTable(sqlite3* db);

// Displays the entire database
void displayDatabase(sqlite3* db);

// Adds a new record to the database
void addOrder(sqlite3* db);

// Deletes records by payer account with options
void deleteOrder(sqlite3* db);

// Checks if a specific payer account exists and displays related records
void checkAccountExists(sqlite3* db);

// Searches for the total amount debited by a specific payer
void findTotalAmountByPayer(sqlite3* db);

// Searches for transactions by payer account
void findTransactionsByPayer(sqlite3* db);

// Writes database records to a file
void saveDatabaseToFile(sqlite3* db, const std::string& filename);

// Loading database from file via filename
void loadDatabaseFromFile();
#endif // DATABASE_H
