# Quiz TCP Client–Server

A university project implementing a TCP-based multi-player quiz game, featuring:  

- 🎮 Multiplayer quiz rounds  
- ⏱️ Timed questions with automatic cut-off  
- 🏆 Real-time score tracking and leaderboards  

---

## 📌 Features

- **Server–Client Architecture**  
  - Server accepts and manages multiple simultaneous TCP connections.  
  - Clients register with a nickname and participate in quiz sessions.

- **Custom Quiz Protocol**  
  - Defines clear message formats for questions, answers, acknowledgments, and score updates.  
  - Ensures reliable, ordered delivery over TCP.

- **Timed Questioning**  
  - Each question has a configurable time limit.  
  - Late or missing answers are automatically marked as incorrect.

- **Score Management**  
  - Tracks per-player scores in real time.  
  - Persists cumulative results in a text file (`score.txt`).

- **Configurable Content**  
  - Questions and answers read from `quiz.txt`.  
  - Allowed nicknames stored in `nicknames.txt`.

---

## 📊 Technologies

- C (C99 standard)  
- POSIX sockets over TCP  
- `pthread` library for handling multiple clients  
- Bash helper script for quick demo runs

---

## 📂 Project Contents

- `server.c` & `client.c` – Core TCP server/client code  
- `quiz.txt` & `nicknames.txt` – Quiz questions and allowed nicknames  
- `score.txt` – Stored player scores  
- `script.sh` – Quick-start demo script  
- `Progetto reti informatiche.pdf` – Project report (Italian)  
- `specifiche progetto 2024-2025.pdf` – Functional specs

---

## 🏫 Academic Context

**University of Pisa**  
**Course**: Computer Networks
**Year**: 2025 
**Authors**: Giovanni Dipace

---

## 📄 License

This project is provided for educational and portfolio purposes under non-commercial use.  
