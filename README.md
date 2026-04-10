# 💰 Monthly Money Coach - Expense Tracker

## Project Overview
A comprehensive expense and budget tracking application built to help working professionals manage their monthly spending and stay within budget limits.

## Features
✅ **Add Expense** - Record expenses with date (DD-MM), category, amount, and note  
✅ **View All Expenses** - Display all recorded expenses in a formatted table  
✅ **Search by Category** - Filter expenses by category (Food, Transport, Entertainment, Utilities, Other)  
✅ **Search by Date** - Find expenses on a specific date  
✅ **Monthly Report** - View total spending for a specific month  
✅ **Category-wise Report** - Breakdown of spending by category  
✅ **Budget Warnings** - Alert when monthly expenses exceed the set budget  
✅ **Data Persistence** - All data is saved automatically (browser localStorage for web, file storage for C++)  

## Project Structure

### Web Version (Recommended for hosting)
- **`expense-tracker.html`** - Main UI with tabbed interface
- **`expense-tracker.js`** - Business logic with `Expense` and `BudgetManager` classes
- **Live Demo**: https://rs1292.github.io/expense-budget-tracker/

### C++ Version (Console-based)
- **`expense_tracker.cpp`** - Full OOP implementation using C++
  - `Expense` class - Data structure for expenses
  - `BudgetManager` class - Manages vector of expenses + file I/O
- **`expense_tracker.exe`** - Compiled executable

## How to Use

### Web Version
1. Open https://rs1292.github.io/expense-budget-tracker/
2. Set your monthly budget in "Budget Setup" section
3. Add expenses using the form
4. View, search, and generate reports using the tabs

### C++ Console Version
```bash
g++ expense_tracker.cpp -o expense_tracker
./expense_tracker
```

## Validation Rules
- ✓ Date must be in DD-MM format
- ✓ Category must be from predefined list or "Other"
- ✓ Amount must be positive (> 0)
- ✓ All fields required before adding expense

## Budget Warnings
- If monthly total exceeds budget, user receives warning
- Visual budget bar shows spending percentage
- Color changes to red when exceeded

## Data Storage
- **Web**: Browser localStorage (persists across sessions)
- **C++**: File-based storage (`expenses.txt`)

## OOP Design
Both versions follow object-oriented principles:

```cpp
class Expense {
  - date, category, amount, note
};

class BudgetManager {
  - vector<Expense>
  - budget limit
  - addExpense(), searchByCategory(), getMonthlyTotal(), ...
};
```

## Example Workflow
1. Set budget to $2000
2. Add expense: 15-04, Food, $45.50, "Dinner"
3. Add expense: 16-04, Transport, $25.00, "Uber"
4. View all expenses in table format
5. Check Category-wise breakdown
6. Get warning if total exceeds $2000

## Future Enhancements
- Export reports to CSV
- Email notifications
- Mobile app version
- Chart visualizations
- Multi-user support

---
**Author**: Student  
**Date**: April 2026  
**Assignment**: DA2 Mini Project - Question 5
