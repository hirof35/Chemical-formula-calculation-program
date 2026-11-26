#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

// ==========================================
// 元素の原子量（約90種類に対応）
// ==========================================
unordered_map<string, double> atomicWeight = {
    {"H", 1.00794},  {"He", 4.002602}, {"Li", 6.941},    {"Be", 9.012182}, {"B", 10.811},
    {"C", 12.0107},  {"N", 14.0067},  {"O", 15.9994},   {"F", 18.9984032},{"Ne", 20.1797},
    {"Na", 22.98977},{"Mg", 24.3050},  {"Al", 26.98154}, {"Si", 28.0855}, {"P", 30.97376},
    {"S", 32.065},   {"Cl", 35.453},  {"Ar", 39.948},   {"K", 39.0983},  {"Ca", 40.078},
    {"Sc", 44.95591},{"Ti", 47.867},  {"V", 50.9415},   {"Cr", 51.9961}, {"Mn", 54.93804},
    {"Fe", 55.845},  {"Co", 58.9332}, {"Ni", 58.6934},  {"Cu", 63.546}, {"Zn", 65.38},
    {"Ga", 69.723},  {"Ge", 72.64},   {"As", 74.9216},  {"Se", 78.96},  {"Br", 79.904},
    {"Kr", 83.798},  {"Rb", 85.4678},{"Sr", 87.62},    {"Y", 88.90585},{"Zr", 91.224},
    {"Nb", 92.90638},{"Mo", 95.96},   {"Tc", 98.0},     {"Ru", 101.07}, {"Rh", 102.9055},
    {"Pd", 106.42},  {"Ag", 107.8682},{"Cd", 112.411}, {"In", 114.818},{"Sn", 118.710},
    {"Sb", 121.760}, {"Te", 127.60},  {"I", 126.90447},{"Xe", 131.293},{"Cs", 132.90546},
    {"Ba", 137.327}, {"La", 138.9055},{"Ce", 140.116}, {"Pr", 140.9077},{"Nd", 144.242},
    {"Pm", 145.0},   {"Sm", 150.36},  {"Eu", 151.964}, {"Gd", 157.25}, {"Tb", 158.92535},
    {"Dy", 162.500}, {"Ho", 164.93032},{"Er", 167.259}, {"Tm", 168.93429},{"Yb", 173.054},
    {"Lu", 174.9668},{"Hf", 178.49},  {"Ta", 180.9479},{"W", 183.84}, {"Re", 186.207},
    {"Os", 190.23},  {"Ir", 192.217}, {"Pt", 195.084}, {"Au", 196.9665},{"Hg", 200.59},
    {"Tl", 204.3833},{"Pb", 207.2},   {"Bi", 208.9804},{"Po", 209.0}, {"At", 210.0},
    {"Rn", 222.0},   {"Fr", 223.0},   {"Ra", 226.0254},{"Ac", 227.0},{"Th", 232.0381},
    {"Pa", 231.0359},{"U", 238.0289}, {"Np", 237.0},   {"Pu", 244.0},  {"Am", 243.0},
    {"Cm", 247.0},   {"Bk", 247.0},   {"Cf", 251.0},   {"Es", 252.0},  {"Fm", 257.0}
};

// ==========================================
// 構造体定義
// ==========================================

// 計算結果を格納する構造体
struct CalcResult {
    string formula;
    bool success;
    string errorMessage;
    unordered_map<string, int> elements;
    double molarMass;
};

// ==========================================
// 関数プロトタイプ宣言
// ==========================================

// 元素記号読み取り
bool readElement(const string& s, int& pos, string& elem);

// 数値読み取り
bool readNumber(const string& s, int& pos, int& num);

// 原子数の乗算
void multiplyMap(unordered_map<string, int>& target, int multiplier);

// 原子数の加算
void addMap(unordered_map<string, int>& target, const unordered_map<string, int>& src);

// 化学式の解析
bool parseFormula(const string& formula,
    unordered_map<string, int>& elements,
    double& molarMass,
    string& errorMessage);

// 計算結果の表示
void displayResult(const CalcResult& result);

// 結果をファイルに保存
bool saveResultsToFile(const string& filename, const vector<CalcResult>& results);

// ファイルから化学式を読み込み
vector<string> readFormulasFromFile(const string& filename, string& errorMsg);

// ヘルプメッセージ表示
void showHelp();


// ==========================================
// メイン関数
// ==========================================

