#include "database.h"
#include "utils.h"
#include <iostream>
#include <fstream>

// Opens or creates a database
sqlite3* openDatabase(const std::string& dbName) {
    sqlite3* db;
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

// Creates the Orders table
void createTable(sqlite3* db) {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Orders (
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            PayerAccount TEXT NOT NULL,
            ReceiverAccount TEXT NOT NULL,
            Amount REAL NOT NULL,
            Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

// Displays the entire database
void displayDatabase(sqlite3* db) {
    const char* sql = "SELECT ID, PayerAccount, ReceiverAccount, Amount, Timestamp FROM Orders;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << "ID | Payer Account | Receiver Account | Amount | Timestamp\n";
    std::cout << "-----------------------------------------------------------\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string payer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string receiver = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        double amount = sqlite3_column_double(stmt, 3);
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        std::cout << id << " | " << payer << " | " << receiver << " | " << amount << " | " << timestamp << "\n";
    }

    sqlite3_finalize(stmt);
}

// Adds a new record to the database
void addOrder(sqlite3* db) {
    std::string payer = getValidatedAccount("Enter payer account (20 digits): ");
    std::string receiver = getValidatedAccount("Enter receiver account (20 digits): ");
    double amount;

    std::cout << "Enter amount: ";
    std::cin >> amount;

    const char* sql = "INSERT INTO Orders (PayerAccount, ReceiverAccount, Amount) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, payer.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, receiver.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, amount);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Order successfully added!\n";
    }

    sqlite3_finalize(stmt);
}

void deleteOrder(sqlite3* db) {
    std::string payer;
    std::cout << "Enter the payer account to delete orders: ";
    std::cin >> payer;

    const char* sql = "DELETE FROM Orders WHERE PayerAccount = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, payer.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }
    else {
        std::cout << "Orders successfully deleted!\n";
    }

    sqlite3_finalize(stmt);
}

void checkAccountExists(sqlite3* db) {
    std::string payer;
    std::cout << "Enter the payer account to check: ";
    std::cin >> payer;

    const char* sql = "SELECT COUNT(*) FROM Orders WHERE PayerAccount = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, payer.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count > 0) {
            std::cout << "Account exists with " << count << " transactions.\n";
        }
        else {
            std::cout << "No transactions found for this account.\n";
        }
    }
    else {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
}

void findTotalAmountByPayer(sqlite3* db) {
    std::string payer;
    std::cout << "Enter the payer account to calculate total amount: ";
    std::cin >> payer;

    const char* sql = "SELECT SUM(Amount) FROM Orders WHERE PayerAccount = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, payer.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        double total = sqlite3_column_double(stmt, 0);
        std::cout << "Total amount debited by account " << payer << ": " << total << "\n";
    }
    else {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
}

void findTransactionsByPayer(sqlite3* db) {
    std::string payer;
    std::cout << "Enter the payer account to find transactions: ";
    std::cin >> payer;

    const char* sql = "SELECT ReceiverAccount, Amount, Timestamp FROM Orders WHERE PayerAccount = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, payer.c_str(), -1, SQLITE_STATIC);

    std::cout << "Receiver Account | Amount | Timestamp\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string receiver = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        double amount = sqlite3_column_double(stmt, 1);
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        std::cout << receiver << " | " << amount << " | " << timestamp << "\n";
    }

    sqlite3_finalize(stmt);
}

void saveDatabaseToFile(sqlite3* db, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    const char* sql = "SELECT ID, PayerAccount, ReceiverAccount, Amount, Timestamp FROM Orders;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    outFile << "ID,PayerAccount,ReceiverAccount,Amount,Timestamp\n";
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string payer = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string receiver = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        double amount = sqlite3_column_double(stmt, 3);
        std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        outFile << id << "," << payer << "," << receiver << "," << amount << "," << timestamp << "\n";
    }

    outFile.close();
    sqlite3_finalize(stmt);

    std::cout << "Database successfully saved to " << filename << "\n";
}

void loadDatabaseFromFile() {
    std::string dbName;
    std::cout << "Enter the database file name to load (e.g., database.db): ";
    std::cin >> dbName;

    sqlite3* db = openDatabase(dbName);

    if (db == nullptr) {
        std::cerr << "Failed to open database from file: " << dbName << "\n";
        return;
    }

    // Проверяем, есть ли таблица Orders
    const char* checkTableSql = R"(
        SELECT name FROM sqlite_master WHERE type='table' AND name='Orders';
    )";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, checkTableSql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    bool tableExists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        tableExists = true;
    }
    sqlite3_finalize(stmt);

    if (!tableExists) {
        std::cerr << "Table 'Orders' does not exist in the database. Please ensure the correct file is loaded.\n";
        sqlite3_close(db);
        return;
    }

    // Отображаем данные из базы данных
    std::cout << "Database successfully loaded from " << dbName << "\n";
    displayDatabase(db);

    sqlite3_close(db);
}
