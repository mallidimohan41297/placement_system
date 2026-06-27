// ============================================================
//  PLACEMENT MANAGEMENT SYSTEM
//  Version 2.0 — Production Quality | Single File
//
//  Features:
//    • Admin Authentication (hashed password)
//    • Student & Company Management (full CRUD)
//    • Persistent Storage (students.txt, companies.txt)
//    • CSV Import (students.csv) & Export (reports)
//    • Undo Last Delete
//    • Advanced Search (roll, name, branch, CGPA, status,
//                       company, package)
//    • Advanced Sort (6 fields × asc/desc)
//    • Rich Statistics (package stats, branch-wise %, etc.)
//    • Robust Error Handling — no crashes
//    • Modern C++17: const-correctness, auto, lambdas,
//      range-for, STL algorithms, unordered_map, fstream,
//      stringstream, RAII, exception handling
//
//  OOP Concepts: Encapsulation, Abstraction, Composition
//  STL Used    : vector, stack, unordered_map, algorithm,
//                fstream, sstream, iomanip
//
//  Compile: g++ -std=c++17 -o pms placement_system.cpp
//  Run    : ./pms
//
//  Default credentials: username=admin  password=admin123
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cctype>
#include <stdexcept>
#include <numeric>

using namespace std;

// ============================================================
//  SECTION 1 — UTILITIES
// ============================================================

/**
 * @brief Converts a string to lowercase.
 * Time: O(N)  Space: O(N)
 */
static string toLower(const string& s) {
    string r = s;
    transform(r.begin(), r.end(), r.begin(),
              [](unsigned char c){ return tolower(c); });
    return r;
}

/** @brief Trims leading/trailing whitespace. */
static string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : s.substr(start, end - start + 1);
}

/** @brief Clears std::cin error state and flushes the buffer. */
static void clearIn() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

/** @brief Prompts and reads a validated integer. */
static int getInt(const string& prompt) {
    int v{};
    while (true) {
        cout << prompt;
        if (cin >> v) { clearIn(); return v; }
        cout << "  [!] Invalid input. Enter an integer.\n";
        clearIn();
    }
}

/** @brief Prompts and reads a validated double. */
static double getDouble(const string& prompt) {
    double v{};
    while (true) {
        cout << prompt;
        if (cin >> v) { clearIn(); return v; }
        cout << "  [!] Invalid input. Enter a number.\n";
        clearIn();
    }
}

/** @brief Reads a full line of text after displaying a prompt. */
static string getString(const string& prompt) {
    string v;
    cout << prompt;
    getline(cin, v);
    return trim(v);
}

/**
 * @brief Simple djb2-based hash — no external dependencies.
 *        Used to store passwords without plaintext.
 * Time: O(N)  Space: O(1)
 */
static size_t hashPassword(const string& password) {
    size_t hash = 5381;
    for (unsigned char c : password)
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    return hash;
}

/** @brief Prints a separator line of given character and width. */
static void printLine(char ch = '=', int width = 60) {
    cout << string(width, ch) << "\n";
}

// ============================================================
//  SECTION 2 — STUDENT CLASS
// ============================================================

/**
 * @class Student
 * @brief Encapsulates all data and behavior of a student.
 *
 * Fields : rollNumber, name, branch, cgpa, backlogsCount,
 *          isPlaced, placedCompany, packageLPA
 */
class Student {
private:
    int    rollNumber;
    string name;
    string branch;
    double cgpa;
    int    backlogsCount;
    bool   isPlaced;
    string placedCompany;
    double packageLPA;      ///< Package offered when placed (0 if unplaced)

public:
    // ── Constructors ──────────────────────────────────────────
    Student()
        : rollNumber(0), cgpa(0.0), backlogsCount(0),
          isPlaced(false), packageLPA(0.0) {}

    Student(int roll, const string& n, const string& br,
            double c, int bl)
        : rollNumber(roll), name(n), branch(br), cgpa(c),
          backlogsCount(bl), isPlaced(false),
          placedCompany(""), packageLPA(0.0) {}

    // ── Getters ───────────────────────────────────────────────
    int    getRollNumber()    const { return rollNumber; }
    string getName()          const { return name; }
    string getBranch()        const { return branch; }
    double getCGPA()          const { return cgpa; }
    int    getBacklogs()      const { return backlogsCount; }
    bool   getIsPlaced()      const { return isPlaced; }
    string getPlacedCompany() const { return placedCompany; }
    double getPackageLPA()    const { return packageLPA; }

    // ── Setters ───────────────────────────────────────────────
    /**
     * @brief Marks the student as placed or unplaced.
     * @param p        true = placed
     * @param company  Name of the hiring company
     * @param pkg      Package offered in LPA
     */
    void setPlaced(bool p, const string& company = "", double pkg = 0.0) {
        isPlaced      = p;
        placedCompany = p ? company : "";
        packageLPA    = p ? pkg     : 0.0;
    }

    // ── Display ───────────────────────────────────────────────
    /** @brief Full detail card for a student. */
    void display() const {
        printLine('-');
        cout << left
             << setw(22) << "Roll Number:"   << rollNumber  << "\n"
             << setw(22) << "Name:"          << name        << "\n"
             << setw(22) << "Branch:"        << branch      << "\n"
             << setw(22) << "CGPA:"          << fixed << setprecision(2) << cgpa << "\n"
             << setw(22) << "Backlogs:"      << backlogsCount << "\n"
             << setw(22) << "Placed:"        << (isPlaced ? "Yes" : "No") << "\n";
        if (isPlaced) {
            cout << setw(22) << "Company:"   << placedCompany << "\n"
                 << setw(22) << "Package:"   << fixed << setprecision(1)
                                             << packageLPA << " LPA\n";
        }
        printLine('-');
    }

    /** @brief Compact single-line summary for table views. */
    void displayShort() const {
        cout << left
             << setw(8)  << rollNumber
             << setw(22) << name
             << setw(8)  << branch
             << setw(7)  << fixed << setprecision(2) << cgpa
             << setw(9)  << backlogsCount
             << setw(8)  << (isPlaced ? "Yes" : "No")
             << setw(20) << (isPlaced ? placedCompany : "-")
             << (isPlaced ? (to_string((int)packageLPA) + " LPA") : "-")
             << "\n";
    }

