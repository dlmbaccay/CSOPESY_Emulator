# CSOPESY OS Emulator

This project implements a Process Scheduler with a Command Line Interface (CLI). The scheduler supports multiple scheduling algorithms and provides system-level operations for managing processes. This application is designed to simulate how processes are scheduled within an operating system, providing insight into CPU scheduling algorithms and allowing users to interact with the scheduler through a simple CLI.

## Members

1. Aquino, Karl Andre
2. Baccay, Dominic Luis
3. Miranda, Bien Aaron

## Getting Started with the Emulator

1. Download the files from this repository or clone the repository to your machine
2. Compile the program
3. Run the application

### Usage

Once the program is running, you can use the following commands to interact with the emulator and process scheduler:

-   **`initialize`**  
    Initializes the system and prepares the environment for further operations. This must be run before starting any session or scheduler test.

    ```bash
     initialize
    ```

    Output:  
    ` Processor configuration initialized.`

-   **`screen -s <session_name>`**  
    Starts a new session with the given name.

    ```bash
     screen -s example_session
    ```

-   **`screen -r <session_name>`**  
    Reattaches to an existing session.

    ```bash
     screen -r example_session
    ```

    Output:  
    Reattaches and switches to the session `example_session`.

-   **`screen -ls`**  
    Lists all active sessions.

    ```bash
     screen -ls
    ```

-   **`scheduler-test`**  
    Starts the creation of dummy processes for testing the scheduling functionality.

    ```bash
     scheduler-test
    ```

    Output:  
    ` Creating dummy processes`

-   **`scheduler-stop`**  
    Stops the creation of dummy processes.

    ```bash
     scheduler-stop
    ```

    Output:  
    ` Stopping creation of dummy processes`

-   **`report-util`**  
    Generates and displays a utilization report of the system.

    ```bash
     report-util
    ```

-   **`clear`**  
    Clears the screen and re-displays the header.

    ```bash
     clear
    ```

-   **`exit`**  
    Exits the current session if inside one, or exits the emulator if not in a session.

    ```bash
     exit
    ```

    Output:  
    If in a session: ` Exiting session...`  
    If outside a session: ` Exiting emulator...`

-   **`help`**  
    Displays the list of available commands.

    ```bash
     help
    ```

-   **`process-smi`**
    Prints simple information of the process. Applicable when attached to a process.

    ```bash
     process-smi
    ```

---

## File Structure

```
│
├── CSOPESY_emulator.cpp       # Entry point for the application
├── ConfigManager.cpp          # Handles the processor configuration
├── ConfigManager.h            # Header file for ConfigManager
├── ConsoleManager.cpp         # Handles console input/output and UI interactions
├── ConsoleManager.h           # Header file for ConsoleManager
├── Process.cpp                # Defines the structure and behavior of a process
├── Process.h                  # Header file for Process
├── Scheduler.cpp              # Implements the scheduling algorithm (e.g., FCFS, SJF, Priority)
├── Scheduler.h                # Header file for scheduler
├── MemoryAllocator.cpp        # Manages memory allocation, deallocation, and usage per quantum cycle
├── MemoryAllocator.h          # Header file for MemoryAllocator
└── README.md                  # Project documentation
```