int main() {
    cout << "=========================================\n";
    cout << "    化学式 分子量計算プログラム 拡張版\n";
    cout << "=========================================\n\n";

    int choice;
    do {
        cout << "\n--- メニュー ---\n";
        cout << "1:単一計算  2:一括入力  3:ファイル読込  9:ヘルプ  0:終了\n> ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1024, '\n');
            cout << "無効な入力です。数字を入力してください。\n";
            continue;
        }
        cin.ignore();

        vector<CalcResult> allResults;

        switch (choice) {
        case 9:
            showHelp();
            break;

        case 1: {
            CalcResult res;
            cout << "\n化学式を入力してください: ";
            getline(cin, res.formula);

            string err;
            res.success = parseFormula(res.formula, res.elements, res.molarMass, err);
            res.errorMessage = err;
            displayResult(res);
            allResults.push_back(res);
            break;
        }

        case 2: {
            cout << "\n複数の化学式を1行に1つずつ入力してください\n";
            cout << "入力を終了するには 空行 を入力してください:\n";

            string line;
            while (true) {
                cout << "> ";
                getline(cin, line);
                if (line.empty()) break;

                CalcResult res;
                res.formula = line;
                string err;
                res.success = parseFormula(res.formula, res.elements, res.molarMass, err);
                res.errorMessage = err;
                allResults.push_back(res);
                displayResult(res);
            }
            break;
        }

        case 3: {
            string filename, errMsg;
            cout << "\n読み込むファイル名を入力してください: ";
            getline(cin, filename);

            vector<string> formulas = readFormulasFromFile(filename, errMsg);
            if (!errMsg.empty()) {
                cout << "❌ " << errMsg << "\n";
                break;
            }
            if (formulas.empty()) {
                cout << "⚠️ ファイルから化学式が読み込めませんでした\n";
                break;
            }

            cout << "✅ " << formulas.size() << " 件の化学式を読み込みました\n\n";
            for (const string& f : formulas) {
                CalcResult res;
                res.formula = f;
                string err;
                res.success = parseFormula(res.formula, res.elements, res.molarMass, err);
                res.errorMessage = err;
                allResults.push_back(res);
                displayResult(res);
            }
            break;
        }

        case 0:
            cout << "プログラムを終了します。お疲れ様でした！\n";
            break;

        default:
            cout << "無効な選択です。0～3,9 を入力してください。\n";
            continue;
        }

        if (!allResults.empty() && choice != 0) {
            char saveChoice;
            cout << "\n結果をファイルに保存しますか？ (y/n): ";
            cin >> saveChoice;
            cin.ignore();

            if (saveChoice == 'y' || saveChoice == 'Y') {
                string outFile;
                cout << "出力ファイル名: ";
                getline(cin, outFile);
                if (saveResultsToFile(outFile, allResults)) {
                    cout << "✅ " << outFile << " に保存しました！\n";
                }
                else {
                    cout << "❌ ファイルの保存に失敗しました\n";
                }
            }
        }

    } while (choice != 0);

    return 0;
}
// ==========================================
// 関数実装
// ==========================================

bool readElement(const string& s, int& pos, string& elem) {
    if (pos >= s.size() || !isupper(s[pos])) return false;
    elem.clear();
    elem += s[pos++];
    if (pos < s.size() && islower(s[pos])) elem += s[pos++];
    return true;
}

bool readNumber(const string& s, int& pos, int& num) {
    num = 0;
    int start = pos;
    while (pos < s.size() && isdigit(s[pos])) {
        num = num * 10 + (s[pos++] - '0');
    }
    if (pos == start) {
        num = 1;
        return true;
    }
    return num > 0;
}

void multiplyMap(unordered_map<string, int>& target, int multiplier) {
    for (auto& p : target) p.second *= multiplier;
}

void addMap(unordered_map<string, int>& target, const unordered_map<string, int>& src) {
    for (const auto& p : src) target[p.first] += p.second;
}

bool parseFormula(const string& formula,
    unordered_map<string, int>& elements,
    double& molarMass,
    string& errorMessage) {
    elements.clear();
    molarMass = 0.0;
    errorMessage.clear();

    stack<unordered_map<string, int>> st;
    stack<int> posStack;
    st.push({});
    posStack.push(0);

    int n = formula.size();
    int pos = 0;

    while (pos < n) {
        char c = formula[pos];

        if (isspace(c)) {
            pos++;
        }
        else if (c == '(') {
            pos++;
            st.push({});
            posStack.push(pos);
        }
        else if (c == ')') {
            pos++;
            if (st.size() <= 1) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 対応する開き括弧がありません";
                return false;
            }
            int coeff;
            if (!readNumber(formula, pos, coeff)) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 無効な数値です";
                return false;
            }
            if (coeff <= 0) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 係数は正の整数にしてください";
                return false;
            }
            auto inner = st.top(); st.pop();
            posStack.pop();
            multiplyMap(inner, coeff);
            addMap(st.top(), inner);
        }
        else if (isupper(c)) {
            string elem;
            int elemPos = pos;
            if (!readElement(formula, pos, elem)) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 無効な元素記号です";
                return false;
            }
            if (atomicWeight.find(elem) == atomicWeight.end()) {
                errorMessage = "エラー: 位置 " + to_string(elemPos) +
                    " — 未知の元素記号です: '" + elem + "'";
                return false;
            }
            int count;
            if (!readNumber(formula, pos, count)) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 無効な原子数です";
                return false;
            }
            if (count <= 0) {
                errorMessage = "エラー: 位置 " + to_string(pos) + " — 原子数は正の整数にしてください";
                return false;
            }
            st.top()[elem] += count;
        }
        else {
            errorMessage = "エラー: 位置 " + to_string(pos + 1) +
                " — 無効な文字です: '" + string(1, c) + "'";
            return false;
        }
    }

    if (st.size() != 1) {
        errorMessage = "エラー: 閉じていない括弧が残っています";
        return false;
    }

    elements = st.top();

    molarMass = 0.0;
    for (const auto& e : elements) {
        molarMass += e.second * atomicWeight[e.first];
    }
    return true;
}

