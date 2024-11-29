#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <ctime>
#include <unordered_map>


class MemoryAllocator {
private:
  struct Frame {
    int processID = -1;          // Process ID that occupies the frame
    bool isOccupied = false;     // Is the frame occupied?
    std::time_t lastAccessed = 0; // Timestamp for the last access (for paging and memory management)
  };

  std::vector<Frame> memoryFrames;
  size_t max_overall_mem;  // Total memory available for all processes
  size_t mem_per_frame;    // Size of each memory frame
  size_t min_mem_per_proc; // Minimum memory required for each process
  size_t max_mem_per_proc; // Maximum memory allowed for each process
  std::string allocationType;  // Allocation type: "flat" or "paging"
  size_t pagedInCount = 0;
  size_t pagedOutCount = 0;


  std::string getCurrentTime() const;  // For generating snapshots
  std::unordered_map<int, std::vector<int>> backingStore; // Backing store for swapped-out pages


public:
  // Constructor to initialize memory parameters
  MemoryAllocator(size_t max_overall_mem, size_t mem_per_frame, size_t min_mem_per_proc, size_t max_mem_per_proc);

  // Memory allocation
  bool allocateMemory(int processID);

  // Free memory
  void freeMemory(int processID);

  // Memory management utilities
  int calculateNumberofProcesses() const;         // Returns the number of unique processes in memory
  size_t getProcessMemory(int processID) const;   // Calculates memory used by a specific process
  size_t calculateUsedMemory() const;  // Calculates the total memory currently in use

  size_t getPagedInCount() const;
  size_t getPagedOutCount() const;

  // Getters and setters
  size_t getMaxOverallMem() const;
  void setMaxOverallMem(size_t maxOverallMem);

  // Helper functions
  int findFreeFrames(size_t requiredFrames) const;
  int findOldestProcess() const;  // For swapping out the oldest process

  size_t getMemPerFrame() const;
  void setMemPerFrame(size_t memPerFrame);

  size_t getMinMemPerProc() const;
  void setMinMemPerProc(size_t minMemPerProc);

  size_t getMaxMemPerProc() const;

  const std::vector<Frame>& getMemoryFrames() const;
  void setMemoryFrames(const std::vector<Frame>& frames);

  const std::string getAllocationType() const;
  void setAllocationType(const std::string& type);

  void moveToBackingStore(int processID);
  void restoreFromBackingStore(int processID);
};