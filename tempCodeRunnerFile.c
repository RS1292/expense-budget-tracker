/*
 * Inventory Management System with Low-Stock Alert
 * DA1 Mini Project - Question 3
 * Emscripten/WASM compatible version
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten.h>

#define MAX_PRODUCTS 100
#define LOW_STOCK_THRESHOLD 5
#define FILENAME "/data/inventory.dat"

/* ===================== STRUCT DEFINITION ===================== */
typedef struct {
    int   productID;
    char  name[50];
    float price;
    int   quantity;
    int   active;
} Product;

/* ===================== GLOBAL DATA ===================== */
Product inventory[MAX_PRODUCTS];
int productCount = 0;

/* ===================== STATE MACHINE ===================== */
typedef enum {
    STATE_MENU = 0,
    STATE_ADD_ID, STATE_ADD_NAME, STATE_ADD_PRICE, STATE_ADD_QTY,
    STATE_UPD_ID, STATE_UPD_TYPE, STATE_UPD_QTY,
    STATE_DEL_ID, STATE_DEL_CONFIRM,
    STATE_SEARCH_ID
} AppState;

AppState currentState = STATE_MENU;
Product  tempProduct;
int      tempID, tempType;

/* ===================== FUNCTION PROTOTYPES ===================== */
void displayMenu(void);
void displayAllProducts(void);
void lowStockReport(void);
void showTotalInventoryValue(void);
void saveToFile(void);
void loadFromFile(void);
int  findProductByID(int id);
int  isIDUnique(int id);

/* ===================== DISPLAY MENU ===================== */
void displayMenu(void) {
    printf("\n========================================\n");
    printf("   INVENTORY MANAGEMENT SYSTEM\n");
    printf("========================================\n");
    printf("  1. Add New Product\n");
    printf("  2. Update Stock (Purchase / Sale)\n");
    printf("  3. Delete Product\n");
    printf("  4. Search Product by ID\n");
    printf("  5. Display All Products\n");
    printf("  6. Low-Stock Report (below %d units)\n", LOW_STOCK_THRESHOLD);
    printf("  7. Total Inventory Value\n");
    printf("  0. Save & Exit\n");
    printf("========================================\n");
    printf("Enter your choice: ");
    fflush(stdout);
}

/* ===================== DISPLAY ALL ===================== */
void displayAllProducts(void) {
    printf("\n--- All Products ---\n");
    printf("%-6s %-25s %10s %10s %s\n","ID","Name","Price","Quantity","Status");
    printf("%-6s %-25s %10s %10s %s\n","------","-------------------------","----------","----------","------");
    int found = 0;
    for (int i = 0; i < productCount; i++) {
        if (!inventory[i].active) continue;
        found = 1;
        const char *st = (inventory[i].quantity < LOW_STOCK_THRESHOLD) ? "[LOW]" : "OK";
        printf("%-6d %-25s %10.2f %10d %s\n",
               inventory[i].productID, inventory[i].name,
               inventory[i].price, inventory[i].quantity, st);
    }
    if (!found) printf("  (No products)\n");
}

/* ===================== LOW STOCK ===================== */
void lowStockReport(void) {
    printf("\n--- Low-Stock Report (Threshold: %d) ---\n", LOW_STOCK_THRESHOLD);
    int found = 0;
    for (int i = 0; i < productCount; i++) {
        if (!inventory[i].active) continue;
        if (inventory[i].quantity < LOW_STOCK_THRESHOLD) {
            printf("  ID:%-6d | %-25s | Qty: %d  <-- REORDER\n",
                   inventory[i].productID, inventory[i].name, inventory[i].quantity);
            found = 1;
        }
    }
    if (!found) printf("  All products adequately stocked.\n");
}

/* ===================== INVENTORY VALUE ===================== */
void showTotalInventoryValue(void) {
    float total = 0;
    printf("\n--- Inventory Value ---\n");
    printf("%-6s %-25s %10s %8s %12s\n","ID","Name","Price","Qty","Value");
    printf("%-6s %-25s %10s %8s %12s\n","------","-------------------------","----------","--------","------------");
    for (int i = 0; i < productCount; i++) {
        if (!inventory[i].active) continue;
        float val = inventory[i].price * inventory[i].quantity;
        total += val;
        printf("%-6d %-25s %10.2f %8d %12.2f\n",
               inventory[i].productID, inventory[i].name,
               inventory[i].price, inventory[i].quantity, val);
    }
    printf("----------------------------------------------------------\n");
    printf("  TOTAL INVENTORY VALUE = %.2f\n", total);
}