    // ── Serialization ─────────────────────────────────────────
    /**
     * @brief Serializes to pipe-delimited string for file storage.
     *        Format: roll|name|branch|cgpa|backlogs|placed|company|pkg
     */
    string serialize() const {
        return to_string(rollNumber) + "|" + name    + "|" + branch + "|"
             + to_string(cgpa)       + "|" + to_string(backlogsCount) + "|"
             + to_string(isPlaced)   + "|" + placedCompany + "|"
             + to_string(packageLPA);
    }

    /**
     * @brief Deserializes a Student from a pipe-delimited string.
     * @throws std::runtime_error on malformed data
     * Time: O(N)  Space: O(1)
     */
    static Student deserialize(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, '|'))
            fields.push_back(token);
        if (fields.size() < 8)
            throw runtime_error("Malformed student record: " + line);

        Student s;
        s.rollNumber    = stoi(fields[0]);
        s.name          = fields[1];
        s.branch        = fields[2];
        s.cgpa          = stod(fields[3]);
        s.backlogsCount = stoi(fields[4]);
        s.isPlaced      = stoi(fields[5]) != 0;
        s.placedCompany = fields[6];
        s.packageLPA    = stod(fields[7]);
        return s;
    }
};

// ============================================================
//  SECTION 3 — COMPANY CLASS
// ============================================================

/**
 * @class Company
 * @brief Encapsulates all data and behavior of a recruiting company.
 */
class Company {
private:
    int    companyId;
    string name;
    string domain;
    string requiredBranch;  ///< "ALL" means open to every branch
    double minCGPA;
    double packageLPA;
    int    maxBacklogsAllowed;

public:
    // ── Constructors ──────────────────────────────────────────
    Company()
        : companyId(0), minCGPA(0.0), packageLPA(0.0),
          maxBacklogsAllowed(0) {}

    Company(int id, const string& n, const string& dom,
            double mc, int mb, const string& br, double pkg)
        : companyId(id), name(n), domain(dom), requiredBranch(br),
          minCGPA(mc), packageLPA(pkg), maxBacklogsAllowed(mb) {}

    // ── Getters ───────────────────────────────────────────────
    int    getCompanyId()      const { return companyId; }
    string getName()           const { return name; }
    string getDomain()         const { return domain; }
    string getRequiredBranch() const { return requiredBranch; }
    double getMinCGPA()        const { return minCGPA; }
    double getPackageLPA()     const { return packageLPA; }
    int    getMaxBacklogs()    const { return maxBacklogsAllowed; }

    // ── Display ───────────────────────────────────────────────
    void display() const {
        printLine('-');
        cout << left
             << setw(26) << "Company ID:"    << companyId  << "\n"
             << setw(26) << "Name:"          << name       << "\n"
             << setw(26) << "Domain:"        << domain     << "\n"
             << setw(26) << "Package (LPA):" << fixed << setprecision(1) << packageLPA << "\n"
             << setw(26) << "Min CGPA:"      << minCGPA    << "\n"
             << setw(26) << "Max Backlogs:"  << maxBacklogsAllowed << "\n"
             << setw(26) << "Branch:"        << requiredBranch << "\n";
        printLine('-');
    }

    // ── Serialization ─────────────────────────────────────────
    /** @brief Pipe-delimited: id|name|domain|minCGPA|maxBL|branch|pkg */
    string serialize() const {
        return to_string(companyId) + "|" + name   + "|" + domain + "|"
             + to_string(minCGPA)   + "|" + to_string(maxBacklogsAllowed) + "|"
             + requiredBranch       + "|" + to_string(packageLPA);
    }

    /**
     * @brief Deserializes a Company from a pipe-delimited string.
     * @throws std::runtime_error on malformed data
     */
    static Company deserialize(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> fields;
        while (getline(ss, token, '|'))
            fields.push_back(token);
        if (fields.size() < 7)
            throw runtime_error("Malformed company record: " + line);

        return Company(
            stoi(fields[0]),   // id
            fields[1],         // name
            fields[2],         // domain
            stod(fields[3]),   // minCGPA
            stoi(fields[4]),   // maxBacklogs
            fields[5],         // branch
            stod(fields[6])    // package
        );
    }
};

// ============================================================
//  SECTION 4 — FILE MANAGER
// ============================================================

/**
 * @class FileManager
 * @brief Handles all persistent storage using fstream.
 *        RAII: files are opened and closed within each method.
 *
 *  Files:
 *    students.txt  — pipe-delimited student records
 *    companies.txt — pipe-delimited company records
 */
class FileManager {
public:
    static const string STUDENTS_FILE;
    static const string COMPANIES_FILE;

    /**
     * @brief Saves a collection to a text file (one record per line).
     * Time: O(N)  Space: O(1) auxiliary
     * @tparam T  Must implement serialize() const -> string
     */
    template<typename T>
    static bool saveAll(const string& filename, const vector<T>& items) {
        ofstream ofs(filename, ios::trunc);
        if (!ofs.is_open()) {
            cerr << "  [!] Cannot write to " << filename << "\n";
            return false;
        }
        for (const auto& item : items)
            ofs << item.serialize() << "\n";
        return true;
    }

    /**
     * @brief Loads students from file; skips malformed lines gracefully.
     * Time: O(N)  Space: O(N)
     */
    static vector<Student> loadStudents() {
        vector<Student> students;
        ifstream ifs(STUDENTS_FILE);
        if (!ifs.is_open()) return students; // file doesn't exist yet — OK
        string line;
        int lineNo = 0;
        while (getline(ifs, line)) {
            ++lineNo;
            line = trim(line);
            if (line.empty()) continue;
            try {
                students.push_back(Student::deserialize(line));
            } catch (const exception& e) {
                cerr << "  [WARN] Skipping corrupt student record (line "
                     << lineNo << "): " << e.what() << "\n";
            }
        }
        return students;
    }

    /**
     * @brief Loads companies from file; skips malformed lines gracefully.
     * Time: O(N)  Space: O(N)
     */
    static vector<Company> loadCompanies() {
        vector<Company> companies;
        ifstream ifs(COMPANIES_FILE);
        if (!ifs.is_open()) return companies;
        string line;
        int lineNo = 0;
        while (getline(ifs, line)) {
            ++lineNo;
            line = trim(line);
            if (line.empty()) continue;
            try {
                companies.push_back(Company::deserialize(line));
            } catch (const exception& e) {
                cerr << "  [WARN] Skipping corrupt company record (line "
                     << lineNo << "): " << e.what() << "\n";
            }
        }
        return companies;
    }
};

