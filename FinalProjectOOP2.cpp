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

public:
    Item(const string& name, const string& category, double price)
        : name(name), category(category), price(price) {}

    virtual ~Item() {}

    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }

    virtual void print() const 
    {
        cout << "Name: " << name << endl;
        cout << "Category: " << category << endl;
        cout << "Price: " << price << endl;
    }
};

class Product : public Item 
{
private:
    string expirationDate;

public:
    Product(const string& name, const string& expirationDate, const string& category, double price)
        : Item(name, category, price), expirationDate(expirationDate) {}

    string getExpirationDate() const { return expirationDate; }

    void print() const override 
    {
        Item::print();
        cout << "Expiration Date: " << expirationDate << endl;
    }
};

class NonFoodItem : public Item 
{
private:
    string expirationDate;

public:
    NonFoodItem(const string& name, const string& category, double price, const string& expirationDate)
        : Item(name, category, price), expirationDate(expirationDate) {}

    string getExpirationDate() const { return expirationDate; }

    void print() const override 
    {
        Item::print();
        cout << "Expiration Date: " << expirationDate << endl;
    }
};

class Supermarket 
{
private:
    vector<Item*> items;

    bool itemExists(const string& name) const 
    {
        for (const auto& item : items) 
        {
            if (item->getName() == name) 
            {
                return true;
            }
        }
        return false;
    }

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
        if (itemExists(item->getName())) 
        {
            cout << "Error: Item '" << item->getName() << "' already exists." << endl;
            delete item;
        }
        else 
        {
            items.push_back(item);
        }
    }

    void addItem(const string& name, const string& category, double price, const string& expirationDate) 
    {
        if (itemExists(name)) 
        {
            cout << "Error: Item '" << name << "' already exists." << endl;
        }
        else 
        {
            if (category == "dairy" || category == "bakery") 
            {
                addItem(new Product(name, expirationDate, category, price));
            }
            else 
            {
                addItem(new NonFoodItem(name, category, price, expirationDate));
            }
        }
    }

    void saveToFile(const string& filename) const 
    {
        ofstream file(filename);
        for (const auto& item : items) 
        {
            file << item->getName() << "," << item->getCategory() << ","
                << item->getPrice();
            if (auto product = dynamic_cast<Product*>(item)) 
            {
                file << "," << product->getExpirationDate();
            }
            else if (auto nonFood = dynamic_cast<NonFoodItem*>(item)) 
            {
                file << "," << nonFood->getExpirationDate();
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
        while (getline(file, line)) 
        {
            stringstream ss(line);
            getline(ss, name, ',');
            getline(ss, category, ',');
            ss >> price;
            ss.ignore(1, ',');
            getline(ss, expirationDate);
            addItem(name, category, price, expirationDate);
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
            cout << endl;
        }
    }

    void viewAllItems() const 
    {
        cout << "All items:" << endl;
        for (const auto& item : items) 
        {
            cout << "Name: " << item->getName() << ", Category: " << item->getCategory() << ", Price: " << item->getPrice();
            if (auto product = dynamic_cast<const Product*>(item)) 
            {
                cout << ", Expiration Date: " << product->getExpirationDate();
            }
            else if (auto nonFood = dynamic_cast<const NonFoodItem*>(item)) 
            {
                cout << ", Expiration Date: " << nonFood->getExpirationDate();
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
            for (auto i = it; i != items.end(); ++i) 
            {
                delete* i;
            }
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
        for (const auto& itemToSell : itemsToSell) 
        {
            const string& name = itemToSell.first;
            const int& quantity = itemToSell.second;

            bool itemFound = false;
            for (const auto& item : items) 
            {
                if (item->getName() == name) 
                {
                    itemFound = true;
                    totalCost += item->getPrice() * quantity;
                    cout << "Item '" << name << "' sold successfully." << endl;
                    break;
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

    supermarket.addItem(new Product("Milk", "2024-06-10", "dairy products", 1.5));
    supermarket.addItem(new Product("Bread", "2024-06-08", "bakery", 1.2));
    supermarket.addItem(new NonFoodItem("Soap", "household goods", 0.8, "2025-06-01"));
    supermarket.addItem(new Product("Eggs", "2024-06-12", "dairy products", 2.0));
    supermarket.addItem(new Product("Cheese", "2024-06-15", "dairy products", 3.0));
    supermarket.addItem(new NonFoodItem("Shampoo", "household goods", 1.5, "2025-07-01"));
    supermarket.addItem(new Product("Coffee", "2024-06-20", "beverages", 2.5));

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
            getline(cin, name);
            cout << "Enter category: ";
            getline(cin, category);
            cout << "Enter price: ";
            cin >> price;
            cin.ignore();
            cout << "Enter expiration date: ";
            getline(cin, expirationDate);
            supermarket.addItem(name, category, price, expirationDate);
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
            getline(cin, name);
            supermarket.searchItem(name);
            break;
        }
        case 7: 
        {
            string name;
            cout << "Enter item name to delete: ";
            getline(cin, name);
            supermarket.deleteItem(name);
            break;
        }
        case 8: 
        {
            vector<pair<string, int>> itemsToSell;
            string name;
            int quantity;
            char response;
            do {
                cout << "Enter item name: ";
                getline(cin, name);
                cout << "Enter quantity: ";
                cin >> quantity;
                cin.ignore(); 
                itemsToSell.push_back({ name, quantity });
                cout << "Add another item? (y/n): ";
                cin >> response;
                cin.ignore();  
            } while (response == 'y');
            supermarket.sellItems(itemsToSell);
            break;
        }
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }
}