void displayResult(const CalcResult& result) {
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
    cout << "📌 化学式: " << result.formula << "\n";

    if (!result.success) {
        cout << "❌ " << result.errorMessage << "\n";
        return;
    }

    cout << "✅ 分子量: " << fixed << setprecision(4) << result.molarMass << " g/mol\n\n";
    cout << "元素組成:\n";
    cout << setw(8) << "元素"
        << setw(10) << "原子数"
        << setw(12) << "原子量"
        << setw(14) << "質量比率\n";
    cout << "─────────────────────────────────────────────\n";

    vector<pair<string, double>> sortedPercent;
    for (const auto& e : result.elements) {
        double weight = e.second * atomicWeight[e.first];
        double percent = (weight / result.molarMass) * 100.0;
        sortedPercent.push_back({ e.first, percent });
    }
    sort(sortedPercent.begin(), sortedPercent.end(),
        [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });

    for (const auto& p : sortedPercent) {
        const string& elem = p.first;
        int count = result.elements.at(elem);
        double percent = p.second;

        cout << setw(6) << elem
            << setw(10) << count
            << setw(12) << fixed << setprecision(4) << atomicWeight[elem]
            << setw(13) << fixed << setprecision(2) << percent << "%\n";
    }
    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
}

bool saveResultsToFile(const string& filename, const vector<CalcResult>& results) {
    ofstream ofs(filename);
    if (!ofs.is_open()) return false;

    ofs << "==================================================\n";
    ofs << "       化学式 計算結果レポート\n";
    ofs << "       作成日時: 2026-11-26\n";
    ofs << "==================================================\n\n";

    for (const auto& r : results) {
        ofs << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        ofs << "化学式: " << r.formula << "\n";

        if (!r.success) {
            ofs << ">> " << r.errorMessage << "\n";
            ofs << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
            continue;
        }

        ofs << "分子量: " << fixed << setprecision(4) << r.molarMass << " g/mol\n\n";
        ofs << "  元素   原子数    原子量(g/mol)   質量(%)\n";
        ofs << "-------------------------------------------\n";

        vector<pair<string, double>> sortedPercent;
        for (const auto& e : r.elements) {
            double weight = e.second * atomicWeight[e.first];
            sortedPercent.push_back({ e.first, (weight / r.molarMass) * 100.0 });
        }
        sort(sortedPercent.begin(), sortedPercent.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        for (const auto& p : sortedPercent) {
            const string& elem = p.first;
            int count = r.elements.at(elem);
            double percent = p.second;
            ofs << "  " << setw(4) << elem
                << setw(6) << count
                << setw(12) << fixed << setprecision(4) << atomicWeight[elem]
                << setw(10) << fixed << setprecision(2) << percent << "%\n";
        }
        ofs << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";
    }
    ofs.close();
    return true;
}

vector<string> readFormulasFromFile(const string& filename, string& errorMsg) {
    vector<string> formulas;
    ifstream ifs(filename);
    if (!ifs.is_open()) {
        errorMsg = "ファイルを開けません: " + filename;
        return {};
    }

    string line;
    while (getline(ifs, line)) {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \t") == string::npos)
            continue;

        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");
        string formula = line.substr(start, end - start + 1);
        formulas.push_back(formula);
    }
    ifs.close();
    return formulas;
}

void showHelp() {
    cout << "\n=========================================\n";
    cout << "      化学式 分子量計算プログラム\n";
    cout << "           拡張版 使い方\n";
    cout << "=========================================\n";
    cout << "【メニュー】\n";
    cout << "  1 : 1つの化学式を計算\n";
    cout << "  2 : 複数の化学式を一括入力して計算\n";
    cout << "  3 : ファイルから読み込んで一括計算\n";
    cout << "  0 : 終了\n\n";
    cout << "【入力例】\n";
    cout << "  H2O, NaCl, Ca(OH)2, Fe2(SO4)3\n";
    cout << "  C6H12O6\n\n";
    cout << "【ファイル形式】\n";
    cout << "  1行に1つの化学式を記述\n";
    cout << "  # から始まる行はコメントとして無視されます\n";
    cout << "=========================================\n\n";
}
