/*
 * B1343013_w1.cpp
 * 多項式加減運算程式 — 使用鏈結串列 (Linked List) 實現
 *
 * ========== 輸入方式說明 ==========
 *
 * 程式啟動後，先選擇輸入模式：
 *   1 = 簡單版：依序輸入「係數 指數」配對，以空白分隔，一整行輸入
 *       範例：5 4 6.1 2 2.9 1 6 0
 *       代表多項式 5x^4 + 6.1x^2 + 2.9x + 6
 *
 *   2 = 困難版：直接輸入多項式表達式字串
 *       範例：5x4+6.1x2+2.9x+6   或   9x^5-3.2x^2-4x-5
 *       支援有無 ^ 符號，支援省略係數 1（如 x3 代表 1x^3）
 *
 * 接著輸入第二個多項式（格式同上）。
 * 最後輸入運算符號 + 或 -，程式即輸出運算結果。
 *
 * 項次數量不限，使用者可自由輸入任意多少項。
 * ================================
 */

#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cctype>

using namespace std;

// ===== 鏈結串列節點 =====
struct Node {
    double coeff;
    int    exp;
    Node*  next;
};

Node* createNode(double coeff, int exp) {
    Node* n = new Node;
    n->coeff = coeff;
    n->exp   = exp;
    n->next  = nullptr;
    return n;
}

// 將一項插入多項式鏈結串列（依指數遞減排序，同指數合併係數）
void insertTerm(Node*& head, double coeff, int exp) {
    if (fabs(coeff) < 1e-9) return;

    if (!head || exp > head->exp) {
        Node* n = createNode(coeff, exp);
        n->next = head;
        head = n;
        return;
    }

    if (exp == head->exp) {
        head->coeff += coeff;
        if (fabs(head->coeff) < 1e-9) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        return;
    }

    Node* cur = head;
    while (cur->next && cur->next->exp > exp)
        cur = cur->next;

    if (cur->next && cur->next->exp == exp) {
        cur->next->coeff += coeff;
        if (fabs(cur->next->coeff) < 1e-9) {
            Node* tmp = cur->next;
            cur->next = tmp->next;
            delete tmp;
        }
        return;
    }

    Node* n = createNode(coeff, exp);
    n->next  = cur->next;
    cur->next = n;
}

// ===== 簡單版解析 =====
// 格式："係數 指數 係數 指數 ..."，例如 "5 4 6.1 2 2.9 1 6 0"
Node* parseSimple(const string& line) {
    Node* head = nullptr;
    istringstream iss(line);
    double c;
    int e;
    while (iss >> c >> e)
        insertTerm(head, c, e);
    return head;
}

// ===== 困難版解析 =====
// 格式："5x4+6.1x2+2.9x+6" 或 "9x^5-3.2x^2-4x-5"
Node* parseDifficult(const string& raw) {
    Node* head = nullptr;

    string s;
    for (char c : raw)
        if (c != ' ') s += c;

    int i = 0, n = (int)s.length();

    while (i < n) {
        double sign = 1.0;
        if (s[i] == '+') { sign =  1.0; i++; }
        else if (s[i] == '-') { sign = -1.0; i++; }

        string numStr;
        while (i < n && (isdigit((unsigned char)s[i]) || s[i] == '.'))
            numStr += s[i++];

        bool hasNum = !numStr.empty();
        double coeff = hasNum ? stod(numStr) : 0.0;

        int exp;
        if (i < n && (s[i] == 'x' || s[i] == 'X')) {
            if (!hasNum) coeff = 1.0;
            i++;
            if (i < n && s[i] == '^') i++;

            string expStr;
            while (i < n && isdigit((unsigned char)s[i]))
                expStr += s[i++];
            exp = expStr.empty() ? 1 : stoi(expStr);
        } else {
            exp = 0;
        }

        insertTerm(head, sign * coeff, exp);
    }
    return head;
}

// ===== 格式化數字（去除多餘的尾零）=====
string formatNum(double val) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%.10g", fabs(val));
    return string(buf);
}

// ===== 輸出多項式 =====
void printPoly(Node* head) {
    if (!head) {
        cout << "0";
        return;
    }

    Node* cur = head;
    bool first = true;

    while (cur) {
        double c = cur->coeff;
        double ac = fabs(c);
        int e = cur->exp;

        if (first) {
            if (c < 0) cout << "-";
            if (e == 0) {
                cout << formatNum(ac);
            } else if (e == 1) {
                if (fabs(ac - 1.0) < 1e-9) cout << "x";
                else cout << formatNum(ac) << "x";
            } else {
                if (fabs(ac - 1.0) < 1e-9) cout << "x" << e;
                else cout << formatNum(ac) << "x" << e;
            }
            first = false;
        } else {
            cout << (c > 0 ? " + " : " - ");
            if (e == 0) {
                cout << formatNum(ac);
            } else if (e == 1) {
                if (fabs(ac - 1.0) < 1e-9) cout << "x";
                else cout << formatNum(ac) << "x";
            } else {
                if (fabs(ac - 1.0) < 1e-9) cout << "x" << e;
                else cout << formatNum(ac) << "x" << e;
            }
        }
        cur = cur->next;
    }
}

// ===== 多項式加法 / 減法 =====
Node* polyOperate(Node* a, Node* b, char op) {
    Node* result = nullptr;

    for (Node* cur = a; cur; cur = cur->next)
        insertTerm(result, cur->coeff, cur->exp);

    for (Node* cur = b; cur; cur = cur->next)
        insertTerm(result, (op == '-') ? -cur->coeff : cur->coeff, cur->exp);

    return result;
}

// ===== 釋放鏈結串列記憶體 =====
void freePoly(Node* head) {
    while (head) {
        Node* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// ===== 主程式 =====
int main() {
    int mode;
    cout << "選擇輸入模式 (1=簡單版, 2=困難版): ";
    cin >> mode;
    cin.ignore();

    string lineA, lineB;
    cout << "輸入A(x): ";
    getline(cin, lineA);
    cout << "輸入B(x): ";
    getline(cin, lineB);

    Node* A = nullptr;
    Node* B = nullptr;

    if (mode == 1) {
        A = parseSimple(lineA);
        B = parseSimple(lineB);
    } else {
        A = parseDifficult(lineA);
        B = parseDifficult(lineB);
    }

    char op;
    cout << "輸入運算(+/-): ";
    cin >> op;

    cout << endl;
    cout << "A(x) = ";
    printPoly(A);
    cout << endl;

    cout << "B(x) = ";
    printPoly(B);
    cout << endl;

    Node* result = polyOperate(A, B, op);

    cout << endl;
    cout << "A(x)" << op << "B(x) = ";
    printPoly(result);
    cout << endl;

    freePoly(A);
    freePoly(B);
    freePoly(result);

    return 0;
}
