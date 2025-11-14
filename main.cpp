#include "consoleUI.h"
#include "contactBook.h"

int main() {
    ContactBook book("contacts.txt");
    ConsoleUI ui(book);
    ui.run();
    return 0;
}