const string FileManager::STUDENTS_FILE  = "students.txt";
const string FileManager::COMPANIES_FILE = "companies.txt";

// ============================================================
//  SECTION 5 — CSV MANAGER
// ============================================================

/**
 * @class CSVManager
 * @brief Handles CSV import and export using fstream + stringstream.
 *
 *  Import  : students.csv
 *  Exports : placement_report.csv, student_report.csv,
 *            company_report.csv
 */
class CSVManager {
public:
    /**
     * @brief Parses one CSV line into fields (handles quoted commas).
     * Time: O(N)  Space: O(K) where K = number of fields
     */
    static vector<string> parseCSVLine(const string& line) {
        vector<string> fields;
        string field;
        bool inQuotes = false;
        for (char c : line) {
            if (c == '"') {
                inQuotes = !inQuotes;
            } else if (c == ',' && !inQuotes) {
                fields.push_back(trim(field));
                field.clear();
            } else {
                field += c;
            }
        }
        fields.push_back(trim(field));
        return fields;
    }

    /**
     * @brief Imports students from students.csv.
     *        Expected CSV header: Roll,Name,Branch,CGPA,Backlogs
     *        Invalid rows are skipped with a warning.
     * Time: O(N)  Space: O(N)
     * @param[out] imported  Number of successfully imported rows
     * @param[out] skipped   Number of skipped/invalid rows
     */
    static vector<Student> importStudents(const string& filename,
                                          int& imported, int& skipped) {
        vector<Student> result;
        imported = skipped = 0;
        ifstream ifs(filename);
        if (!ifs.is_open()) {
            cerr << "  [!] File not found: " << filename << "\n";
            return result;
        }

        string line;
        bool headerSkipped = false;
        int lineNo = 0;

        while (getline(ifs, line)) {
            ++lineNo;
            line = trim(line);
            if (line.empty()) continue;

            // Skip header row
            if (!headerSkipped) {
                headerSkipped = true;
                // If first field is not numeric, treat as header
                auto fields = parseCSVLine(line);
                if (!fields.empty() && !isdigit((unsigned char)fields[0][0]))
                    continue;
            }

            try {
                auto f = parseCSVLine(line);
                if (f.size() < 5) throw runtime_error("Not enough columns");

                int    roll = stoi(f[0]);
                string name = f[1];
                string branch = f[2];
                double cgpa   = stod(f[3]);
                int    bl     = stoi(f[4]);

                if (name.empty() || branch.empty())
                    throw runtime_error("Empty name or branch");
                if (cgpa < 0.0 || cgpa > 10.0)
                    throw runtime_error("CGPA out of range: " + to_string(cgpa));
                if (bl < 0)
                    throw runtime_error("Negative backlogs");

                result.push_back(Student(roll, name, branch, cgpa, bl));
                ++imported;
            } catch (const exception& e) {
                cerr << "  [WARN] Skipping CSV row " << lineNo
                     << ": " << e.what() << "\n";
                ++skipped;
            }
        }
        return result;
    }

    /** @brief Exports placed students to placement_report.csv. */
    static bool exportPlacementReport(const vector<Student>& students,
                                       const string& filename = "placement_report.csv") {
        ofstream ofs(filename, ios::trunc);
        if (!ofs.is_open()) { cerr << "  [!] Cannot create " << filename << "\n"; return false; }
        ofs << "Roll,Name,Branch,CGPA,Company,Package_LPA\n";
        for (const auto& s : students)
            if (s.getIsPlaced())
                ofs << s.getRollNumber() << ","
                    << s.getName()       << ","
                    << s.getBranch()     << ","
                    << fixed << setprecision(2) << s.getCGPA() << ","
                    << s.getPlacedCompany() << ","
                    << s.getPackageLPA() << "\n";
        cout << "  [OK] " << filename << " exported.\n";
        return true;
    }

    /** @brief Exports all students to student_report.csv. */
    static bool exportStudentReport(const vector<Student>& students,
                                     const string& filename = "student_report.csv") {
        ofstream ofs(filename, ios::trunc);
        if (!ofs.is_open()) { cerr << "  [!] Cannot create " << filename << "\n"; return false; }
        ofs << "Roll,Name,Branch,CGPA,Backlogs,Placed,Company,Package_LPA\n";
        for (const auto& s : students)
            ofs << s.getRollNumber()     << ","
                << s.getName()           << ","
                << s.getBranch()         << ","
                << fixed << setprecision(2) << s.getCGPA() << ","
                << s.getBacklogs()       << ","
                << (s.getIsPlaced() ? "Yes" : "No") << ","
                << (s.getIsPlaced() ? s.getPlacedCompany() : "-") << ","
                << s.getPackageLPA()     << "\n";
        cout << "  [OK] " << filename << " exported.\n";
        return true;
    }

    /** @brief Exports all companies to company_report.csv. */
    static bool exportCompanyReport(const vector<Company>& companies,
                                     const string& filename = "company_report.csv") {
        ofstream ofs(filename, ios::trunc);
        if (!ofs.is_open()) { cerr << "  [!] Cannot create " << filename << "\n"; return false; }
        ofs << "ID,Name,Domain,MinCGPA,MaxBacklogs,Branch,Package_LPA\n";
        for (const auto& c : companies)
            ofs << c.getCompanyId()      << ","
                << c.getName()           << ","
                << c.getDomain()         << ","
                << c.getMinCGPA()        << ","
                << c.getMaxBacklogs()    << ","
                << c.getRequiredBranch() << ","
                << c.getPackageLPA()     << "\n";
        cout << "  [OK] " << filename << " exported.\n";
        return true;
    }
};

// ============================================================
//  SECTION 6 — AUTHENTICATION
// ============================================================

/**
 * @class Authentication
 * @brief Manages admin login with hashed password verification.
 *
 *  Default credentials: username = admin, password = admin123
 *  The password is stored as a djb2 hash — never plaintext.
 *
 *  To change credentials, update ADMIN_USERNAME and
 *  compute a new hash with hashPassword("yourpassword").
 */
class Authentication {
private:
    static const string ADMIN_USERNAME;
    static const size_t ADMIN_PASSWORD_HASH; ///< hash of "admin123"