/* ===================== FILE I/O ===================== */
void saveToFile(void) {
    FILE *fp = fopen(FILENAME, "wb");
    if (!fp) { printf("[!] Could not save.\n"); return; }
    fwrite(&productCount, sizeof(int), 1, fp);
    fwrite(inventory, sizeof(Product), productCount, fp);
    fclose(fp);
    /* sync to IndexedDB so data persists in browser */
    EM_ASM( FS.syncfs(false, function(err){}); );
    printf("[i] Data saved.\n");
}

void loadFromFile(void) {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) { productCount = 0; return; }
    fread(&productCount, sizeof(int), 1, fp);
    if (productCount < 0 || productCount > MAX_PRODUCTS) productCount = 0;
    else fread(inventory, sizeof(Product), productCount, fp);
    fclose(fp);
    if (productCount > 0)
        printf("[i] Loaded %d product(s) from storage.\n", productCount);
}

/* ===================== HELPERS ===================== */
int findProductByID(int id) {
    for (int i = 0; i < productCount; i++)
        if (inventory[i].active && inventory[i].productID == id) return i;
    return -1;
}

int isIDUnique(int id) {
    for (int i = 0; i < productCount; i++)
        if (inventory[i].active && inventory[i].productID == id) return 0;
    return 1;
}

