/*
 * B1343013_w3.cpp
 * 題目：m元搜尋平衡B樹
 * 姓名：馮聖元
 * 學號：B1343013
 *
 * ========== 完成功能 ==========
 * A) 讀取 tree.dat 內的整數資料，建立一棵 m 元搜尋平衡 B 樹。
 *    m 由使用者輸入，範圍限制為 2 <= m <= 5。
 *    本程式將 m 視為 B 樹的階數：
 *      - 每個節點最多有 m 個子節點
 *      - 每個節點最多有 m - 1 筆資料
 *
 * B) 提供搜尋某個數值資料，並顯示 Found!! 或 Not Found!!。
 *
 * C) 提供選單，可新增、刪除、搜尋資料。新增與刪除後會維持 B 樹平衡。
 *
 * ========== 輸入操作 ==========
 * 1. 程式會先讀取目前資料夾下的 tree.dat。
 *    tree.dat 可用空白、換行或逗號分隔整數。
 *    範例：51, 70, 39, 45, 63, 80, 20, 42
 *
 * 2. 輸入 m，例如：3
 *
 * 3. 輸入第一次要搜尋的資料，例如：45
 *
 * 4. 進入選單：
 *    1 = insert，新增資料
 *    2 = delete，刪除資料
 *    3 = find，搜尋資料
 *    4 = exit，結束程式
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ===== B樹節點 =====
struct BNode {
    vector<int> key;
    vector<BNode*> child;
    bool leaf;

    BNode(bool isLeaf = true) {
        leaf = isLeaf;
    }
};

struct SplitResult {
    bool split;
    int middleKey;
    BNode* rightNode;

    SplitResult(bool s = false, int k = 0, BNode* r = nullptr) {
        split = s;
        middleKey = k;
        rightNode = r;
    }
};

// ===== m元搜尋平衡B樹 =====
class BTree {
private:
    int order;
    int maxKeyCount;
    BNode* root;
    vector<int> data;

    void clear(BNode* node) {
        if (!node) return;
        for (BNode* p : node->child)
            clear(p);
        delete node;
    }

    bool searchNode(BNode* node, int value) const {
        if (!node) return false;

        int i = 0;
        while (i < (int)node->key.size() && value > node->key[i])
            i++;

        if (i < (int)node->key.size() && value == node->key[i])
            return true;

        if (node->leaf)
            return false;

        return searchNode(node->child[i], value);
    }

    SplitResult splitNode(BNode* node) {
        int mid = (int)node->key.size() / 2;
        int middleKey = node->key[mid];

        BNode* right = new BNode(node->leaf);

        for (int i = mid + 1; i < (int)node->key.size(); i++)
            right->key.push_back(node->key[i]);

        node->key.resize(mid);

        if (!node->leaf) {
            for (int i = mid + 1; i < (int)node->child.size(); i++)
                right->child.push_back(node->child[i]);
            node->child.resize(mid + 1);
        }

        return SplitResult(true, middleKey, right);
    }

    SplitResult insertNode(BNode* node, int value) {
        int pos = 0;
        while (pos < (int)node->key.size() && value > node->key[pos])
            pos++;

        if (pos < (int)node->key.size() && value == node->key[pos])
            return SplitResult();

        if (node->leaf) {
            node->key.insert(node->key.begin() + pos, value);
        } else {
            SplitResult childResult = insertNode(node->child[pos], value);
            if (childResult.split) {
                node->key.insert(node->key.begin() + pos, childResult.middleKey);
                node->child.insert(node->child.begin() + pos + 1, childResult.rightNode);
            }
        }

        if ((int)node->key.size() > maxKeyCount)
            return splitNode(node);

        return SplitResult();
    }

    void insertToTreeOnly(int value) {
        if (!root) {
            root = new BNode(true);
            root->key.push_back(value);
            return;
        }

        SplitResult result = insertNode(root, value);
        if (result.split) {
            BNode* newRoot = new BNode(false);
            newRoot->key.push_back(result.middleKey);
            newRoot->child.push_back(root);
            newRoot->child.push_back(result.rightNode);
            root = newRoot;
        }
    }

    void rebuildTree() {
        clear(root);
        root = nullptr;

        for (int value : data)
            insertToTreeOnly(value);
    }

    void inorderNode(BNode* node, vector<int>& result) const {
        if (!node) return;

        int n = (int)node->key.size();
        for (int i = 0; i < n; i++) {
            if (!node->leaf)
                inorderNode(node->child[i], result);
            result.push_back(node->key[i]);
        }

        if (!node->leaf)
            inorderNode(node->child[n], result);
    }

public:
    BTree(int m = 3) {
        order = m;
        maxKeyCount = order - 1;
        root = nullptr;
    }

    ~BTree() {
        clear(root);
    }

    bool find(int value) const {
        return searchNode(root, value);
    }

    bool insert(int value) {
        if (find(value))
            return false;

        data.push_back(value);
        insertToTreeOnly(value);
        return true;
    }

    bool remove(int value) {
        vector<int>::iterator it = find_if(data.begin(), data.end(),
            [value](int x) { return x == value; });

        if (it == data.end())
            return false;

        data.erase(it);
        rebuildTree();
        return true;
    }

    vector<int> inorder() const {
        vector<int> result;
        inorderNode(root, result);
        return result;
    }

    vector<int> currentData() const {
        return data;
    }
};

// ===== 讀取 tree.dat 並解析整數 =====
vector<int> readFileData(const string& fileName) {
    vector<int> result;
    ifstream fin(fileName);

    if (!fin)
        return result;

    string text((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
    string token;

    for (int i = 0; i <= (int)text.size(); i++) {
        char c = (i < (int)text.size()) ? text[i] : ' ';

        if (isdigit((unsigned char)c) || c == '-' || c == '+') {
            token += c;
        } else {
            if (!token.empty() && token != "-" && token != "+") {
                result.push_back(stoi(token));
            }
            token.clear();
        }
    }

    return result;
}

void printList(const vector<int>& values) {
    for (int i = 0; i < (int)values.size(); i++) {
        cout << values[i];
        if (i != (int)values.size() - 1)
            cout << ", ";
    }
    if (!values.empty())
        cout << ", ";
    cout << endl;
}

int inputOrder() {
    int m;
    cout << "m: ";
    cin >> m;

    while (m < 2 || m > 5) {
        cout << "m must be 2 ~ 5, input again: ";
        cin >> m;
    }

    return m;
}

void printSearchResult(BTree& tree, int value) {
    if (tree.find(value))
        cout << "Found!!" << endl;
    else
        cout << "Not Found!!" << endl;
}

void printTreeData(BTree& tree) {
    cout << "data: ";
    printList(tree.currentData());

    cout << "inorder: ";
    printList(tree.inorder());
}

// ===== 主程式 =====
int main() {
    cout << "Binary Search Tree Create" << endl;
    cout << "Read File (tree.dat)" << endl;

    vector<int> fileData = readFileData("tree.dat");
    if (fileData.empty()) {
        cout << "tree.dat has no data or cannot be opened." << endl;
        return 1;
    }

    cout << "data: ";
    printList(fileData);

    int m = inputOrder();
    BTree tree(m);

    for (int value : fileData)
        tree.insert(value);

    int searchValue;
    cout << endl;
    cout << "search data: ";
    cin >> searchValue;
    printSearchResult(tree, searchValue);

    while (true) {
        int choice;

        cout << endl;
        cout << "----------" << endl;
        cout << "Binary Search Tree" << endl;
        cout << "1. insert" << endl;
        cout << "2. delete" << endl;
        cout << "3. find" << endl;
        cout << "4. exit" << endl;
        cout << "choice: ";

        if (!(cin >> choice))
            break;

        if (choice == 1) {
            int value;
            cout << "data: ";
            cin >> value;

            if (tree.insert(value))
                cout << "Insert success." << endl;
            else
                cout << "Data already exists." << endl;

            printTreeData(tree);
        } else if (choice == 2) {
            int value;
            cout << "data: ";
            cin >> value;

            if (tree.remove(value))
                cout << "Delete success." << endl;
            else
                cout << "Data not found." << endl;

            printTreeData(tree);
        } else if (choice == 3) {
            int value;
            cout << "search data: ";
            cin >> value;
            printSearchResult(tree, value);
        } else if (choice == 4) {
            break;
        } else {
            cout << "Wrong choice." << endl;
        }
    }

    return 0;
}