    static const int MAX_ATTEMPTS = 3;

public:
    /**
     * @brief Prompts for credentials; allows MAX_ATTEMPTS tries.
     * @return true if authenticated, false if all attempts exhausted.
     */
    static bool login() {
        cout << "\n";
        printLine('=', 50);
        cout << "      PLACEMENT MANAGEMENT SYSTEM — LOGIN\n";
        printLine('=', 50);

        for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt) {
            cout << "\nAttempt " << attempt << " of " << MAX_ATTEMPTS << "\n";

            string username = getString("Username : ");
            // Hide password input (echoing is terminal-specific;
            // we skip raw-mode toggling for portability)
            string password = getString("Password : ");

            if (username == ADMIN_USERNAME &&
                hashPassword(password) == ADMIN_PASSWORD_HASH) {
                cout << "\n  [OK] Welcome, " << username << "!\n";
                return true;
            }
            cout << "  [!] Invalid credentials.\n";
        }
        cout << "\n  [!] Too many failed attempts. Exiting.\n";
        return false;
    }
};

// Hash of "admin123" pre-computed via djb2
// If you change the password, replace this value with:
//     cout << hashPassword("yourpassword");
const string Authentication::ADMIN_USERNAME    = "admin";
const size_t Authentication::ADMIN_PASSWORD_HASH =
    []() -> size_t {
        size_t h = 5381;
        for (unsigned char c : string("admin123"))
            h = ((h << 5) + h) + c;
        return h;
    }();

// ============================================================
//  SECTION 7 — PLACEMENT MANAGER
// ============================================================

/**
 * @class PlacementManager
 * @brief Core business logic: CRUD, search, sort, eligibility,
 *        statistics, undo-delete, and persistence coordination.
 *
 *  Data Structures:
 *    vector<Student>   — O(1) append, O(N) search
 *    vector<Company>   — O(1) append, O(N) search
 *    stack<Student>    — O(1) push/pop for undo
 *    unordered_map     — O(1) average for branch stats
 */
class PlacementManager {
private:
    vector<Student> students;
    vector<Company> companies;
    stack<Student>  deletedStudents; ///< Undo-delete buffer

    // ── Internal Finders ──────────────────────────────────────
    /**
     * @brief Linear search by roll number.
     * Time: O(N)  Space: O(1)
     */
    int findStudentIdx(int roll) const {
        for (int i = 0; i < (int)students.size(); ++i)
            if (students[i].getRollNumber() == roll) return i;
        return -1;
    }

    /**
     * @brief Linear search by company ID.
     * Time: O(N)  Space: O(1)
     */
    int findCompanyIdx(int id) const {
        for (int i = 0; i < (int)companies.size(); ++i)
            if (companies[i].getCompanyId() == id) return i;
        return -1;
    }

    /**
     * @brief Checks placement eligibility of student s for company c.
     * Time: O(1)  Space: O(1)
     */
    static bool eligible(const Student& s, const Company& c) {
        if (s.getIsPlaced())                               return false;
        if (s.getCGPA()     < c.getMinCGPA())              return false;
        if (s.getBacklogs() > c.getMaxBacklogs())          return false;
        if (c.getRequiredBranch() != "ALL" &&
            toLower(c.getRequiredBranch()) != toLower(s.getBranch()))
            return false;
        return true;
    }

    /** @brief Prints the column header for the student table. */
    static void printStudentHeader() {
        cout << left
             << setw(8)  << "Roll"
             << setw(22) << "Name"
             << setw(8)  << "Branch"
             << setw(7)  << "CGPA"
             << setw(9)  << "Backlogs"
             << setw(8)  << "Placed"
             << setw(20) << "Company"
             << "Package\n"
             << string(90, '=') << "\n";
    }

    /** @brief Persists students to disk after any mutation. */
    void saveStudents() const {
        FileManager::saveAll(FileManager::STUDENTS_FILE, students);
    }

    /** @brief Persists companies to disk after any mutation. */
    void saveCompanies() const {
        FileManager::saveAll(FileManager::COMPANIES_FILE, companies);
    }

public:
    // ── Constructor — loads from disk on startup ───────────────
    PlacementManager() {
        students  = FileManager::loadStudents();
        companies = FileManager::loadCompanies();
        if (!students.empty() || !companies.empty())
            cout << "  [INFO] Loaded " << students.size()
                 << " students and " << companies.size()
                 << " companies from disk.\n";
    }

    // ── Accessors for CSV export ───────────────────────────────
    const vector<Student>& getStudents()  const { return students; }
    const vector<Company>& getCompanies() const { return companies; }

    // ============================================================
    //  STUDENT CRUD
    // ============================================================

    /**
     * @brief Adds a student; rejects duplicates.
     * Time: O(N)  Space: O(1)
     */
    bool addStudent(const Student& s) {
        if (findStudentIdx(s.getRollNumber()) != -1) {
            cout << "  [ERROR] Roll " << s.getRollNumber() << " already exists.\n";
            return false;
        }
        students.push_back(s);
        saveStudents();
        cout << "  [OK] " << s.getName() << " added.\n";
        return true;
    }

    /**
     * @brief Removes a student; pushes to undo stack.
     * Time: O(N)  Space: O(1)
     */
    bool removeStudent(int roll) {
        int i = findStudentIdx(roll);
        if (i == -1) { cout << "  [ERROR] Student not found.\n"; return false; }
        deletedStudents.push(students[i]);
        cout << "  [OK] " << students[i].getName() << " deleted. (Undo available)\n";
        students.erase(students.begin() + i);
        saveStudents();
        return true;
    }

    /**
     * @brief Restores the most recently deleted student.
     * Time: O(N) for dup check  Space: O(1)
     */
    bool undoDelete() {
        if (deletedStudents.empty()) {
            cout << "  [INFO] Nothing to undo.\n"; return false;
        }
        Student s = deletedStudents.top();
        deletedStudents.pop();
        if (findStudentIdx(s.getRollNumber()) != -1) {
            cout << "  [WARN] Cannot restore Roll " << s.getRollNumber()
                 << " — a student with that roll already exists.\n";
            return false;
        }
        students.push_back(s);
        saveStudents();
        cout << "  [OK] Restored: " << s.getName() << "\n";
        return true;
    }

    /**
     * @brief Pointer-based find; returns nullptr if not found.
     * Time: O(N)  Space: O(1)
     */
    Student* findStudent(int roll) {
        int i = findStudentIdx(roll);
        return (i == -1) ? nullptr : &students[i];
    }

    // ── Display ───────────────────────────────────────────────
    void displayAll() const {
        cout << "\n===== All Students (" << students.size() << ") =====\n";
        if (students.empty()) { cout << "  (none)\n"; return; }
        printStudentHeader();
        for (const auto& s : students) s.displayShort();
    }

