#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <map>

using namespace std;

class Expense {
public:
    string date; // DD-MM
    string category;
    double amount;
    string note;

    Expense(string d, string c, double a, string n) : date(d), category(c), amount(a), note(n) {}
};

class BudgetManager {
private:
    vector<Expense> expenses;
    double budget;
    string filename;
    vector<string> categories = {"Food", "Transport", "Entertainment", "Utilities", "Other"};

public:
    BudgetManager(double b, string f) : budget(b), filename(f) { loadFromFile(); }
    ~BudgetManager() { saveToFile(); }

    bool addExpense(string date, string cat, double amt, string note) {
        if (amt <= 0) {
            cout << "Amount must be positive.\n";
            return false;
        }
        if (cat.empty() || (find(categories.begin(), categories.end(), cat) == categories.end() && cat != "Other")) {
            cout << "Invalid category. Must be from: Food, Transport, Entertainment, Utilities, Other\n";
            return false;
        }
        expenses.push_back(Expense(date, cat, amt, note));
        string month = date.substr(3, 2);
        double total = getMonthlyTotal(month);
        if (total > budget) {
            cout << "Warning: Monthly expense exceeds budget!\n";
        }
        return true;
    }

    void viewAllExpenses() {
        if (expenses.empty()) {
            cout << "No expenses recorded.\n";
            return;
        }
        cout << left << setw(10) << "Date" << setw(15) << "Category" << setw(10) << "Amount" << "Note" << endl;
        cout << string(50, '-') << endl;
        for (auto& e : expenses) {
            cout << left << setw(10) << e.date << setw(15) << e.category << setw(10) << fixed << setprecision(2) << e.amount << e.note << endl;
        }
    }

    void searchByCategory(string cat) {
        vector<Expense> res;
        for (auto& e : expenses) {
            if (e.category == cat) res.push_back(e);
        }
        if (res.empty()) {
            cout << "No expenses found in category: " << cat << endl;
            return;
        }
        cout << "Expenses in category: " << cat << endl;
        cout << left << setw(10) << "Date" << setw(15) << "Category" << setw(10) << "Amount" << "Note" << endl;
        cout << string(50, '-') << endl;
        for (auto& e : res) {
            cout << left << setw(10) << e.date << setw(15) << e.category << setw(10) << fixed << setprecision(2) << e.amount << e.note << endl;
        }
    }

    void searchByDate(string date) {
        vector<Expense> res;
        for (auto& e : expenses) {
            if (e.date == date) res.push_back(e);
        }
        if (res.empty()) {
            cout << "No expenses found on date: " << date << endl;
            return;
        }
        cout << "Expenses on date: " << date << endl;
        cout << left << setw(10) << "Date" << setw(15) << "Category" << setw(10) << "Amount" << "Note" << endl;
        cout << string(50, '-') << endl;
        for (auto& e : res) {
            cout << left << setw(10) << e.date << setw(15) << e.category << setw(10) << fixed << setprecision(2) << e.amount << e.note << endl;
        }
    }

    double getMonthlyTotal(string month) {
        double total = 0;
        for (auto& e : expenses) {
            if (e.date.substr(3, 2) == month) total += e.amount;
        }
        return total;
    }

    void showMonthlyTotal(string month) {
        double total = getMonthlyTotal(month);
        cout << "Monthly total for " << month << ": " << fixed << setprecision(2) << total << endl;
        if (total > budget) {
            cout << "Warning: Exceeds budget of " << budget << endl;
        }
    }

    void showCategoryWiseTotal() {
        map<string, double> totals;
        for (auto& e : expenses) {
            totals[e.category] += e.amount;
        }
        cout << "Category-wise totals:" << endl;
        for (auto& p : totals) {
            cout << left << setw(15) << p.first << ": " << fixed << setprecision(2) << p.second << endl;
        }
    }

    void saveToFile() {
        ofstream out(filename);
        if (!out) {
            cout << "Error saving to file.\n";
            return;
        }
        out << budget << endl;
        for (auto& e : expenses) {
            out << e.date << "," << e.category << "," << e.amount << "," << e.note << endl;
        }
    }

    void loadFromFile() {
        ifstream in(filename);
        if (!in) return;
        string line;
        getline(in, line);
        if (!line.empty()) budget = stod(line);
        while (getline(in, line)) {
            size_t p1 = line.find(',');
            if (p1 == string::npos) continue;
            size_t p2 = line.find(',', p1 + 1);
            if (p2 == string::npos) continue;
            size_t p3 = line.find(',', p2 + 1);
            if (p3 == string::npos) continue;
            string d = line.substr(0, p1);
            string c = line.substr(p1 + 1, p2 - p1 - 1);
            string a_str = line.substr(p2 + 1, p3 - p2 - 1);
            double a = stod(a_str);
            string n = line.substr(p3 + 1);
            expenses.push_back(Expense(d, c, a, n));
        }
    }
};

int main() {
    double budget;
    cout << "Enter monthly budget: ";
    cin >> budget;
    BudgetManager bm(budget, "expenses.txt");

    int choice;
    do {
        cout << "\nMonthly Money Coach\n";
        cout << "1. Add Expense\n";
        cout << "2. View All Expenses\n";
        cout << "3. Search by Category\n";
        cout << "4. Search by Date\n";
        cout << "5. Monthly Total\n";
        cout << "6. Category-wise Total\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            string date, cat, note;
            double amt;
            cout << "Date (DD-MM): ";
            cin >> date;
            cout << "Category: ";
            cin >> cat;
            cout << "Amount: ";
            cin >> amt;
            cout << "Note: ";
            cin.ignore();
            getline(cin, note);
            bm.addExpense(date, cat, amt, note);
        } else if (choice == 2) {
            bm.viewAllExpenses();
        } else if (choice == 3) {
            string cat;
            cout << "Category: ";
            cin >> cat;
            bm.searchByCategory(cat);
        } else if (choice == 4) {
            string date;
            cout << "Date (DD-MM): ";
            cin >> date;
            bm.searchByDate(date);
        } else if (choice == 5) {
            string month;
            cout << "Month (MM): ";
            cin >> month;
            bm.showMonthlyTotal(month);
        } else if (choice == 6) {
            bm.showCategoryWiseTotal();
        }
    } while (choice != 7);

    return 0;
}