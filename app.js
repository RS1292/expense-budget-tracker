// Expense Tracker Logic

class Expense {
  constructor(date, category, amount, note) {
    this.date = date;
    this.category = category;
    this.amount = amount;
    this.note = note;
  }
}

class BudgetManager {
  constructor() {
    this.expenses = [];
    this.budget = 0;
    this.loadFromStorage();
  }
  
  addExpense(date, category, amount, note) {
    // Validation
    if (!date || !date.match(/^\d{2}-\d{2}$/)) {
      return { success: false, msg: "Date must be in DD-MM format" };
    }
    if (!category) {
      return { success: false, msg: "Category is required" };
    }
    if (amount <= 0) {
      return { success: false, msg: "Amount must be positive" };
    }
    
    this.expenses.push(new Expense(date, category, amount, note || ""));
    this.saveToStorage();
    
    // Check if exceeded
    const [dd, mm] = date.split('-');
    const total = this.getMonthlyTotal(mm);
    let msg = "Expense added successfully!";
    if (total > this.budget && this.budget > 0) {
      msg += " ⚠️ WARNING: Monthly spending exceeds budget!";
    }
    return { success: true, msg };
  }
  
  getAllExpenses() {
    return this.expenses.sort((a, b) => {
      const [ad, am] = a.date.split('-');
      const [bd, bm] = b.date.split('-');
      if (parseInt(am) !== parseInt(bm)) return parseInt(bm) - parseInt(am);
      return parseInt(bd) - parseInt(ad);
    });
  }
  
  searchByCategory(category) {
    if (!category) return this.expenses;
    return this.expenses.filter(e => e.category === category);
  }
  
  searchByDate(date) {
    return this.expenses.filter(e => e.date === date);
  }
  
  getMonthlyTotal(month) {
    return this.expenses
      .filter(e => e.date.endsWith('-' + month))
      .reduce((sum, e) => sum + e.amount, 0);
  }
  
  getCategoryTotals() {
    const totals = {};
    this.expenses.forEach(e => {
      totals[e.category] = (totals[e.category] || 0) + e.amount;
    });
    return totals;
  }
  
  getCurrentMonthTotal() {
    const today = new Date();
    const month = String(today.getMonth() + 1).padStart(2, '0');
    return this.getMonthlyTotal(month);
  }
  
  setBudget(amount) {
    this.budget = amount;
    this.saveToStorage();
  }
  
  saveToStorage() {
    localStorage.setItem('expenses', JSON.stringify(this.expenses));
    localStorage.setItem('budget', this.budget);
  }
  
  loadFromStorage() {
    const stored = localStorage.getItem('expenses');
    if (stored) {
      try {
        this.expenses = JSON.parse(stored);
      } catch(e) {
        this.expenses = [];
      }
    }
    const budget = localStorage.getItem('budget');
    if (budget) {
      const parsed = parseFloat(budget);
      if (isNaN(parsed) || parsed < 0 || parsed > 999999) {
        localStorage.removeItem('budget');
        localStorage.removeItem('expenses');
        this.budget = 0;
        this.expenses = [];
      } else {
        this.budget = parsed;
      }
    }
  }
  
  clearAllData() {
    localStorage.clear();
    this.budget = 0;
    this.expenses = [];
  }
}

const budgetManager = new BudgetManager();

// UI Functions
function showMessage(elementId, text, type = 'success') {
  const el = document.getElementById(elementId);
  el.innerHTML = `<div class="message ${type}">${text}</div>`;
  setTimeout(() => { el.innerHTML = ''; }, 4000);
}

function updateBudgetDisplay() {
  if (budgetManager.budget <= 0 || budgetManager.budget > 999999 || isNaN(budgetManager.budget)) {
    document.getElementById('budgetStatus').style.display = 'none';
    return;
  }
  
  document.getElementById('budgetStatus').style.display = 'block';
  const used = budgetManager.getCurrentMonthTotal();
  const total = budgetManager.budget;
  const percentage = Math.min((used / total) * 100, 100);
  const barInner = document.getElementById('budgetBarInner');
  
  barInner.style.width = percentage + '%';
  if (used > total) {
    barInner.classList.add('exceeded');
  } else {
    barInner.classList.remove('exceeded');
  }
  
  document.getElementById('budgetUsed').textContent = '$' + used.toFixed(2);
  document.getElementById('budgetTotal').textContent = '$' + total.toFixed(2);
}

function setBudget() {
  const val = parseFloat(document.getElementById('budgetInput').value);
  if (!val || isNaN(val) || val <= 0) {
    showMessage('budgetMessage', 'Please enter a valid positive budget', 'error');
    return;
  }
  if (val > 999999) {
    showMessage('budgetMessage', 'Budget value too large', 'error');
    return;
  }
  budgetManager.setBudget(val);
  updateBudgetDisplay();
  showMessage('budgetMessage', `Budget set to $${val.toFixed(2)}`, 'success');
}

function clearAllData() {
  if (confirm('Are you sure you want to clear all data? This cannot be undone.')) {
    budgetManager.clearAllData();
    document.getElementById('budgetInput').value = '';
    document.getElementById('output').innerHTML = '';
    document.getElementById('output2').innerHTML = '';
    document.getElementById('output3').innerHTML = '';
    document.getElementById('output4').innerHTML = '';
    updateBudgetDisplay();
    showMessage('budgetMessage', 'All data cleared!', 'success');
  }
}