    void displayPlaced() const {
        cout << "\n===== Placed Students =====\n";
        printStudentHeader();
        int count = 0;
        for (const auto& s : students)
            if (s.getIsPlaced()) { s.displayShort(); ++count; }
        if (!count) cout << "  (none)\n";
        else cout << "  Total: " << count << "\n";
    }

    void displayUnplaced() const {
        cout << "\n===== Unplaced Students =====\n";
        printStudentHeader();
        int count = 0;
        for (const auto& s : students)
            if (!s.getIsPlaced()) { s.displayShort(); ++count; }
        if (!count) cout << "  (none)\n";
        else cout << "  Total: " << count << "\n";
    }

    // ============================================================
    //  ADVANCED SEARCH
    // ============================================================

    /**
     * @brief Search by exact roll number.
     * Time: O(N)  Space: O(1)
     */
    Student* searchByRoll(int roll) {
        return findStudent(roll);
    }

    /**
     * @brief Case-insensitive substring search on name.
     * Time: O(N·M) where M = avg name length  Space: O(K) results
     */
    vector<Student> searchByName(const string& kw) const {
        const string k = toLower(kw);
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [&k](const Student& s){
                return toLower(s.getName()).find(k) != string::npos;
            });
        return r;
    }

    /**
     * @brief Exact case-insensitive branch match.
     * Time: O(N)  Space: O(K) results
     */
    vector<Student> searchByBranch(const string& br) const {
        const string b = toLower(br);
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [&b](const Student& s){
                return toLower(s.getBranch()) == b;
            });
        return r;
    }

    /**
     * @brief Inclusive CGPA range filter.
     * Time: O(N)  Space: O(K) results
     */
    vector<Student> searchByCGPARange(double lo, double hi) const {
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [lo, hi](const Student& s){
                return s.getCGPA() >= lo && s.getCGPA() <= hi;
            });
        return r;
    }

    /**
     * @brief Filter by placement status.
     * Time: O(N)  Space: O(K) results
     */
    vector<Student> searchByPlacementStatus(bool placed) const {
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [placed](const Student& s){ return s.getIsPlaced() == placed; });
        return r;
    }

    /**
     * @brief Case-insensitive company name substring search.
     * Time: O(N·M)  Space: O(K) results
     */
    vector<Student> searchByCompany(const string& companyKw) const {
        const string k = toLower(companyKw);
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [&k](const Student& s){
                return s.getIsPlaced() &&
                       toLower(s.getPlacedCompany()).find(k) != string::npos;
            });
        return r;
    }

    /**
     * @brief Filters placed students by minimum package.
     * Time: O(N)  Space: O(K) results
     */
    vector<Student> searchByMinPackage(double minPkg) const {
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [minPkg](const Student& s){
                return s.getIsPlaced() && s.getPackageLPA() >= minPkg;
            });
        return r;
    }

    // ============================================================
    //  ADVANCED SORT
    // ============================================================

    /**
     * @brief Generic sort — returns sorted copy.
     * Time: O(N log N)  Space: O(N)
     * @param field  "name"|"roll"|"cgpa"|"package"|"branch"|"placed"
     * @param asc    true = ascending, false = descending
     */
    vector<Student> sortStudents(const string& field, bool asc = true) const {
        vector<Student> r = students;
        auto cmp = [&](const Student& a, const Student& b) -> bool {
            bool result;
            if (field == "name")
                result = toLower(a.getName()) < toLower(b.getName());
            else if (field == "roll")
                result = a.getRollNumber() < b.getRollNumber();
            else if (field == "cgpa")
                result = a.getCGPA() < b.getCGPA();
            else if (field == "package")
                result = a.getPackageLPA() < b.getPackageLPA();
            else if (field == "branch")
                result = toLower(a.getBranch()) < toLower(b.getBranch());
            else if (field == "placed")
                result = (int)a.getIsPlaced() < (int)b.getIsPlaced();
            else
                result = a.getRollNumber() < b.getRollNumber();
            return asc ? result : !result;
        };
        sort(r.begin(), r.end(), cmp);
        return r;
    }

    // Convenience wrappers (preserved from v1)
    vector<Student> sortByCGPA(bool asc = false) const { return sortStudents("cgpa", asc); }
    vector<Student> sortByRoll(bool asc = true)  const { return sortStudents("roll", asc); }
    vector<Student> sortByName(bool asc = true)  const { return sortStudents("name", asc); }

    // ============================================================
    //  COMPANY CRUD
    // ============================================================

    bool addCompany(const Company& c) {
        if (findCompanyIdx(c.getCompanyId()) != -1) {
            cout << "  [ERROR] Company ID " << c.getCompanyId() << " already exists.\n";
            return false;
        }
        companies.push_back(c);
        saveCompanies();
        cout << "  [OK] " << c.getName() << " added.\n";
        return true;
    }

    Company* findCompany(int id) {
        int i = findCompanyIdx(id);
        return (i == -1) ? nullptr : &companies[i];
    }

    void displayAllCompanies() const {
        cout << "\n===== Companies (" << companies.size() << ") =====\n";
        if (companies.empty()) { cout << "  (none)\n"; return; }
        cout << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(14) << "Domain"
             << setw(8)  << "Pkg"
             << setw(10) << "MinCGPA"
             << setw(8)  << "MaxBL"
             << "Branch\n"
             << string(80, '=') << "\n";
        for (const auto& c : companies)
            cout << left
                 << setw(6)  << c.getCompanyId()
                 << setw(20) << c.getName()
                 << setw(14) << c.getDomain()
                 << setw(8)  << fixed << setprecision(1) << c.getPackageLPA()
                 << setw(10) << c.getMinCGPA()
                 << setw(8)  << c.getMaxBacklogs()
                 << c.getRequiredBranch() << "\n";
    }

    /**
     * @brief Sort companies by package (descending by default).
     * Time: O(N log N)  Space: O(N)
     */
    vector<Company> sortCompaniesByPackage(bool asc = false) const {
        vector<Company> r = companies;
        sort(r.begin(), r.end(), [asc](const Company& a, const Company& b){
            return asc ? a.getPackageLPA() < b.getPackageLPA()
                       : a.getPackageLPA() > b.getPackageLPA();
        });
        return r;
    }

    // ============================================================
    //  ELIGIBILITY & PLACEMENT
    // ============================================================

    /**
     * @brief Returns eligible students for a company, sorted by CGPA desc.
     * Time: O(N log N)  Space: O(K) results
     */
    vector<Student> getEligibleStudents(int companyId) const {
        int i = findCompanyIdx(companyId);
        if (i == -1) { cout << "  [ERROR] Company not found.\n"; return {}; }
        const Company& c = companies[i];
        vector<Student> r;
        copy_if(students.begin(), students.end(), back_inserter(r),
            [&c](const Student& s){ return eligible(s, c); });
        sort(r.begin(), r.end(), [](const Student& a, const Student& b){
            return a.getCGPA() > b.getCGPA();
        });
        return r;
    }

    /**
     * @brief Returns companies a student is eligible for, sorted by package desc.
     * Time: O(N log N)  Space: O(K) results
     */
    vector<Company> getEligibleCompanies(int roll) const {
        int i = findStudentIdx(roll);
        if (i == -1) { cout << "  [ERROR] Student not found.\n"; return {}; }
        const Student& s = students[i];
        vector<Company> r;
        copy_if(companies.begin(), companies.end(), back_inserter(r),
            [&s](const Company& c){ return eligible(s, c); });
        sort(r.begin(), r.end(), [](const Company& a, const Company& b){
            return a.getPackageLPA() > b.getPackageLPA();
        });
        return r;
    }

    /**
     * @brief Marks a student as placed at a company with a package.
     */
    bool markPlaced(int roll, const string& company, double pkg) {
        Student* s = findStudent(roll);
        if (!s) { cout << "  [ERROR] Student not found.\n"; return false; }
        if (s->getIsPlaced()) {
            cout << "  [WARN] " << s->getName()
                 << " is already placed at " << s->getPlacedCompany() << ".\n";
            return false;
        }
        s->setPlaced(true, company, pkg);
        saveStudents();
        cout << "  [OK] " << s->getName()
             << " placed at " << company
             << " (" << fixed << setprecision(1) << pkg << " LPA)!\n";
        return true;
    }

    // ============================================================
    //  STATISTICS
    // ============================================================

    /**
     * @brief Displays comprehensive placement statistics.
     * Time: O(N)  Space: O(B) where B = number of unique branches
     */
    void displayStats() const {
        int total  = (int)students.size();
        int placed = (int)count_if(students.begin(), students.end(),
                         [](const Student& s){ return s.getIsPlaced(); });
        int unplaced = total - placed;

        // CGPA stats
        double sumCGPA = 0, maxCGPA = 0, minCGPA = 10.0;
        for (const auto& s : students) {
            sumCGPA += s.getCGPA();
            maxCGPA  = max(maxCGPA, s.getCGPA());
            minCGPA  = min(minCGPA, s.getCGPA());
        }
        double avgCGPA = total ? sumCGPA / total : 0.0;
        if (total == 0) minCGPA = 0.0;

        // Package stats (placed students only)
        double sumPkg = 0, maxPkg = 0, minPkg = 1e9;
        for (const auto& s : students) {
            if (s.getIsPlaced()) {
                sumPkg += s.getPackageLPA();
                maxPkg  = max(maxPkg, s.getPackageLPA());
                minPkg  = min(minPkg, s.getPackageLPA());
            }
        }
        double avgPkg = placed ? sumPkg / placed : 0.0;
        if (!placed) minPkg = 0.0;

        // Branch-wise stats using unordered_map — O(1) average insert/lookup
        unordered_map<string, int> branchTotal, branchPlaced;
        for (const auto& s : students) {
            branchTotal[s.getBranch()]++;
            if (s.getIsPlaced()) branchPlaced[s.getBranch()]++;
        }

        // Most recruiting company
        unordered_map<string, int> companyCount;
        for (const auto& s : students)
            if (s.getIsPlaced()) companyCount[s.getPlacedCompany()]++;
        string topCompany = "-";
        int topCount = 0;
        for (const auto& [comp, cnt] : companyCount)
            if (cnt > topCount) { topCount = cnt; topCompany = comp; }

        // Print
        printLine('=', 50);
        cout << "          PLACEMENT STATISTICS\n";
        printLine('=', 50);
        cout << left
             << setw(30) << "Total Students:"   << total    << "\n"
             << setw(30) << "Placed:"            << placed   << "\n"
             << setw(30) << "Unplaced:"          << unplaced << "\n"
             << setw(30) << "Placement %:"
             << fixed << setprecision(1)
             << (total ? (double)placed / total * 100.0 : 0.0) << "%\n";
        printLine('-', 50);
        cout << setw(30) << "Highest Package:" << fixed << setprecision(1) << maxPkg << " LPA\n"
             << setw(30) << "Lowest Package:"  << minPkg << " LPA\n"
             << setw(30) << "Average Package:" << avgPkg << " LPA\n";
        printLine('-', 50);
        cout << setw(30) << "Average CGPA:"    << setprecision(2) << avgCGPA << "\n"
             << setw(30) << "Highest CGPA:"    << maxCGPA << "\n"
             << setw(30) << "Lowest CGPA:"     << minCGPA << "\n";
        printLine('-', 50);
        cout << setw(30) << "Most Recruiting Company:" << topCompany
             << " (" << topCount << ")\n";
        printLine('-', 50);
        cout << "\n  Branch-Wise Placement:\n";
        cout << left << setw(12) << "  Branch" << setw(10) << "Total"
             << setw(10) << "Placed" << "Rate\n" << string(40, '-') << "\n";
        for (const auto& [branch, tot] : branchTotal) {
            int pl = branchPlaced.count(branch) ? branchPlaced[branch] : 0;
            double rate = tot ? (double)pl / tot * 100.0 : 0.0;
            cout << "  " << setw(10) << branch << setw(10) << tot
                 << setw(10) << pl
                 << fixed << setprecision(1) << rate << "%\n";
        }
        printLine('=', 50);
    }

    // ============================================================
    //  CSV IMPORT (delegates to CSVManager, checks duplicates)
    // ============================================================
    void importFromCSV(const string& filename) {
        int imported = 0, skipped = 0;
        auto rows = CSVManager::importStudents(filename, imported, skipped);
        int added = 0, dups = 0;
        for (const auto& s : rows) {
            if (findStudentIdx(s.getRollNumber()) != -1) {
                cout << "  [SKIP] Duplicate roll " << s.getRollNumber() << "\n";
                ++dups;
            } else {
                students.push_back(s);
                ++added;
            }
        }
        if (added) saveStudents();
        cout << "  [CSV] Parsed=" << imported
             << " Added=" << added
             << " Duplicates=" << dups
             << " Skipped(invalid)=" << skipped << "\n";
    }
};

