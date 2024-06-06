#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>

using namespace std;

class Item 
{
protected:
    string name;
    string category;
    double price;
    int quantity;

public:
    Item(const string& name, const string& category, double price, int quantity)
        : name(name), category(category), price(price), quantity(quantity) {}

    virtual ~Item() {}

    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void setQuantity(int quantity) { this->quantity = quantity; }

    virtual void print() const 
    {
        cout << "Name: " << name << endl;
        cout << "Category: " << category << endl;
        cout << "Price: " << price << endl;
        cout << "Quantity: " << quantity << endl;
    }
};

class Product : public Item 
{
private:
    string expirationDate;

public:
    Product(const string& name, const string& expirationDate, const string& category, double price, int quantity)
        : Item(name, category, price, quantity), expirationDate(expirationDate) {}

    string getExpirationDate() const { return expirationDate; }

    void print() const override 
    {
        Item::print();
        cout << "Expiration Date: " << expirationDate << endl;
    }
};

class NonFoodItem : public Item 
{
public:
    NonFoodItem(const string& name, const string& category, double price, int quantity)
        : Item(name, category, price, quantity) {}

    void print() const override 
    {
        Item::print();
    }
};

class Supermarket 
{
private:
    vector<Item*> items;

public:
    ~Supermarket() 
    {
        for (auto item : items) 
        {
            delete item;
        }
    }

    void addItem(Item* item) 
    {
        for (auto& i : items) 
        {
            if (i->getName() == item->getName() && i->getCategory() == item->getCategory()) 
            {
                i->setQuantity(i->getQuantity() + item->getQuantity());
                delete item;
                return;
            }
        }
        items.push_back(item);
    }

    void saveToFile(const string& filename) const 
    {
        ofstream file(filename);
        for (const auto& item : items) 
        {
            file << item->getName() << "," << item->getCategory() << ","
                << item->getPrice() << "," << item->getQuantity();
            if (auto product = dynamic_cast<Product*>(item)) 
            {
                file << "," << product->getExpirationDate();
            }
            file << endl;
        }
        file.close();
    }

    void loadFromFile(const string& filename) 
    {
        ifstream file(filename);
        string name, category, expirationDate, line;
        double price;
        int quantity;
        while (getline(file, line)) 
        {
            stringstream ss(line);
            getline(ss, name, ',');
            getline(ss, category, ',');
            ss >> price;
            ss.ignore(1, ',');
            ss >> quantity;
            ss.ignore(1, ',');
            if (category == "dairy" || category == "bakery") 
            {
                getline(ss, expirationDate);
                addItem(new Product(name, expirationDate, category, price, quantity));
            }
            else 
            {
                addItem(new NonFoodItem(name, category, price, quantity));
            }
        }
        file.close();
    }

    void viewItemsByCategory() const 
    {
        map<string, vector<Item*>> categorizedItems;
        for (const auto& item : items) 
        {
            categorizedItems[item->getCategory()].push_back(item);
        }

        for (const auto& pair : categorizedItems) 
        {
            cout << "Category: " << pair.first << endl;
            for (const auto& item : pair.second) 
            {
                item->print();
            }
        }
    }

    void viewAllItems() const 
    {
        cout << "All items:" << endl;
        for (const auto& item : items) 
        {
            cout << "Name: " << item->getName() << ", Category: " << item->getCategory() << ", Price: " << item->getPrice();
            if (auto product = dynamic_cast<Product*>(item)) 
            {
                cout << ", Expiration Date: " << product->getExpirationDate();
            }
            cout << endl;
        }
    }

    void searchItem(const string& name) const 
    {
        bool found = false;
        for (const auto& item : items) 
        {
            if (item->getName() == name) 
            {
                item->print();
                found = true;
            }
        }
        if (!found) 
        {
            cout << "Item not found." << endl;
        }
    }

