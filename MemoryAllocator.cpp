#include "MemoryAllocator.h"
#include <fstream>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

// Constructor: initializes memory frames based on total memory size and frame size.
MemoryAllocator::MemoryAllocator(size_t max_overall_mem, size_t mem_per_frame, size_t min_mem_per_proc, size_t max_mem_per_proc)
  :
  max_overall_mem(max_overall_mem),
  mem_per_frame(mem_per_frame),
  min_mem_per_proc(min_mem_per_proc),
  max_mem_per_proc(max_mem_per_proc)
{
  size_t numFrames = max_overall_mem / mem_per_frame;
  memoryFrames.resize(numFrames, { -1, false });  // Initialize frames as unoccupied

  if (max_overall_mem == mem_per_frame) {
    allocationType = "paging";
  }
  else {
    allocationType = "flat";
  }
}

// Allocate memory for a process using the first-fit method.
bool MemoryAllocator::allocateMemory(int processID) {
  // Determine a random memory size for the process between min and max memory required
  size_t processMemory = min_mem_per_proc + rand() % (max_mem_per_proc - min_mem_per_proc + 1);
  size_t requiredPages = (processMemory + mem_per_frame - 1) / mem_per_frame; // Correctly calculate required pages

  // Ensure we have enough memory to allocate the process
  if (requiredPages * mem_per_frame > max_overall_mem) {
    return false; // Not enough memory to allocate the process
  }

  if (allocationType == "paging") {
    // Paging allocation logic
    int startFrame = findFreeFrames(requiredPages);

    if (startFrame == -1) {
      // If no free frames are available, we need to swap out an old process to the backing store
      int oldestProcessID = findOldestProcess();
      if (oldestProcessID != -1) {
        moveToBackingStore(oldestProcessID); // Move oldest process to backing store
        freeMemory(oldestProcessID);        // Free its memory in physical frames
      }

      // Try to allocate memory again after swapping out
      startFrame = findFreeFrames(requiredPages);
      if (startFrame == -1) {
        return false; // Still no free frames available after swapping out
      }
    }

    // Restore from backing store if process was previously swapped out
    if (backingStore.find(processID) != backingStore.end()) {
      restoreFromBackingStore(processID); // Restore pages for the process
    }

    // Allocate memory for the process (load the pages into memory)
    for (size_t i = startFrame; i < startFrame + requiredPages; ++i) {
      memoryFrames[i] = { processID, true, std::time(0) }; // Mark the page as occupied and set lastAccessed timestamp
      pagedInCount++;
    }
  }
  else { // Flat memory allocation
    // Flat allocation logic: Process memory must fit within contiguous frames
    size_t requiredFrames = (processMemory + mem_per_frame - 1) / mem_per_frame;

    // Ensure process memory doesn't exceed the available space
    if (requiredFrames * mem_per_frame > max_overall_mem) {
      return false; // Not enough memory to allocate the process
    }

    // Find a contiguous block of free frames for the process
    int startFrame = findFreeFrames(requiredFrames);

    if (startFrame == -1) {
      // If no contiguous space is available, swap out the oldest process
      int oldestProcessID = findOldestProcess();
      if (oldestProcessID != -1) {
        moveToBackingStore(oldestProcessID); // Move oldest process to backing store
        freeMemory(oldestProcessID);        // Free its memory in physical frames
      }

      // Try again after swapping out
      startFrame = findFreeFrames(requiredFrames);
      if (startFrame == -1) {
        return false; // Still no contiguous space available
      }
    }

    // Allocate memory for the process (mark frames as occupied)
    for (size_t i = startFrame; i < startFrame + requiredFrames; ++i) {
      memoryFrames[i] = { processID, true, std::time(0) }; // Mark the frame as occupied and set lastAccessed timestamp
      pagedInCount++;
    }
  }

  return true;
}