// ============================================================
//  SECTION 8 — SAMPLE DATA LOADER
// ============================================================

/**
 * @brief Populates manager with demo data (only if storage is empty).
 */
static void loadSample(PlacementManager& pm) {
    pm.addStudent(Student(101, "Aditya Sharma",  "CSE", 9.1, 0));
    pm.addStudent(Student(102, "Priya Patel",    "ECE", 8.5, 0));
    pm.addStudent(Student(103, "Rohit Verma",    "CSE", 7.8, 1));
    pm.addStudent(Student(104, "Sneha Gupta",    "IT",  8.9, 0));
    pm.addStudent(Student(105, "Karan Mehta",    "ME",  6.9, 2));
    pm.addStudent(Student(106, "Ananya Singh",   "CSE", 9.5, 0));
    pm.addStudent(Student(107, "Deepak Kumar",   "EEE", 7.2, 1));
    pm.addStudent(Student(108, "Pooja Reddy",    "IT",  8.1, 0));
    pm.addStudent(Student(109, "Arjun Nair",     "CSE", 8.7, 0));
    pm.addStudent(Student(110, "Divya Joshi",    "ECE", 7.5, 2));

    pm.addCompany(Company(1, "Google",    "Software", 8.5, 0, "ALL", 45.0));
    pm.addCompany(Company(2, "Infosys",   "IT",       6.5, 2, "ALL",  3.6));
    pm.addCompany(Company(3, "Qualcomm",  "Hardware", 7.5, 1, "ECE", 18.0));
    pm.addCompany(Company(4, "Microsoft", "Software", 8.0, 0, "CSE", 40.0));
    pm.addCompany(Company(5, "TCS",       "IT",       6.0, 2, "ALL",  3.5));
    cout << "\n  [INFO] Sample data loaded!\n";
}

