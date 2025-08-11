# MiniRedis – A Simplified Redis-Style In-Memory Database in C

## Overview
**MiniRedis** is a lightweight, Redis-inspired in-memory database server implemented in C.  
Unlike Redis’s flat keyspace, MiniRedis uses a hierarchical tree-based key-value store, giving it a filesystem like structure for intuitive data organization.  
This project demonstrates core systems programming concepts including custom data structures, memory management, and command parsing.

## Features
- **Hierarchical Key-Value Storage** – Nodes as directories, leaves as key-value pairs
- **Redis-Style Commands** – Familiar CRUD operations (`SET`, `GET`, `DEL`) plus directory navigation (`MKDIR`, `CD`, `LS`, `PWD`)
- **Text-Based Protocol** – Human-readable command interface
- **Memory Management** – Manual allocation/deallocation with leak prevention
- **Single-Threaded Event Loop** – Predictable and simple execution model