void MemoryAllocator::freeMemory(int processID) {
  size_t freedPages = 0; // Track the number of pages freed

  for (auto& frame : memoryFrames) {
    //std::cout << "Frame process ID: " << frame.processID << ", Target process ID: " << processID << "\n";

    if (frame.processID == processID) {
      frame.isOccupied = false;  // Mark frame as free
      frame.processID = -1;      // Reset the process ID
      frame.lastAccessed = 0;    // Clear last accessed time
      freedPages++;              // Increment the count of freed pages
    }
  }
  //cout << freedPages << endl;
  if (freedPages > 0) {
    pagedOutCount += freedPages; // Update the total number of pages paged out
  }

  // Move the process's pages to the backing store
  std::vector<int> swappedPages;
  for (size_t i = 0; i < memoryFrames.size(); ++i) {
    if (memoryFrames[i].processID == -1) {
      swappedPages.push_back(i);
    }
  }

  if (!swappedPages.empty()) {
    backingStore[processID] = swappedPages;
    // Optional: Log this action
    //std::cout << "Process " << processID << " moved to backing store. Pages freed: " << freedPages << "\n";
  }
}



size_t MemoryAllocator::getPagedInCount() const { return pagedInCount; }
size_t MemoryAllocator::getPagedOutCount() const { return pagedOutCount; }

// Find the starting frame index for the first contiguous block of free frames.
int MemoryAllocator::findFreeFrames(size_t requiredFrames) const {
  if (allocationType == "flat") {
    // Flat memory allocation (requires contiguous blocks of memory)
    size_t consecutiveFreeFrames = 0;
    int startFrame = -1;

    for (size_t i = 0; i < memoryFrames.size(); ++i) {
      if (!memoryFrames[i].isOccupied) {
        if (startFrame == -1) startFrame = i;
        consecutiveFreeFrames++;
        if (consecutiveFreeFrames == requiredFrames) {
          return startFrame;
        }
      }
      else {
        consecutiveFreeFrames = 0;
        startFrame = -1;
      }
    }
  }
  else if (allocationType == "paging") {
    // Paging memory allocation (non-contiguous pages)
    size_t freeFramesFound = 0;
    int startFrame = -1;

    for (size_t i = 0; i < memoryFrames.size(); ++i) {
      if (!memoryFrames[i].isOccupied) {
        if (freeFramesFound == 0) {
          startFrame = i; // Mark the start of the free frames
        }
        freeFramesFound++;

        // If we've found the required number of free frames, return the start index
        if (freeFramesFound == requiredFrames) {
          return startFrame;
        }
      }
    }
  }

  return -1; // No sufficient free frames available
}

// Find the process that has been in memory the longest (oldest process)
int MemoryAllocator::findOldestProcess() const {
  int oldestProcessID = -1;
  std::time_t oldestAccessTime = std::time_t(0); // Initialize to epoch time (oldest)

  // Loop through all memory frames to find the process with the oldest access time
  for (const auto& frame : memoryFrames) {
    if (frame.isOccupied) {
      // If we haven't found a process yet or if this one was accessed earlier
      if (oldestProcessID == -1 || frame.lastAccessed < oldestAccessTime) {
        oldestProcessID = frame.processID;
        oldestAccessTime = frame.lastAccessed;
      }
    }
  }

  return oldestProcessID; // Return the process ID of the oldest process
}

// Get the current system time for snapshot
std::string MemoryAllocator::getCurrentTime() const {
  std::time_t now = std::time(0);
  std::tm localTime;
  localtime_s(&localTime, &now);
  char buffer[50];
  std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", &localTime);
  return buffer;
}

int MemoryAllocator::calculateNumberofProcesses() const {
  std::unordered_set<int> uniqueProcesses;

  // Count unique process IDs in memory
  for (const auto& frame : memoryFrames) {
    if (frame.isOccupied) {
      uniqueProcesses.insert(frame.processID);  // Insert unique process IDs
    }
  }

  // Return the count of unique processes
  return uniqueProcesses.size();
}

size_t MemoryAllocator::getMaxOverallMem() const {
  return max_overall_mem;
}

void MemoryAllocator::setMaxOverallMem(size_t max_overall_mem) {
  max_overall_mem = max_overall_mem;
}

size_t MemoryAllocator::getMemPerFrame() const {
  return mem_per_frame;
}