// ============================================================
//  SECTION 9 — MENUS
// ============================================================

// ── Helper: print search results ──────────────────────────────
static void printSearchResults(const vector<Student>& res) {
    if (res.empty()) { cout << "  (No matching students found)\n"; return; }
    cout << "\n  Found " << res.size() << " result(s):\n";
    // inline header
    cout << left
         << setw(8)  << "Roll"
         << setw(22) << "Name"
         << setw(8)  << "Branch"
         << setw(7)  << "CGPA"
         << setw(9)  << "Backlogs"
         << setw(8)  << "Placed"
         << setw(20) << "Company"
         << "Package\n"
         << string(90, '=') << "\n";
    for (const auto& s : res) s.displayShort();
}

// ── Student Menu ──────────────────────────────────────────────
static void studentMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- Student Management ---\n"
             << " 1. Add Student\n"
             << " 2. Remove Student\n"
             << " 3. View All Students\n"
             << " 4. View Placed\n"
             << " 5. View Unplaced\n"
             << " 6. Undo Last Delete\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;
        switch (ch) {
            case 1: {
                int    r  = getInt("  Roll Number : ");
                string n  = getString("  Name        : ");
                string b  = getString("  Branch      : ");
                double c  = getDouble("  CGPA        : ");
                int    bl = getInt("  Backlogs    : ");
                if (n.empty() || b.empty()) {
                    cout << "  [!] Name and branch cannot be empty.\n"; break;
                }
                if (c < 0.0 || c > 10.0) {
                    cout << "  [!] CGPA must be between 0 and 10.\n"; break;
                }
                pm.addStudent(Student(r, n, b, c, bl));
                break;
            }
            case 2:
                pm.removeStudent(getInt("  Roll Number : "));
                break;
            case 3: pm.displayAll();     break;
            case 4: pm.displayPlaced();  break;
            case 5: pm.displayUnplaced();break;
            case 6: pm.undoDelete();     break;
            default: cout << "  [!] Invalid option.\n";
        }
    }
}

// ── Advanced Search Menu ──────────────────────────────────────
static void searchMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- Advanced Search ---\n"
             << " 1. Search by Roll Number\n"
             << " 2. Search by Name\n"
             << " 3. Search by Branch\n"
             << " 4. Search by CGPA Range\n"
             << " 5. Search by Placement Status\n"
             << " 6. Search by Company\n"
             << " 7. Search by Minimum Package\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;
        switch (ch) {
            case 1: {
                int roll = getInt("  Roll Number : ");
                Student* s = pm.searchByRoll(roll);
                if (s) s->display();
                else   cout << "  (Not found)\n";
                break;
            }
            case 2:
                printSearchResults(pm.searchByName(getString("  Name keyword : ")));
                break;
            case 3:
                printSearchResults(pm.searchByBranch(getString("  Branch : ")));
                break;
            case 4: {
                double lo = getDouble("  Min CGPA : ");
                double hi = getDouble("  Max CGPA : ");
                printSearchResults(pm.searchByCGPARange(lo, hi));
                break;
            }
            case 5: {
                cout << "  1. Placed   2. Unplaced\n  Choice: ";
                int p; cin >> p; clearIn();
                printSearchResults(pm.searchByPlacementStatus(p == 1));
                break;
            }
            case 6:
                printSearchResults(pm.searchByCompany(getString("  Company keyword : ")));
                break;
            case 7: {
                double pkg = getDouble("  Minimum Package (LPA) : ");
                printSearchResults(pm.searchByMinPackage(pkg));
                break;
            }
            default: cout << "  [!] Invalid option.\n";
        }
    }
}

// ── Advanced Sort Menu ────────────────────────────────────────
static void sortMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- Advanced Sort ---\n"
             << " 1. Sort by Name\n"
             << " 2. Sort by Roll Number\n"
             << " 3. Sort by CGPA\n"
             << " 4. Sort by Package\n"
             << " 5. Sort by Branch\n"
             << " 6. Sort by Placement Status\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;

        static const map<int, string> fieldMap = {
            {1,"name"},{2,"roll"},{3,"cgpa"},{4,"package"},{5,"branch"},{6,"placed"}
        };
        if (fieldMap.find(ch) == fieldMap.end()) {
            cout << "  [!] Invalid option.\n"; continue;
        }

        cout << "  Order: 1. Ascending   2. Descending\n  Choice: ";
        int ord; cin >> ord; clearIn();
        bool asc = (ord != 2);

        auto res = pm.sortStudents(fieldMap.at(ch), asc);
        cout << "\n  Sorted by " << fieldMap.at(ch)
             << (asc ? " (Ascending)" : " (Descending)") << ":\n";
        // print header inline
        cout << left
             << setw(8)  << "Roll"
             << setw(22) << "Name"
             << setw(8)  << "Branch"
             << setw(7)  << "CGPA"
             << setw(9)  << "Backlogs"
             << setw(8)  << "Placed"
             << setw(20) << "Company"
             << "Package\n"
             << string(90, '=') << "\n";
        for (const auto& s : res) s.displayShort();
    }
}

