#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "contactBook.h"

using namespace std;

class ConsoleUI {
private:
    ContactBook& contactBook;
    
    void showMainMenu();
    void showAllContacts();
    void addContact();
    void searchContacts();
    void removeContact();
    
    string getInput(const string& prompt);
    int getIntInput(const string& prompt);

public:
    ConsoleUI(ContactBook& book);
    void run();
};

#endif