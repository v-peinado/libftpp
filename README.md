# libftpp - Advanced C++ Toolbox

A comprehensive C++ library implementing advanced data structures, design patterns, and utilities for high-performance applications. This library provides a collection of reusable, efficient, and thread-safe components designed to accelerate C++ development.

## 📋 Table of Contents

- [Overview](#overview)
- [Building the Library](#building-the-library)
- [Modules](#modules)
  - [Data Structures](#data-structures)
  - [Design Patterns](#design-patterns)
  - [IOStream Utilities](#iostream-utilities)
- [Usage](#usage)
- [Testing](#testing)
- [Project Structure](#project-structure)

## 🎯 Overview

libftpp is a static library (`libftpp.a`) that provides:
- **Type-safe data structures** for efficient memory management
- **Gang of Four design patterns** for robust software architecture
- **Thread-safe utilities** for concurrent programming
- **Modern C++** features with C++11+ compatibility

The library is designed with performance, safety, and ease of use in mind, making it suitable for both educational purposes and production environments.

## 🔨 Building the Library

```bash
# Build the static library
make

# Clean build artifacts
make clean

# Complete rebuild
make re
```

This generates `libftpp.a` and provides a unified header `libftpp.hpp` for easy integration.

## 📚 Modules

### Data Structures

#### Pool & Pool::Object
**Purpose**: Efficient object pool management with automatic lifecycle handling.

**Objective**: Eliminate frequent memory allocation/deallocation overhead by pre-allocating objects and reusing them through RAII wrappers.

**Key Features**:
- Template-based for any object type
- RAII-compliant `Pool::Object` wrapper for automatic return-to-pool
- Exception-safe operations with strong guarantees
- Variadic template support for object construction

**Use Cases**: Game engines, real-time systems, high-frequency trading applications where memory allocation patterns are predictable.

#### DataBuffer
**Purpose**: Polymorphic binary serialization container.

**Objective**: Provide a type-safe, stream-like interface for serializing/deserializing objects to/from binary format.

**Key Features**:
- Template operators for seamless serialization (`<<`, `>>`)
- Built-in support for primitive types and `std::string`
- Extensible for custom types through operator overloading
- Exception handling for buffer overflow scenarios

**Use Cases**: Network protocols, file I/O, inter-process communication, game state serialization.

### Design Patterns

#### Observer Pattern
**Purpose**: Event-driven programming with type-safe event handling.

**Objective**: Implement the classic Observer pattern for decoupled communication between components.

**Key Features**:
- Template-based event types for compile-time safety
- Multiple subscribers per event type
- Lambda function support for flexible callback definitions
- Exception propagation from callbacks

**Use Cases**: GUI frameworks, game event systems, model-view architectures, plugin systems.

#### Singleton Pattern
**Purpose**: Controlled single-instance creation with lazy initialization.

**Objective**: Ensure only one instance of a class exists while providing global access point.

**Key Features**:
- Template-based for any class type
- Variadic template constructor support
- Exception handling for invalid usage patterns
- Memory-safe automatic cleanup

**Use Cases**: Configuration managers, logging systems, database connections, resource managers.

#### State Machine
**Purpose**: Finite state machine implementation with action and transition management.

**Objective**: Provide a robust framework for managing complex state-dependent behavior.

**Key Features**:
- Template-based state types
- Action callbacks for state entry/execution
- Transition callbacks for state changes
- Comprehensive error handling for invalid transitions

**Use Cases**: Game AI, protocol implementations, workflow engines, device control systems.

#### Memento Pattern
**Purpose**: Object state capture and restoration without exposing internal structure.

**Objective**: Enable undo/redo functionality and state persistence while maintaining encapsulation.

**Key Features**:
- Binary serialization through `DataBuffer` integration
- Stream operators for intuitive state management
- Inheritance-based design for clean integration
- Exception-safe state operations

**Use Cases**: Undo systems, game save states, configuration snapshots, transactional operations.

### IOStream Utilities

#### ThreadSafeIOStream
**Purpose**: Thread-safe I/O operations with automatic line prefixing.

**Objective**: Eliminate race conditions in multi-threaded console output while providing thread identification.

**Key Features**:
- Global synchronization for all I/O operations
- Thread-local prefix management
- Stream operator overloading for familiar interface
- Prompt functionality for interactive applications
- Global `threadSafeCout` instance for easy adoption

**Use Cases**: Multi-threaded applications, debugging concurrent systems, server applications, parallel computing frameworks.

## 🚀 Usage

### Basic Integration
Include the main header in your project:
```cpp
#include "libftpp.hpp"
```

Link against the static library:
```bash
g++ your_code.cpp libftpp.a -o your_program
```

### Example Applications

**Object Pool for Game Entities**:
Manage bullet objects in a game without dynamic allocation during gameplay.

**Event System for GUI Applications**:
Use Observer pattern to handle button clicks, window events, and user interactions.

**State Machine for Protocol Implementation**:
Manage connection states (connecting, connected, disconnecting) with proper transitions.

**Thread-Safe Logging**:
Use ThreadSafeIOStream for coordinated output from multiple worker threads.

**Undo/Redo Systems**:
Implement Memento pattern for document editors or configuration tools.

## 🧪 Testing

The library includes comprehensive tests for all modules:

```bash
cd tests
make all_tests          # Run all tests
make test_pool          # Test object pool
make test_observer      # Test observer pattern
make test_state_machine # Test state machine
make test_memento       # Test memento pattern
make test_data_buffer   # Test data buffer
```

Each test demonstrates practical usage scenarios and validates functionality.

## 📁 Project Structure

```
libftpp/
├── include/
│   ├── data_structures/     # Pool, DataBuffer headers
│   ├── design_patterns/     # Observer, Singleton, StateMachine, Memento
│   ├── iostream/           # ThreadSafeIOStream
│   └── *.hpp              # Module aggregation headers
├── src/
│   ├── data_structures/     # Implementation files
│   ├── design_patterns/     # Implementation files
│   └── iostream/           # Implementation files
├── tests/
│   └── src/               # Test applications
├── Makefile               # Main build configuration
└── libftpp.hpp           # Unified public API
```

## 🎯 Design Philosophy

- **Performance**: Zero-overhead abstractions where possible
- **Safety**: RAII, exception safety, and strong type checking
- **Usability**: Intuitive interfaces following C++ idioms
- **Modularity**: Independent components with clear dependencies
- **Testability**: Comprehensive test coverage for reliability

## 📄 License

This project is part of an advanced C++ curriculum focusing on practical implementation of computer science concepts and modern C++ techniques.