// ── Company Menu ──────────────────────────────────────────────
static void companyMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- Company Management ---\n"
             << " 1. Add Company\n"
             << " 2. View All Companies\n"
             << " 3. Sort Companies by Package\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;
        switch (ch) {
            case 1: {
                int    id  = getInt("  Company ID    : ");
                string n   = getString("  Name          : ");
                string d   = getString("  Domain        : ");
                double pkg = getDouble("  Package (LPA) : ");
                double mc  = getDouble("  Min CGPA      : ");
                int    mb  = getInt("  Max Backlogs  : ");
                string br  = getString("  Branch (or ALL): ");
                if (n.empty()) { cout << "  [!] Name cannot be empty.\n"; break; }
                pm.addCompany(Company(id, n, d, mc, mb, br, pkg));
                break;
            }
            case 2: pm.displayAllCompanies(); break;
            case 3: {
                cout << "  Order: 1. Ascending   2. Descending\n  Choice: ";
                int ord; cin >> ord; clearIn();
                auto sorted = pm.sortCompaniesByPackage(ord != 2);
                cout << "\n  Companies sorted by Package:\n";
                for (const auto& c : sorted) c.display();
                break;
            }
            default: cout << "  [!] Invalid option.\n";
        }
    }
}

// ── Eligibility Menu ──────────────────────────────────────────
static void eligibilityMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- Eligibility & Placement ---\n"
             << " 1. Eligible students for a company\n"
             << " 2. Eligible companies for a student\n"
             << " 3. Mark student as placed\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;
        switch (ch) {
            case 1: {
                pm.displayAllCompanies();
                int id = getInt("  Company ID : ");
                auto list = pm.getEligibleStudents(id);
                Company* c = pm.findCompany(id);
                if (c) cout << "\n  Eligible for " << c->getName()
                            << " (" << list.size() << "):\n";
                if (list.empty()) cout << "  (None)\n";
                else {
                    cout << left << setw(8) << "Roll" << setw(22) << "Name"
                         << setw(8) << "Branch" << "CGPA\n" << string(50,'-') << "\n";
                    for (const auto& s : list)
                        cout << setw(8) << s.getRollNumber()
                             << setw(22) << s.getName()
                             << setw(8)  << s.getBranch()
                             << fixed << setprecision(2) << s.getCGPA() << "\n";
                }
                break;
            }
            case 2: {
                int roll = getInt("  Roll Number : ");
                auto list = pm.getEligibleCompanies(roll);
                Student* s = pm.findStudent(roll);
                if (s) cout << "\n  Companies for " << s->getName()
                            << " (" << list.size() << "):\n";
                if (list.empty()) cout << "  (None)\n";
                else for (const auto& c : list) c.display();
                break;
            }
            case 3: {
                int    roll = getInt("  Roll Number   : ");
                string co   = getString("  Company Name  : ");
                double pkg  = getDouble("  Package (LPA) : ");
                pm.markPlaced(roll, co, pkg);
                break;
            }
            default: cout << "  [!] Invalid option.\n";
        }
    }
}

// ── CSV Menu ──────────────────────────────────────────────────
static void csvMenu(PlacementManager& pm) {
    while (true) {
        cout << "\n--- CSV Import / Export ---\n"
             << " 1. Import Students from students.csv\n"
             << " 2. Export Placement Report\n"
             << " 3. Export Student Report\n"
             << " 4. Export Company Report\n"
             << " 0. Back\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        if (ch == 0) break;
        switch (ch) {
            case 1: pm.importFromCSV("students.csv"); break;
            case 2: CSVManager::exportPlacementReport(pm.getStudents()); break;
            case 3: CSVManager::exportStudentReport(pm.getStudents());   break;
            case 4: CSVManager::exportCompanyReport(pm.getCompanies());  break;
            default: cout << "  [!] Invalid option.\n";
        }
    }
}

// ============================================================
//  SECTION 10 — MAIN
// ============================================================

int main() {
    // ── Authentication ────────────────────────────────────────
    if (!Authentication::login()) return 1;

    // ── Load persistent data & show banner ────────────────────
    PlacementManager pm;

    cout << "\n";
    printLine('=', 60);
    cout << "       PLACEMENT MANAGEMENT SYSTEM  v2.0\n"
         << "       Modern C++ | OOP | STL | File I/O\n";
    printLine('=', 60);

    // Offer sample data only if nothing is persisted
    if (pm.getStudents().empty() && pm.getCompanies().empty()) {
        cout << "\n  No existing data found.\n"
             << "  Load sample data? (1=Yes / 0=No): ";
        int ch; cin >> ch; clearIn();
        if (ch == 1) loadSample(pm);
    }

    // ── Main Loop ─────────────────────────────────────────────
    while (true) {
        cout << "\n";
        printLine('=', 45);
        cout << "              MAIN MENU\n";
        printLine('=', 45);
        cout << "  1. Student Management\n"
             << "  2. Company Management\n"
             << "  3. Eligibility & Placement\n"
             << "  4. Advanced Search\n"
             << "  5. Advanced Sort\n"
             << "  6. Statistics\n"
             << "  7. CSV Import / Export\n"
             << "  0. Exit\n"
             << "Choice: ";
        int ch; cin >> ch; clearIn();
        switch (ch) {
            case 0: cout << "\n  Goodbye! Data saved to disk.\n\n"; return 0;
            case 1: studentMenu(pm);    break;
            case 2: companyMenu(pm);    break;
            case 3: eligibilityMenu(pm);break;
            case 4: searchMenu(pm);     break;
            case 5: sortMenu(pm);       break;
            case 6: pm.displayStats();  break;
            case 7: csvMenu(pm);        break;
            default: cout << "  [!] Invalid choice. Enter 0-7.\n";
        }
    }
}
