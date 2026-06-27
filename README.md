# 🎓 Placement Management System

> A production-quality console application for managing campus placements, built in Modern C++ with full OOP, persistent storage, CSV processing, and admin authentication.

**Author:** Shiva  
**Institution:** IIT Goa  
**Language:** C++ (C++11 standard)  
**Compiler:** GCC / TDM-GCC (Dev-C++ compatible)

---

## 📌 Project Overview

The **Placement Management System** is a fully-featured command-line application that digitizes and automates the campus placement process. It allows placement officers to manage student records, company details, eligibility filtering, placement tracking, and generate detailed reports — all from a secure, menu-driven console interface.

This project demonstrates strong command over **Object-Oriented Programming**, **STL**, **File I/O**, **Data Structures**, and **Software Design** — making it ideal for a Software Engineering internship portfolio.

---

## ✨ Features

### 🔐 Admin Authentication
- Secure login with username and password
- Password stored as a **djb2 hash** — never in plaintext
- 3-attempt lockout system

### 👨‍🎓 Student Management
- Add, view, and delete students
- View placed / unplaced students separately
- **Undo Last Delete** — restore accidentally deleted records

### 🏢 Company Management
- Add and view recruiting companies
- Define eligibility criteria (min CGPA, max backlogs, branch, package)
- Sort companies by package offered

### ✅ Eligibility & Placement
- Auto-filter eligible students for any company
- Auto-filter eligible companies for any student
- Mark students as placed with company name and package

### 🔍 Advanced Search (7 modes)
- By Roll Number
- By Name (substring, case-insensitive)
- By Branch
- By CGPA Range
- By Placement Status
- By Company Name
- By Minimum Package

### 📊 Advanced Sort (6 fields × 2 directions)
- Sort by Name, Roll, CGPA, Package, Branch, Placement Status
- Ascending or Descending

### 📈 Rich Statistics
- Total / Placed / Unplaced students
- Placement percentage
- Highest, Lowest, Average Package
- Highest, Lowest, Average CGPA
- **Branch-wise placement percentage**
- **Most recruiting company**

### 💾 Persistent Storage
- Data auto-saved to `students.txt` and `companies.txt`
- Auto-loaded on every startup — no data loss on restart
- Graceful handling of corrupt or missing files

### 📂 CSV Import / Export
- Import students from `students.csv`
- Export `placement_report.csv`, `student_report.csv`, `company_report.csv`
- Invalid CSV rows skipped gracefully with warnings

---

## 🗂️ Folder Structure

```
placement-management-system/
│
├── placement_system.cpp     # Full source code (single file)
├── README.md                # Project documentation
├── Makefile                 # Build automation
├── students.csv             # Sample CSV for import testing
├── .gitignore               # Ignores binaries and data files
│
# Generated at runtime:
├── students.txt             # Persistent student storage
├── companies.txt            # Persistent company storage
├── placement_report.csv     # Placed students export
├── student_report.csv       # All students export
└── company_report.csv       # All companies export
```

---

## 🧠 OOP Concepts Used

| Concept | Where Used |
|---|---|
| **Encapsulation** | `Student`, `Company`, `PlacementManager`, `Authentication` classes with private fields and public getters/setters |
| **Abstraction** | `FileManager` and `CSVManager` hide all I/O complexity |
| **Composition** | `PlacementManager` owns `vector<Student>`, `vector<Company>`, `stack<Student>` |
| **RAII** | `fstream` objects opened and destroyed within each method scope |
| **const-correctness** | All read-only methods marked `const`; getters return `const` references |

---

## 📚 STL Used

| Container / Algorithm | Purpose |
|---|---|
| `std::vector<T>` | Primary storage for students and companies |
| `std::stack<Student>` | Undo-delete buffer |
| `std::unordered_map<string, int>` | Branch-wise stats, company recruitment count — O(1) average lookup |
| `std::map<int, string>` | Sort field mapping in menu |
| `std::sort` | Generic bidirectional sorting with lambda comparators |
| `std::copy_if` | Filtering students by search criteria |
| `std::count_if` | Counting placed students |
| `std::transform` | `toLower()` utility |
| `std::fstream` | Persistent file I/O |
| `std::stringstream` | CSV parsing and serialization |
| `std::iomanip` | Formatted table output |

---

## ⚙️ Algorithms Used

### Searching
| Algorithm | Time | Space | Where |
|---|---|---|---|
| Linear Search | O(N) | O(1) | Find by roll / company ID |
| Substring Search | O(N·M) | O(K) | Search by name / company keyword |
| Filter Scan | O(N) | O(K) | Search by branch, CGPA, status, package |

### Sorting
| Algorithm | Time | Space | Where |
|---|---|---|---|
| `std::sort` (Introsort) | O(N log N) | O(log N) | All sort operations |
| Lambda Comparator | O(1) per compare | O(1) | Custom field + direction sorting |

### Hashing
| Algorithm | Time | Space | Where |
|---|---|---|---|
| djb2 Hash | O(N) | O(1) | Password hashing in Authentication |

---

## 🚀 How to Run

### Option 1 — Dev-C++ (Windows)
1. Open `placement_system.cpp` in Dev-C++
2. Press **F11** to Compile & Run
3. Login: `admin` / `admin123`

### Option 2 — GCC (Linux / Mac / Windows terminal)
```bash
# Compile
g++ -std=c++11 -o pms placement_system.cpp

# Run
./pms

# Login
Username: admin
Password: admin123
```

### Option 3 — Makefile
```bash
make
./pms
```

### CSV Import Test
Place a `students.csv` in the same folder:
```csv
Roll,Name,Branch,CGPA,Backlogs
201,Rahul Singh,CSE,8.9,0
202,Neha Sharma,ECE,7.5,1
```
Then choose **CSV Import / Export → Import Students**.

---

## 📸 Screenshots

> _Add screenshots of your running application here._

| Login Screen | Main Menu | Statistics |
|---|---|---|
| _(screenshot)_ | _(screenshot)_ | _(screenshot)_ |

---

## 🔮 Future Improvements

- [ ] GUI using Qt or SFML
- [ ] MySQL / SQLite database backend
- [ ] Multi-user role support (Student / Company / Admin)
- [ ] Email notification system for placement drives
- [ ] Web dashboard using REST API
- [ ] Resume scoring and auto-shortlisting using AI

---

## 📄 Resume Bullet Points

Use these directly on your resume:

- Developed a **Placement Management System** in C++ (2000+ lines) using OOP, STL, and file I/O with persistent storage and CSV processing
- Implemented **Admin Authentication** using djb2 password hashing with 3-attempt lockout and session management
- Built **7 advanced search modes** and **6-field bidirectional sorting** using STL `copy_if`, `sort`, and lambda comparators — O(N log N)
- Designed **branch-wise placement statistics** and company recruitment analytics using `unordered_map` for O(1) average lookup
- Implemented **Undo-Delete** using `std::stack` and CSV import/export pipeline with graceful error handling and row validation

---

## 👨‍💻 Author

**Shiva**  
B.Tech Student — IIT Goa  
[GitHub](https://github.com/) • [LinkedIn](https://linkedin.com/)

---

*Built with ❤️ at IIT Goa*
