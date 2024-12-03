#include "database.h"
#include "utils.h"
#include <iostream>

int main() {
    sqlite3* db = openDatabase("orders.db");
    if (!db) return 1;

    createTable(db);

    int choice;
    do {
        displayMenu();
        std::cin >> choice;

        switch (choice) {
            case 1: displayDatabase(db); break;
            case 2: addOrder(db); break;
            case 3: deleteOrder(db); break;
            case 4: checkAccountExists(db); break;
            case 5: findTotalAmountByPayer(db); break;
            case 6: findTransactionsByPayer(db); break;
            case 7: saveDatabaseToFile(db, "database_backup.csv"); break;
            case 8: loadDatabaseFromFile(); break;
            case 9: std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid option. Try again.\n"; break;
        }
    } while (choice != 9);

    sqlite3_close(db);
    return 0;
}
