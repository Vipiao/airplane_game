







#include "DebugTimer.h"


std::unordered_map<std::string, RunData> DebugTimer::m_runData{};

std::string DebugTimer::getResultsString() {
   std::string result = "";
   double totalTotalTime{ 0 };
   for (auto& it : m_runData) {
      totalTotalTime += (double)it.second.m_totalTime / (double)it.second.m_numAttempts;
   }
   for (auto& it : m_runData) {
      double averageTime{ (double)it.second.m_totalTime / (double)it.second.m_numAttempts };
      double fractionOfTotal = averageTime / totalTotalTime * 100.0;
      result += it.first + " " + std::to_string(averageTime) + " " + std::to_string(fractionOfTotal) + "%" + '\n';
   }
   return result;
}

DebugTimer::DebugTimer(std::string key) {
   m_key = key;
   m_start = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count();
   //m_startTime = std::chrono::high_resolution_clock::now ();
}

DebugTimer::~DebugTimer() {
   uint64_t timespan = std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
      ).count() - m_start;
   if (m_runData.find(m_key) == m_runData.end()) { // Category does not yet exist.
      m_runData[m_key] = { 1, timespan };
   } else {
      m_runData[m_key].m_numAttempts++;
      m_runData[m_key].m_totalTime += timespan;
   }

   //std::chrono::duration_cast<std::chrono::nanoseconds>
   //std::chrono::time_point<std::chrono::high_resolution_clock> endTime =
   //   std::chrono::high_resolution_clock::now ();
   //auto start = std::chrono::time_point<std::chrono::microseconds> (m_startTime).time_since_epoch ().count ();
}


/*
int64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now ().time_since_epoch ()
      ).count ();


int64_t timespan = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now ().time_since_epoch ()
      ).count () - start;


*/