function addExpense() {
  const date = document.getElementById('dateInput').value.trim();
  const category = document.getElementById('categoryInput').value;
  const amount = parseFloat(document.getElementById('amountInput').value);
  const note = document.getElementById('noteInput').value.trim();
  
  const result = budgetManager.addExpense(date, category, amount, note);
  
  if (result.success) {
    showMessage('addMessage', result.msg, result.msg.includes('WARNING') ? 'warning' : 'success');
    document.getElementById('dateInput').value = '';
    document.getElementById('categoryInput').value = '';
    document.getElementById('amountInput').value = '';
    document.getElementById('noteInput').value = '';
    updateBudgetDisplay();
    displayAllExpenses();
  } else {
    showMessage('addMessage', result.msg, 'error');
  }
}

function formatTable(expenses) {
  if (expenses.length === 0) return 'No expenses found.';
  
  let html = '<table class="table"><thead><tr><th>Date</th><th>Category</th><th>Amount</th><th>Note</th></tr></thead><tbody>';
  expenses.forEach(e => {
    html += `<tr><td>${e.date}</td><td class="category">${e.category}</td><td class="amount">$${e.amount.toFixed(2)}</td><td>${e.note}</td></tr>`;
  });
  html += '</tbody></table>';
  return html;
}

function displayAllExpenses() {
  const expenses = budgetManager.getAllExpenses();
  document.getElementById('output').innerHTML = formatTable(expenses);
}

function searchByCategory() {
  const category = document.getElementById('searchCategory').value;
  const expenses = budgetManager.searchByCategory(category);
  const title = category ? `Expenses in "${category}"` : 'All Expenses';
  document.getElementById('output2').innerHTML = `<strong>${title}</strong><br>` + formatTable(expenses);
}

function searchByDate() {
  const date = document.getElementById('searchDate').value.trim();
  if (!date) {
    showMessage('output3', 'Please enter a date in DD-MM format', 'error');
    return;
  }
  const expenses = budgetManager.searchByDate(date);
  document.getElementById('output3').innerHTML = `<strong>Expenses on ${date}</strong><br>` + formatTable(expenses);
}

function showMonthlyReport() {
  const month = document.getElementById('reportMonth').value.trim();
  if (!month || !month.match(/^\d{2}$/) || month < '01' || month > '12') {
    showMessage('output4', 'Please enter month in MM format (01-12)', 'error');
    return;
  }
  
  const total = budgetManager.getMonthlyTotal(month);
  const budget = budgetManager.budget;
  const exceeded = total > budget && budget > 0;
  
  let html = `<strong>Monthly Report - Month ${month}</strong><br><br>`;
  html += `Total Expenses: <span class="amount">$${total.toFixed(2)}</span><br>`;
  html += `Budget: <span class="amount">$${budget.toFixed(2)}</span><br>`;
  if (budget > 0) {
    const remaining = budget - total;
    const status = exceeded ? 'EXCEEDED' : 'REMAINING';
    const statusColor = exceeded ? 'ff6b6b' : '4ec9b0';
    html += `<span style="color: #${statusColor};">${status}: $${Math.abs(remaining).toFixed(2)}</span>`;
  }
  
  document.getElementById('output4').innerHTML = html;
}

function showCategoryReport() {
  const catTotals = budgetManager.getCategoryTotals();
  const sorted = Object.entries(catTotals).sort((a, b) => b[1] - a[1]);
  
  let html = '<strong>Category-wise Breakdown</strong><br><br>';
  if (sorted.length === 0) {
    html += 'No expenses recorded.';
  } else {
    html += '<table class="table"><thead><tr><th>Category</th><th>Total</th></tr></thead><tbody>';
    sorted.forEach(([cat, total]) => {
      html += `<tr><td class="category">${cat}</td><td class="amount">$${total.toFixed(2)}</td></tr>`;
    });
    html += '</tbody></table>';
  }
  
  document.getElementById('output4').innerHTML = html;
}

function viewTab(tab, ev) {
  // Hide all tabs
  document.getElementById('allTab').style.display = 'none';
  document.getElementById('categoryTab').style.display = 'none';
  document.getElementById('dateTab').style.display = 'none';
  document.getElementById('reportsTab').style.display = 'none';
  
  // Clear outputs
  document.getElementById('output').innerHTML = '';
  document.getElementById('output2').innerHTML = '';
  document.getElementById('output3').innerHTML = '';
  document.getElementById('output4').innerHTML = '';
  
  // Update buttons
  document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
  if (ev && ev.currentTarget) {
    ev.currentTarget.classList.add('active');
  }
  
  // Show selected tab
  if (tab === 'all') {
    document.getElementById('allTab').style.display = 'block';
    displayAllExpenses();
  } else if (tab === 'category') {
    document.getElementById('categoryTab').style.display = 'block';
  } else if (tab === 'date') {
    document.getElementById('dateTab').style.display = 'block';
  } else if (tab === 'reports') {
    document.getElementById('reportsTab').style.display = 'block';
    // FIX: Auto-display current month report when Reports tab is clicked
    const today = new Date();
    const month = String(today.getMonth() + 1).padStart(2, '0');
    document.getElementById('reportMonth').value = month;
    showMonthlyReport();
  }
}

// Initialize on load
window.addEventListener('load', () => {
  updateBudgetDisplay();
  if (budgetManager.budget > 0) {
    document.getElementById('budgetInput').value = budgetManager.budget;
  }
  displayAllExpenses();
});
