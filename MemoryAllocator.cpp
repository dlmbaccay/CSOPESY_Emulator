#include "MemoryAllocator.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

MemoryAllocator::MemoryAllocator(ConfigManager* configManager)
{
	memPerFrame = configManager->getMemPerFrame();
	memPerProcess = configManager->getMemPerProcess();
	maxOverallMem = configManager->getMaxOverallMem();
	totalSlots = maxOverallMem / memPerProcess;
  memorySlots.resize(totalSlots, false);
}

MemoryAllocator::~MemoryAllocator()
{
}

void MemoryAllocator::showMemoryUsage()
{

	// get the current time in format MM/DD/YYYY HH:MM:SSp
	time_t now = time(0);
	tm localtm;
	localtime_s(&localtm, &now);  // Use thread-safe localtime_s
	char timestamp[100];
	strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);

	std::cout << "Timestamp: " << timestamp << std::endl;
	std::cout << "Number of processes in memory: " << processMap.size() << std::endl;
	std::cout << "Total external fragmentation in KB: " << calculateExternalFrag() << std::endl << std::endl;
	std::cout << "----end---- = " << maxOverallMem << std::endl << std::endl;

  std::vector<std::pair<std::string, int>> processVec(processMap.begin(), processMap.end());

  std::sort(processVec.begin(), processVec.end(),
    [](const auto& a, const auto& b) { return a.second > b.second; });


	// print process memories from the processMap with their upper limit and lower limit based on which slot and memory per process
  for (const auto& pm : processVec) {
		std::cout << (pm.second + 1) * memPerProcess << std::endl;
		std::cout << pm.first << std::endl;
		std::cout << pm.second * memPerProcess << std::endl;
		std::cout << std::endl;
	}

	std::cout << std::endl << "----start---- = " << 0 << std::endl;
}

bool MemoryAllocator::addProcessMemory(Process* process)
{
  string processName = process->getProcessName();
  // Check if process is already in memory
  if (processMap.find(processName) != processMap.end()) {
    return false;
  }
  
  int freeSlots = 0;
  int consecutiveFreeSlots = 0;

  for (int i = 0; i < totalSlots; ++i) {
    if (!memorySlots[i]) {
      ++consecutiveFreeSlots;
      ++freeSlots;
    }
    else {
      consecutiveFreeSlots = 0;
    }

    if (consecutiveFreeSlots >= 1 &&
      (freeSlots * memPerProcess) >= memPerProcess + this->calculateExternalFrag()) {
      // Found enough contiguous space! Assign the process.
      for (int j = i - consecutiveFreeSlots + 1; j <= i; ++j) {
        memorySlots[j] = true;
      }
      processMap[processName] = i;
      return true;
    }
  }


  //memorySlots[i] = true;  // Mark slot as occupied
  //processMap[processName] = i;  // Track the slot assigned to this process
  //return true;
  return false;
}



int MemoryAllocator::calculateExternalFrag() const
{
	// Calculate external fragmentation by available free slots and memory per process
	/*int freeSlots = 0;
	for (int i = 0; i < totalSlots; ++i) {
		if (!memorySlots[i]) {
			++freeSlots;
		}
	}
	return freeSlots * memPerProcess;*/
  // Find the index of the last occupied slot using std::max_element
  // (more efficient than a manual loop for large arrays).

  int freeSlots = 0;
  int lastOccupiedIndex = -1;

  // Find the index of the last occupied slot
  for (int i = totalSlots - 1; i >= 0; --i) {
    if (memorySlots[i]) {
      lastOccupiedIndex = i;
      break;
    }
  }

  // If no process is running, there's no external fragmentation
  if (lastOccupiedIndex == -1) {
    return 0;
  }

  // Count free slots before the last occupied index
  for (int i = 0; i < lastOccupiedIndex; ++i) {
    if (!memorySlots[i]) {
      ++freeSlots;
    }
  }

  return freeSlots * memPerProcess;
}


void MemoryAllocator::removeProcessMemory(Process* process)
{
	string processName = process->getProcessName();
	auto it = processMap.find(processName);
	if (it != processMap.end()) {
		int slotIndex = it->second;
		memorySlots[slotIndex] = false;  // Mark slot as free
		processMap.erase(it);  // Remove process from map
	}
	else {
		std::cout << "Process " << processName << " not found in memory\n";
	}
}

// print out to text file in file name memory_stamp_<qq> where qq is current quantum cycle
void MemoryAllocator::logMemoryUsage() {
  std::lock_guard<std::mutex> logGuard(logMutex); // Ensure single access per log

  // Create the filename with the current quantum cycle
  std::string directory = "memory_stamps";
  std::filesystem::create_directories(directory);

  std::string filename = directory + "/memory_stamp_" + std::to_string(quantumCycles) + ".txt";

  // Open the file for writing
  std::ofstream outputFile(filename);
  if (!outputFile.is_open()) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  // Get the current time in format MM/DD/YYYY HH:MM:SSp
  time_t now = time(0);
  tm localtm;
  localtime_s(&localtm, &now);  // Use thread-safe localtime_s
  char timestamp[100];
  strftime(timestamp, sizeof(timestamp), "(%m/%d/%Y %I:%M:%S%p)", &localtm);

  // Write the output to the file
  outputFile << "Timestamp: " << timestamp << std::endl;
  outputFile << "Number of processes in memory: " << processMap.size() << std::endl;
  outputFile << "Total external fragmentation in KB: " << calculateExternalFrag() << std::endl << std::endl;
  outputFile << "----end---- = " << maxOverallMem << std::endl << std::endl;

  std::unordered_map<std::string, int> processMapCopy;
  {
    std::lock_guard<std::mutex> mapGuard(mapMutex); // Lock to safely copy the map
    processMapCopy = processMap; // Copy to avoid asynchronous issues
  }

  std::vector<std::pair<std::string, int>> processVec(processMapCopy.begin(), processMapCopy.end());

  std::sort(processVec.begin(), processVec.end(),
    [](const auto& a, const auto& b) { return a.second > b.second; });

  // Print process memories from the processMap 
  for (auto& pm : processVec) {
    outputFile << (pm.second + 1) * memPerProcess << std::endl;
    outputFile << pm.first << std::endl;
    outputFile << pm.second * memPerProcess << std::endl;
    outputFile << std::endl;
  }

  outputFile << std::endl << "----start---- = " << 0 << std::endl;

  // Close the file
  outputFile.close();
}