/* ===================== INPUT HANDLER (called from JS) ===================== */
EMSCRIPTEN_KEEPALIVE
void handleInput(const char *input) {

    switch (currentState) {

        /* ---- MENU ---- */
        case STATE_MENU: {
            int choice = atoi(input);
            switch (choice) {
                case 1:
                    if (productCount >= MAX_PRODUCTS) {
                        printf("[!] Inventory full.\n");
                        displayMenu(); break;
                    }
                    memset(&tempProduct, 0, sizeof(tempProduct));
                    tempProduct.active = 1;
                    printf("Enter Product ID (positive number): ");
                    fflush(stdout);
                    currentState = STATE_ADD_ID;
                    break;
                case 2:
                    printf("Enter Product ID to update stock: ");
                    fflush(stdout);
                    currentState = STATE_UPD_ID;
                    break;
                case 3:
                    printf("Enter Product ID to delete: ");
                    fflush(stdout);
                    currentState = STATE_DEL_ID;
                    break;
                case 4:
                    printf("Enter Product ID to search: ");
                    fflush(stdout);
                    currentState = STATE_SEARCH_ID;
                    break;
                case 5: displayAllProducts();     displayMenu(); break;
                case 6: lowStockReport();         displayMenu(); break;
                case 7: showTotalInventoryValue(); displayMenu(); break;
                case 0:
                    saveToFile();
                    printf("\nGoodbye!\n");
                    break;
                default:
                    printf("[!] Invalid choice. Enter 0-7.\n");
                    displayMenu();
            }
            break;
        }

        /* ---- ADD: ID ---- */
        case STATE_ADD_ID: {
            int id = atoi(input);
            if (id <= 0) {
                printf("[!] ID must be a positive number. Try again: ");
                fflush(stdout); break;
            }
            if (!isIDUnique(id)) {
                printf("[!] ID %d already exists. Try again: ", id);
                fflush(stdout); break;
            }
            tempProduct.productID = id;
            printf("Enter Product Name: ");
            fflush(stdout);
            currentState = STATE_ADD_NAME;
            break;
        }
        /* ---- ADD: NAME ---- */
        case STATE_ADD_NAME: {
            if (strlen(input) == 0) {
                printf("[!] Name cannot be empty. Enter name: ");
                fflush(stdout); break;
            }
            strncpy(tempProduct.name, input, 49);
            printf("Enter Price (e.g. 25.50): ");
            fflush(stdout);
            currentState = STATE_ADD_PRICE;
            break;
        }
        /* ---- ADD: PRICE ---- */
        case STATE_ADD_PRICE: {
            float p = atof(input);
            if (p < 0) {
                printf("[!] Price cannot be negative. Enter price: ");
                fflush(stdout); break;
            }
            tempProduct.price = p;
            printf("Enter Quantity: ");
            fflush(stdout);
            currentState = STATE_ADD_QTY;
            break;
        }
        /* ---- ADD: QTY ---- */
        case STATE_ADD_QTY: {
            int q = atoi(input);
            if (q < 0) {
                printf("[!] Quantity cannot be negative. Enter quantity: ");
                fflush(stdout); break;
            }
            tempProduct.quantity = q;
            inventory[productCount++] = tempProduct;
            saveToFile();
            printf("[+] Product '%s' (ID:%d) added successfully!\n",
                   tempProduct.name, tempProduct.productID);
            currentState = STATE_MENU;
            displayMenu();
            break;
        }

        /* ---- UPDATE: ID ---- */
        case STATE_UPD_ID: {
            tempID = atoi(input);
            int idx = findProductByID(tempID);
            if (idx == -1) {
                printf("[!] ID %d not found. Try again: ", tempID);
                fflush(stdout); break;
            }
            printf("Product: %s | Current Qty: %d\n",
                   inventory[idx].name, inventory[idx].quantity);
            printf("1. Purchase (add stock)  2. Sale (reduce stock)\nChoose: ");
            fflush(stdout);
            currentState = STATE_UPD_TYPE;
            break;
        }
        /* ---- UPDATE: TYPE ---- */
        case STATE_UPD_TYPE: {
            tempType = atoi(input);
            if (tempType != 1 && tempType != 2) {
                printf("[!] Enter 1 or 2: ");
                fflush(stdout); break;
            }
            printf("Enter quantity (>0): ");
            fflush(stdout);
            currentState = STATE_UPD_QTY;
            break;
        }
        /* ---- UPDATE: QTY ---- */
        case STATE_UPD_QTY: {
            int qty = atoi(input);
            if (qty <= 0) {
                printf("[!] Must be > 0. Enter quantity: ");
                fflush(stdout); break;
            }
            int idx = findProductByID(tempID);
            if (tempType == 1) {
                inventory[idx].quantity += qty;
                printf("[+] Stock added. New quantity: %d\n", inventory[idx].quantity);
            } else {
                if (qty > inventory[idx].quantity) {
                    printf("[!] Insufficient stock! Available: %d\n", inventory[idx].quantity);
                } else {
                    inventory[idx].quantity -= qty;
                    printf("[+] Sale recorded. New quantity: %d\n", inventory[idx].quantity);
                    if (inventory[idx].quantity < LOW_STOCK_THRESHOLD)
                        printf("[!] WARNING: '%s' is low on stock!\n", inventory[idx].name);
                }
            }
            saveToFile();
            currentState = STATE_MENU;
            displayMenu();
            break;
        }

        /* ---- DELETE: ID ---- */
        case STATE_DEL_ID: {
            tempID = atoi(input);
            int idx = findProductByID(tempID);
            if (idx == -1) {
                printf("[!] ID %d not found. Try again: ", tempID);
                fflush(stdout); break;
            }
            printf("Delete '%s'? Enter 1=Yes / 0=No: ", inventory[idx].name);
            fflush(stdout);
            currentState = STATE_DEL_CONFIRM;
            break;
        }
        /* ---- DELETE: CONFIRM ---- */
        case STATE_DEL_CONFIRM: {
            int c = atoi(input);
            if (c == 1) {
                int idx = findProductByID(tempID);
                printf("[-] Product '%s' deleted.\n", inventory[idx].name);
                inventory[idx].active = 0;
                saveToFile();
            } else {
                printf("Delete cancelled.\n");
            }
            currentState = STATE_MENU;
            displayMenu();
            break;
        }

        /* ---- SEARCH ---- */
        case STATE_SEARCH_ID: {
            int id = atoi(input);
            int idx = findProductByID(id);
            if (idx == -1) {
                printf("[!] Product ID %d not found.\n", id);
            } else {
                printf("\n--- Product Details ---\n");
                printf("  ID       : %d\n",   inventory[idx].productID);
                printf("  Name     : %s\n",   inventory[idx].name);
                printf("  Price    : %.2f\n", inventory[idx].price);
                printf("  Quantity : %d\n",   inventory[idx].quantity);
                printf("  Status   : %s\n",
                    inventory[idx].quantity < LOW_STOCK_THRESHOLD ? "LOW STOCK" : "OK");
            }
            currentState = STATE_MENU;
            displayMenu();
            break;
        }

        default:
            currentState = STATE_MENU;
            displayMenu();
    }
}

/* ===================== STARTUP ===================== */
EMSCRIPTEN_KEEPALIVE
void afterFSReady(void) {
    loadFromFile();
    displayMenu();
}

int main(void) {
    EM_ASM(
        FS.mkdir('/data');
        FS.mount(IDBFS, {}, '/data');
        FS.syncfs(true, function(err) {
            ccall('afterFSReady', null, [], []);
        });
    );
    return 0;
}
