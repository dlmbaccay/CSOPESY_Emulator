#pragma once

#include "ConfigManager.h"
#include <vector>
#include <unordered_map>
#include "Process.h"
#include <mutex>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <random>
#include <deque>
#include <queue>
#include <unordered_set>

class MemoryAllocator {
public:
  MemoryAllocator(ConfigManager* configManager);
  ~MemoryAllocator();

  void showFlatMemory();
  void showPagingMemory();
  bool allocateMemory(Process* process);
	void deallocateMemory(Process* process);

  bool isProcessInMemory(Process* process) const;
  bool isFlatAllocation() const;

  int numPagesIn = 0;
  int numPagesOut = 0;

  std::unordered_set<std::string> backingStoreSet;
  Process* loadProcessFromBackingStore(const std::string& processName);

  void showProcessSMI(double cpuUtil);
  void showVmStat(int idleCpuTicks, int activeCpuTicks);

private:
  int maxOverallMem;     // Max overall memory
  int memPerFrame;       // Memory per frame
  int minMemPerProcess;  // Minimum memory per process
  int maxMemPerProcess;  // Maximum memory per process

  // Flat memory allocation members
  struct ProcessAllocated {
    Process* process;
		int startAddressIndex;
    int endAddressIndex;
		time_t allocationTime;
  };

	std::vector<bool> memorySlots;
  std::vector<int> memoryAddresses; // Vector to store memory addresses
  std::vector<int> allocatedAddresses; // Vector to track allocated addresses
  std::unordered_map<std::string, ProcessAllocated> processMap;

  bool allocateFlatMemory(Process* process);
  void removeFlatMemory(Process* process);

  struct Page {
    Process* process;
    int pageNumber;
  };

  std::deque<Page> frameList; // Deque to represent the frame list
  std::unordered_map<std::string, std::pair<time_t, std::vector<int>>> processPageMap;  // Store page numbers for each process

  bool allocatePagingMemory(Process* process);
  void removePagingMemory(Process* process);

  const std::string backingStorePath = "backing_store"; // Path for backing store files
  

  void saveProcessToBackingStore(Process* process);
};