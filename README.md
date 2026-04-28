# 📚 Student Management System

A complete desktop application built with **C++ and Qt Framework** — featuring a dark professional UI, student records management, grade calculation, CSV export/import, and real-time preview.

> Built by [@haroonmughalgfx](https://github.com/haroonmughalgfx)

---

## ✨ Features

- **Add Students** — Name, Roll Number, and 5 subject marks
- **Live Grade Preview** — Total, Percentage & Grade updates in real-time as you type
- **Records Table** — View all students with color-coded grades
- **Search** — Filter by Name or Roll Number instantly
- **Delete** — Remove records with confirmation dialog
- **Save to CSV** — Export all student data to a `.csv` file
- **Load from CSV** — Import previously saved data back
- **Class Statistics** — Total students, class average, and pass count
- **Dark Theme** — Professional dark UI with red accent

---

## 🗂️ Project Structure

```
StudentMS/
├── main.cpp           # Application entry point
├── mainwindow.h       # Class declarations + Student struct
├── mainwindow.cpp     # Full UI logic + stylesheet
└── StudentMS.pro      # Qt project config file
```

---

## 🛠️ Requirements

| Tool | Version |
|---|---|
| Qt Framework | 6.x (or 5.15+) |
| Qt Creator IDE | Latest |
| C++ Standard | C++17 |
| OS | Windows / Linux / macOS |

---

## 🚀 How to Run

### Step 1 — Install Qt
Download and install Qt from the official website:
```
https://qt.io/download-open-source
```
Select: **Qt 6** + **Qt Creator** during installation.

### Step 2 — Clone the Repository
```bash
git clone https://github.com/your-username/StudentMS.git
cd StudentMS
```

### Step 3 — Open in Qt Creator
```
File → Open Project → StudentMS.pro
```

### Step 4 — Build & Run
```
Press Ctrl+R  (or click the green ▶ Play button)
```

---

## ⚠️ Common Error Fix

If you see this error:
```
error: [Makefile.Debug:75: debug/StudentMS_resource_res.o] Error 1
```

Make sure `RC_ICONS = app.ico` line is **removed** from `StudentMS.pro`.  
The `.pro` file should only contain:

```pro
QT += core gui widgets
CONFIG += c++17
TARGET = StudentMS
TEMPLATE = app

SOURCES += main.cpp mainwindow.cpp
HEADERS += mainwindow.h
```

Then do: **Build → Clean All → Run**

---

## 📊 Grade Scale

| Percentage | Grade |
|---|---|
| 90% and above | A+ |
| 80% – 89% | A |
| 70% – 79% | B |
| 60% – 69% | C |
| 50% – 59% | D |
| Below 50% | F |

---

## 📁 CSV Format

When saving, the file is exported in this format:

```
Roll No,Name,Math,Physics,English,Programming,Islamiat,Percentage,Grade
CS-001,Ali Hassan,85,90,78,92,88,86.6,A
CS-002,Sara Khan,70,65,80,75,72,72.4,B
```

---

## 📦 Dependencies

Only **Qt Framework** — no third-party libraries required.

---

## 📄 License

This project is open source and free to use for learning purposes.

---

## 🙋‍♂️ Author

**Haroon Mughal**  
Graphic Designer & Developer  
📧 haroonmughalgfx@gmail.com  
🎨 [@haroonmughalgfx](https://instagram.com/haroonmughalgfx)
