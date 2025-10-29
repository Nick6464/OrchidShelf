#pragma once

#include "esphome.h"
#include <sstream>
#include <vector>
#include <algorithm>

// Speed conversion function
float speed_to_level(const std::string& speed) {
  if (speed == "55%") return 0.55;
  if (speed == "65%") return 0.65;
  if (speed == "75%") return 0.75;
  if (speed == "85%") return 0.85;
  if (speed == "100%") return 1.0;
  return 0.65; // default
}

// Calculate actual water depth from sensor distance
// Sensor measures distance to water surface, so:
// water_depth = empty_distance - current_distance
float calculate_water_depth(int bin_num, float sensor_distance) {
  float empty_distance = id(bin_1_empty_distance).state;
  
  // Water depth = distance when empty - current distance to water
  float depth = empty_distance - sensor_distance;
  
  // Clamp to reasonable values
  if (depth < 0) depth = 0;
  if (depth > empty_distance) depth = empty_distance;
  
  return depth;
}

// Get target depth for a bin
float get_target_depth(int bin_num) {
  return id(bin_1_target_depth).state;
}

// Helper function to get pump state by number
std::string get_pump_state(int pump_num) {
  return id(pump_1_state);
}

// Helper function to get bin enable state by number
bool get_bin_enable(int bin_num) {
  return id(bin_1_enable).state;
}

// Helper function to get next cycle timestamp by number
int get_next_cycle_time(int bin_num) {
  return id(bin_1_next_cycle);
}

// Forward declaration
float get_cycle_interval(int pump_num);

// Helper function to set next cycle timestamp by number
void set_next_cycle_time(int bin_num, int current_time) {
  float interval_days = get_cycle_interval(bin_num);
  int interval_seconds = (int)(interval_days * 86400);
  int next_time = current_time + interval_seconds;
  id(bin_1_next_cycle) = next_time;
}

// Helper function to get last cycle by number
int get_last_cycle(int pump_num) {
  return id(pump_1_last_cycle);
}

// Helper function to get cycle interval by number
float get_cycle_interval(int pump_num) {
  return id(pump_1_cycle_interval).state;
}

// Helper function to get schedule mode by number (0 = interval days, 1 = daily times)
int get_schedule_mode(int bin_num) {
  return id(bin_1_schedule_mode);
}

// Helper function to get daily times by number
std::string get_daily_times(int bin_num) {
  return id(ha_bin_1_daily_times).state;
}

// Helper function to get interval time by number
int get_interval_time(int bin_num) {
  return id(bin_1_interval_time);
}

// Helper function to set last cycle by number
void set_last_cycle(int pump_num, int value) {
  id(pump_1_last_cycle) = value;
}

// Helper function to execute flood cycle by number
void execute_flood_cycle(int pump_num) {
  id(pump_1_flood_cycle).execute();
}

// Simplified countdown calculation for display only
float calculate_countdown_hours(int pump_num) {
  bool bin_enable = get_bin_enable(pump_num);
  
  if (!bin_enable) {
    return NAN;
  }
  
  auto now = id(homeassistant_time).now();
  auto current_time = now.timestamp;
  int next_cycle_time = get_next_cycle_time(pump_num);
  
  if (next_cycle_time == 0) {
    int current_hour = now.hour;
    int current_minute = now.minute;
    int current_day = now.day_of_year;
    int schedule_mode = get_schedule_mode(pump_num);
    
    if (schedule_mode == 0) {
      int last_run_day = id(bin_1_last_run_day);
      
      int days_since = 0;
      if (last_run_day > 0) {
        days_since = (current_day - last_run_day + 365) % 365;
      }
      
      float interval_days = get_cycle_interval(pump_num);
      int interval_time = get_interval_time(pump_num);
      
      int hours_until = interval_time - current_hour;
      if (hours_until <= 0) hours_until += 24;
      
      if (days_since >= (int)interval_days) {
        return hours_until;
      } else {
        int days_until = (int)interval_days - days_since;
        return (days_until * 24) + hours_until;
      }
    } else {
      // Daily Times mode
      std::string daily_times = get_daily_times(pump_num);
      
      std::stringstream ss(daily_times);
      std::string time_str;
      std::vector<int> times;
      while (std::getline(ss, time_str, ',')) {
        time_str.erase(0, time_str.find_first_not_of(" \t"));
        time_str.erase(time_str.find_last_not_of(" \t") + 1);
        if (!time_str.empty()) {
          times.push_back(std::stoi(time_str));
        }
      }
      
      std::sort(times.begin(), times.end());
      
      for (int time : times) {
        if (time > current_hour) {
          int total_minutes = (time * 60) - (current_hour * 60 + current_minute);
          return total_minutes / 60.0;
        }
      }
      
      if (!times.empty()) {
        int total_minutes = (24 * 60) - (current_hour * 60 + current_minute) + (times[0] * 60);
        return total_minutes / 60.0;
      } else {
        return 24.0;
      }
    }
  }
  
  if (next_cycle_time > current_time) {
    auto time_diff = next_cycle_time - current_time;
    return time_diff / 3600.0;
  }
  
  return 0.0;
}