void MemoryAllocator::setMemPerFrame(size_t mem_per_frame) {
  mem_per_frame = mem_per_frame;
}

size_t MemoryAllocator::getMinMemPerProc() const {
  return min_mem_per_proc;
}

void MemoryAllocator::setMinMemPerProc(size_t minMemPerProc) {
  min_mem_per_proc = minMemPerProc;
}

size_t MemoryAllocator::getMaxMemPerProc() const {
  return max_mem_per_proc;
}

const std::vector<MemoryAllocator::Frame>& MemoryAllocator::getMemoryFrames() const {
  return memoryFrames;
}

void MemoryAllocator::setMemoryFrames(const std::vector<MemoryAllocator::Frame>& frames) {
  memoryFrames = frames;
}

const std::string MemoryAllocator::getAllocationType() const {
  return allocationType;
}

void MemoryAllocator::setAllocationType(const std::string& type) {
  if (type == "flat" || type == "paging") {
    allocationType = type;
  }
}

size_t MemoryAllocator::getProcessMemory(int processID) const {
  size_t memoryUsed = 0;

  // Iterate through all memory frames
  for (const auto& frame : memoryFrames) {
    // Check if the frame is occupied by the given process
    if (frame.isOccupied && frame.processID == processID) {
      memoryUsed += mem_per_frame; // Add the frame size to the total memory used
    }
  }

  return memoryUsed;
}

size_t MemoryAllocator::calculateUsedMemory() const {
  size_t totalUsedMemory = 0;

  // Iterate through all memory frames
  for (const auto& frame : memoryFrames) {
    if (frame.isOccupied) {
      totalUsedMemory += mem_per_frame; // Add frame size to total used memory
    }
  }

  return totalUsedMemory;
}

void MemoryAllocator::moveToBackingStore(int processID) {
  std::vector<int> swappedPages;

  // Iterate through memory frames and move process pages to backing store
  for (size_t i = 0; i < memoryFrames.size(); ++i) {
    if (memoryFrames[i].processID == processID) {
      swappedPages.push_back(i); // Save frame index for the process
      memoryFrames[i] = { -1, false, 0 }; // Mark frame as free
    }
  }

  if (!swappedPages.empty()) {
    backingStore[processID] = swappedPages;

    // Write the swapped pages to a file
    std::ofstream file("backing_store_" + std::to_string(processID) + ".txt");
    if (file.is_open()) {
      file << "Process ID: " << processID << "\n";
      file << "Swapped Pages:\n";
      for (int pageIndex : swappedPages) {
        file << pageIndex << "\n";
      }
      file.close();
    }
    else {
      std::cerr << "Error: Unable to create backing store file for Process " << processID << ".\n";
    }
  }
}
void MemoryAllocator::restoreFromBackingStore(int processID) {
  // Check if the process is in the in-memory backing store
  if (backingStore.find(processID) == backingStore.end()) {

    // Attempt to restore from file
    std::ifstream file("backing_store_" + std::to_string(processID) + ".txt");
    if (file.is_open()) {
      std::vector<int> swappedPages;
      std::string line;
      while (std::getline(file, line)) {
        if (isdigit(line[0])) { // Parse only numeric lines (page indices)
          swappedPages.push_back(std::stoi(line));
        }
      }
      file.close();

      // Restore the process pages into memory
      for (int pageIndex : swappedPages) {
        if (pageIndex >= 0 && pageIndex < memoryFrames.size()) {
          memoryFrames[pageIndex] = { processID, true, std::time(0) };
        }
      }


      // Optionally delete the file after restoring
      std::remove(("backing_store_" + std::to_string(processID) + ".txt").c_str());
    }
    else {
      std::cerr << "Error: Backing store file not found for Process " << processID << ".\n";
    }
    return;
  }

  // Restore from in-memory backing store
  std::vector<int> swappedPages = backingStore[processID];
  backingStore.erase(processID);

  for (int pageIndex : swappedPages) {
    if (pageIndex >= 0 && pageIndex < memoryFrames.size()) {
      memoryFrames[pageIndex] = { processID, true, std::time(0) };
    }
  }

}