    void deleteItem(const string& name) 
    {
        auto it = remove_if(items.begin(), items.end(),
            [&name](Item* item) { return item->getName() == name; });

        if (it != items.end()) 
        {
            delete* it;
            items.erase(it, items.end());
            cout << "Item deleted successfully." << endl;
        }
        else 
        {
            cout << "Item not found." << endl;
        }
    }

    void sellItems(const vector<pair<string, int>>& itemsToSell) 
    {
        double totalCost = 0.0;
        for (const auto& item : itemsToSell) 
        {
            const string& name = item.first;
            const int& quantity = item.second;

            bool itemFound = false;
            for (auto& item : items) 
            {
                if (item->getName() == name) 
                {
                    if (item->getQuantity() >= quantity) 
                    {
                        item->setQuantity(item->getQuantity() - quantity);
                        totalCost += item->getPrice() * quantity;
                        cout << "Item '" << name << "' sold successfully." << endl;
                        itemFound = true;
                        break;
                    }
                    else 
                    {
                        cout << "Not enough quantity of item '" << name << "' available." << endl;
                        itemFound = true;
                        break;
                    }
                }
            }
            if (!itemFound) 
            {
                cout << "Item '" << name << "' not found." << endl;
            }
        }
        cout << "Total cost: " << totalCost << endl;
    }
};

void printMenu() 
{
    cout << "1. Add Item\n";
    cout << "2. Save Items to File\n";
    cout << "3. Load Items from File\n";
    cout << "4. View Items by Category\n";
    cout << "5. View All Items\n";
    cout << "6. Search for an Item\n";
    cout << "7. Delete an Item\n";
    cout << "8. Sell an Item\n";
    cout << "9. Exit\n";
    cout << "Choose an option: ";
}

int main() 
{
    Supermarket supermarket;

    supermarket.addItem(new Product("Milk", "2024-06-10", "Dairy", 1.5, 10));
    supermarket.addItem(new Product("Bread", "2024-06-08", "Bakery", 1.2, 20));
    supermarket.addItem(new Product("Apples", "2024-06-15", "Fruits", 2.0, 15));
    supermarket.addItem(new Product("Pasta", "2024-12-31", "Pasta", 1.0, 30));
    supermarket.addItem(new Product("Soap", "2025-06-01", "Household", 0.8, 25));

    int choice;
    while (true) 
    {
        printMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 9) break;

        switch (choice) 
        {
        case 1: 
        {
            string name, category, expirationDate;
            double price;
            cout << "Enter item name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter category: ";
            getline(cin, category);
            cout << "Enter price: ";
            cin >> price;
            cout << "Enter expiration date: ";
            cin.ignore();
            getline(cin, expirationDate);
            if (category == "dairy" || category == "bakery") 
            {
                supermarket.addItem(new Product(name, expirationDate, category, price, 0));
            }
            else 
            {
                supermarket.addItem(new NonFoodItem(name, category, price, 0));
            }
            break;
        }
        case 2:
            supermarket.saveToFile("items.txt");
            break;
        case 3:
            supermarket.loadFromFile("items.txt");
            break;
        case 4:
            supermarket.viewItemsByCategory();
            break;
        case 5:
            supermarket.viewAllItems();
            break;
        case 6: 
        {
            string name;
            cout << "Enter item name to search: ";
            cin.ignore();
            getline(cin, name);
            supermarket.searchItem(name);
            break;
        }
        case 7: 
        {
            string name;
            cout << "Enter item name to delete: ";
            cin.ignore();
            getline(cin, name);
            supermarket.deleteItem(name);
            break;
        }
        case 8: 
        {
            vector<pair<string, int>> itemsToSell;
            string name;
            int quantity;
            cout << "Enter items to sell (format: name quantity, e.g. 'Milk 2'): ";
            while (true) 
            {
                cin >> name >> quantity;
                itemsToSell.push_back({ name, quantity });
                cout << "Add another item? (y/n): ";
                char response;
                cin >> response;
                if (response == 'n') break;
            }
            supermarket.sellItems(itemsToSell);
            break;
        }
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }
}