// Simplified countdown text calculation for display only
std::string calculate_countdown_text(int pump_num) {
  bool bin_enable = get_bin_enable(pump_num);
  
  if (!bin_enable) {
    return "Disabled";
  }
  
  auto now = id(homeassistant_time).now();
  auto current_time = now.timestamp;
  int next_cycle_time = get_next_cycle_time(pump_num);
  
  if (next_cycle_time == 0) {
    int current_hour = now.hour;
    int current_minute = now.minute;
    int current_day = now.day_of_year;
    int schedule_mode = get_schedule_mode(pump_num);
    
    if (schedule_mode == 0) {
      int last_run_day = id(bin_1_last_run_day);
      
      int days_since = 0;
      if (last_run_day > 0) {
        days_since = (current_day - last_run_day + 365) % 365;
      }
      
      float interval_days = get_cycle_interval(pump_num);
      int interval_time = get_interval_time(pump_num);
      
      int hours_until = interval_time - current_hour;
      if (hours_until <= 0) hours_until += 24;
      
      if (days_since >= (int)interval_days) {
        return std::to_string(hours_until) + "h";
      } else {
        int days_until = (int)interval_days - days_since;
        if (days_until > 0) {
          return std::to_string(days_until) + "d " + std::to_string(hours_until) + "h";
        } else {
          return std::to_string(hours_until) + "h";
        }
      }
    } else {
      // Daily Times mode
      std::string daily_times = get_daily_times(pump_num);
      
      std::stringstream ss(daily_times);
      std::string time_str;
      std::vector<int> times;
      while (std::getline(ss, time_str, ',')) {
        time_str.erase(0, time_str.find_first_not_of(" \t"));
        time_str.erase(time_str.find_last_not_of(" \t") + 1);
        if (!time_str.empty()) {
          times.push_back(std::stoi(time_str));
        }
      }
      
      std::sort(times.begin(), times.end());
      
      for (int time : times) {
        if (time > current_hour) {
          int total_minutes = (time * 60) - (current_hour * 60 + current_minute);
          
          if (total_minutes >= 60) {
            int hours = total_minutes / 60;
            int minutes = total_minutes % 60;
            if (minutes > 0) {
              return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
            } else {
              return std::to_string(hours) + "h";
            }
          } else {
            return std::to_string(total_minutes) + "m";
          }
        }
      }
      
      if (!times.empty()) {
        int total_minutes = (24 * 60) - (current_hour * 60 + current_minute) + (times[0] * 60);
        
        if (total_minutes >= 60) {
          int hours = total_minutes / 60;
          int minutes = total_minutes % 60;
          if (minutes > 0) {
            return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
          } else {
            return std::to_string(hours) + "h";
          }
        } else {
          return std::to_string(total_minutes) + "m";
        }
      } else {
        return "No times configured";
      }
    }
  }
  
  if (next_cycle_time <= current_time) {
    return "Due now";
  }
  
  auto time_diff = next_cycle_time - current_time;
  int days = time_diff / 86400;
  int hours = (time_diff % 86400) / 3600;
  int minutes = ((time_diff % 86400) % 3600) / 60;
  
  if (days > 0) {
    return std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(minutes) + "m";
  } else if (hours > 0) {
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
  } else {
    return std::to_string(minutes) + "m";
  }
}

