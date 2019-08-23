#include <algorithm>
#include <ctime>  // For day of week from date etc.
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

using std::cout;
using std::string;


struct Visit {
  std::tm visit_tm;
  string course_number;

  Visit() {
    course_number = "";
  }

  Visit(std::tm visit_tm_, string course_number_) {
    visit_tm = visit_tm_;
    course_number = course_number_;
  }

  void calc_date() {
    std::time_t time_temp = std::mktime(&visit_tm);
    visit_tm = *std::localtime(&time_temp);
  }
};



struct Tutee {
  string id_number;
  string name;
  string email;
  string classification;
  string major;
  std::vector<Visit> visits;

  Tutee() {
    id_number = "";
    name = "";
    email = "";
    classification = "";
    major = "";
  }

  Tutee(string id_number_, string name_, string email_, string classification_, string major_) {
    id_number = id_number_;
    name = name_;
    email = email_;
    classification = classification_;
    major = major_;
  }
};



std::vector<string> split(string str, char delimiter) {
  std::vector<string> ret;
  std::stringstream ss(str); // Turn the string into a stream.
  string tok;
 
  while(getline(ss, tok, delimiter)) {
    ret.push_back(tok);
  }
 
  return ret;
}



std::tm ConvertCellToTM(string cell) {
  std::tm ret;
  std::vector<string> date_time_AMPM = split(cell, ' ');
  std::vector<string> m_d_y = split(date_time_AMPM[0], '/');
  std::vector<string> hour_minute = split(date_time_AMPM[1], ':');

  ret.tm_sec = 0;
  ret.tm_min = std::stoi(hour_minute[1]);
  
  if (12 != std::stoi(hour_minute[0]))
    ret.tm_hour = (0 == date_time_AMPM[2].compare("AM")) ? (std::stoi(hour_minute[0])) : (std::stoi(hour_minute[0]) + 12);
  
  else
    ret.tm_hour = (0 == date_time_AMPM[2].compare("AM")) ? 0 : 12;
  
  ret.tm_mday = std::stoi(m_d_y[1]);  // 1-based year
  ret.tm_mon = std::stoi(m_d_y[0]) - 1;  // 0-based month
  ret.tm_year = std::stoi(m_d_y[2]) - 1900;  // Years since 1900

  return ret;
}



class CheckinsFileRow {
public:
  string const& operator[](std::size_t index) const {
    return m_data[index];
  }
  
  std::size_t size() const {
    return m_data.size();
  }
  
  void readNextRow(std::istream& str) {
    string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    string cell;

    m_data.clear();
    while(std::getline(lineStream, cell, '\t')) {
      m_data.push_back(cell);
    }
    
    // This checks for a trailing tab with no data after it.
    if (!lineStream && cell.empty()) {
      // If there was a trailing tab then add an empty element.
      m_data.push_back("");
    }
  }
  
private:
  std::vector<string> m_data;
};



std::istream& operator>>(std::istream& str, CheckinsFileRow& data)
{
  data.readNextRow(str);
  return str;
}



std::vector<std::string> GetTutorsList(int semester_index) {
  std::vector<std::vector<std::string>> tutor_names_lists;
  std::vector<std::string> tutor_names  = {"Marshall, Matthew", "Wangenheim, Evan", "Hyatt, Haylee",
					   "Freeman, Malik", "Wiggins, Robert", "Williams, Myles", "Williams, Bradley",
					   "Daoudi, Karim", "Rogers, Cassidy", "Lin, Min Chih", "Colley, Jonny"};
  tutor_names_lists.push_back(tutor_names);  // Spring 2019
  tutor_names  = {"Marshall, Matthew", "Freeman, Malik", "Wiggins, Robert", "Williams, Bradley", "Rogers, Cassidy",
		  "Lin, Min Chih", "Amoakon, Lythia", "Patel, Jay", "Hiller, Aaron", "Giles, John", "Dedani, Ashween", "Mistry, Nensi"};
  tutor_names_lists.push_back(tutor_names);  // Summer 2019

  return tutor_names_lists[semester_index];
}



std::vector<std::string> GetCoursesList(int semester_index) {
  std::vector<std::vector<std::string>> course_names_lists;
  std::vector<std::string> course_names  = {"CE-2003", "EE-2301", "ECET-1101", "ECET-1101L", "EE-2305", "EE-3401", "ECET-2300", "ECET-2300L", "ECET-2310", "ECET-2310L",
					    "ENGR-2214", "MET-2124", "ENGR-3122", "MET-3126", "ENGR-3131", "ENGR-3132", "MET-3124", "MET-3124L",
					    "ENGR-3343", "ENGR-3345", "MET-3101", "ISYE-2600", "IET-2227", "ISYE-3400", "IET-4405", "ME-4141", "MET-4141", "MTRE-2610"};
  course_names_lists.push_back(course_names);  // Spring 2019
  course_names_lists.push_back(course_names);  // Summer 2019
  course_names  = {"ENGR-2214", "MET-2124", "ENGR-3131", "ENGR-3132", "MET-3124", "MET-3124L", "EDG-1210", "EDG-1211", "EDG-3112", "EDG-2160",
		   "EE-2301", "ECET-1101", "ECET-1101L", "EE-2305", "EE-2501", "ECET-1200", "ECET-1200L", "ECET-2210", "ECET-2210L", "EE-2401",
		   "EE-3401", "ECET-2300", "ECET-2300L", "ECET-2310", "ECET-2310L",
		   "ENGR-3122", "MET-3126", "MTRE-2610", "ME-3101", "MET-3132", "MET-3132L", "ME-3701", "MET-1311", "ISYE-3350", "IET-2249"};
  course_names_lists.push_back(course_names);  // Fall 2019

  return course_names_lists[semester_index];

}



int main(int argc,  char **argv)
{
  std::vector<Tutee> tutees;
  const uint8_t initial_line_num = 9;  // Where the data start on the txt file, starting at 0
  string complete_path(argv[1]);
  string analysis_mode(argv[2]);
  std::vector<std::string> tutor_names = GetTutorsList(std::stoi(argv[3]));  // Which list of tutors to filter out of checkins
  std::vector<std::string> course_names = GetCoursesList(std::stoi(argv[3]));  // Which list of courses to use for filtering checkins
  std::ifstream file(complete_path);
  CheckinsFileRow row;

  /*
   * Extract data from file
   */
  for (int i = 0; i < initial_line_num; i++)
    file >> row;  // Skip the unused lines of the file

  // Place data into vector of cells, one row at a time
  while (file >> row) {
    // Debug
    /*
    cout << "size of row = " << row.size() << std::endl;
    for (std::size_t j = 0; j < row.size(); j++)
      cout << row[j] << "\n";
    cout << std::endl;
    */
    
    string this_name (row[3].begin()+1, row[3].end()-1);  // Remove quotation marks around name
    string this_course (row[7]);
    
    // Ensure row is not for a tutor swipe created when testing the kiosk
    bool is_tutor = false;
    for (auto n : tutor_names) {
      if (0 == n.compare(this_name)) {  // "name" cell matches entry on the tutor list
	is_tutor = true;
	break;
      }
    }

    // Ensure row is for a valid course
    bool is_valid_course = false;
    for (std::vector<std::string>::size_type i = 0; i < course_names.size(); ++i) {
      if (0 == course_names[i].compare(this_course)) {  // "name" cell matches entry on the course list
	is_valid_course = true;
	break;
      }
    }

    if (!is_tutor && is_valid_course) {
      // Create new Tutee if needed, otherwise add Visit to existing Tutee
      bool is_new_tutee = true;
      for (std::vector<Tutee>::size_type i = 0; i < tutees.size(); ++i) {	
	if (0 == tutees[i].name.compare(this_name)) {  // Tutee already exists
	  is_new_tutee = false;
	  tutees[i].visits.push_back(Visit(ConvertCellToTM(row[0]), row[7]));  // Add Visit
	  tutees[i].visits.back().calc_date();
	  
	  // Debug
	  /*
	    cout << "Existing tutee:\t" << this_name << "\tThis visit:\t" << row[0] << "\t" << row[7] << "\t" << tutees[i].visits.size() << " visits" << std::endl;
	    cout << tutees[i].id_number << "\t" << tutees[i].name << "\t" << tutees[i].email << "\t" << tutees[i].classification << "\t" << tutees[i].major << std::endl;
	    for (auto v : tutees[i].visits)
	    cout << v.course_number << "\t" << v.visit_tm.tm_hour << "h" << v.visit_tm.tm_min << "m\t" << v.visit_tm.tm_mon << "/"
	    << v.visit_tm.tm_mday << "/" << v.visit_tm.tm_year << "\t" << v.visit_tm.tm_wday << std::endl;
	    cout << std::endl;
	  */
	  break;  // Quit searching for tutee name
	}
      }  // End loop through tutees
      
      if (is_new_tutee) {
	//	cout << "New tutee:\t" << this_name << std::endl;  // Debug
	tutees.push_back(Tutee(row[2], this_name, row[12], row[13], row[14]));  // Construct Tutee and add to tutees
	tutees.back().visits.push_back(Visit(ConvertCellToTM(row[0]), row[7]));  // Add Visit to visits
	tutees.back().visits.back().calc_date();
      }
    }  // End treatment of non-tutor row
  }  // End loop through all rows
  

  /*
   * Perform analysis of data
   */
  if (0 == analysis_mode.compare("totals")) {
    std::vector<Visit>::size_type num_visits = 0;
    for (auto t : tutees)
      num_visits += t.visits.size();
    
    cout << "The number of unique visitors is " << tutees.size() << "." << std::endl;
    cout << "The total number of visits was " << num_visits << "." << std::endl;
    cout << "Note: this is disregarding checkins for courses not supported." << std::endl;
  }

  else if (0 == analysis_mode.compare("count_by_course")) {
    // Get vector of course numbers
    std::vector<std::string> course_numbers;
    for (auto t : tutees) {
      for (auto v : t.visits) {
	if (std::find(course_numbers.begin(), course_numbers.end(), v.course_number) == course_numbers.end())
	  course_numbers.push_back(v.course_number);
      }  // End loop through visits
    }  // End loop through tutees
    std::sort(course_numbers.begin(), course_numbers.end());  // Sort the vector
    
    for (auto c : course_numbers) {
      int course_count = 0;
      int tutee_count = 0;
      for (auto t : tutees) {
	bool this_tutee_this_course = false;
	for (auto v : t.visits) {
	  if (0 == v.course_number.compare(c)) {
	    ++course_count;
	    if (!this_tutee_this_course) {
	      this_tutee_this_course = true;
	      ++tutee_count;
	    }
	  }
	}  // End loop through visits
      }  // End loop through tutees
      
      cout << c << ":\t" << course_count << "\tvisits\t" << tutee_count << "\tvisitors\t" << (float)course_count / (float)tutee_count << "\taverage\n";
    }

  }

  
  else if (0 == analysis_mode.compare("print_tutees")) {
    for (auto t : tutees)
      //cout << t.id_number << std::endl;
      cout << t.id_number << "\t" << t.name << "\t" << std::endl;
  }

  
  else if (0 == analysis_mode.compare("print_tutees_and_visits")) {
    for (auto t : tutees) {      
      for (auto v : t.visits)
	cout << t.id_number << "\t" << t.name << "\t" << v.course_number << "\t"
	     << v.visit_tm.tm_hour << "h" << v.visit_tm.tm_min << "m\t" << v.visit_tm.tm_mon << "/"
	     << v.visit_tm.tm_mday << "/" << v.visit_tm.tm_year + 1900 << "\t" << v.visit_tm.tm_wday << "\t" << v.visit_tm.tm_yday << std::endl;
    }
  }


  else if (0 == analysis_mode.compare("emails")) {
    for (auto t : tutees)
      cout << t.email << std::endl;

  }



  else if (0 == analysis_mode.compare("pre_and_post_withdrawal")) {
    std::vector<Tutee> pre_withdrawal_tutees;
    std::vector<Tutee> post_withdrawal_tutees;
    const int withdrawal_tm_yday = 57;

    for (auto t : tutees) {
      if (withdrawal_tm_yday > t.visits[0].visit_tm.tm_yday)  // Tutee first visited Center prior to withdrawal deadline
	pre_withdrawal_tutees.push_back(t);

      else
	post_withdrawal_tutees.push_back(t);
    }

    for (auto t : pre_withdrawal_tutees) {
      cout << t.id_number << "\t" << t.name << "\t" << "PRE\t" << t.visits[0].visit_tm.tm_yday << std::endl;
      /*
      for (auto v : t.visits)
	cout << t.id_number << "\t" << t.name << "\t" << v.course_number << "\t"
	     << v.visit_tm.tm_hour << "h" << v.visit_tm.tm_min << "m\t" << v.visit_tm.tm_mon << "/"
	     << v.visit_tm.tm_mday << "/" << v.visit_tm.tm_year + 1900 << "\t" << v.visit_tm.tm_wday << "\t" << v.visit_tm.tm_yday << std::endl;
      */
    }

    cout << "\n\n" << std::endl;
    for (auto t : post_withdrawal_tutees) {
      cout << t.id_number << "\t" << t.name << "\t" << "POST\t" << t.visits[0].visit_tm.tm_yday << std::endl;
      /*
      for (auto v : t.visits)
	cout << t.id_number << "\t" << t.name << "\t" << v.course_number << "\t"
	     << v.visit_tm.tm_hour << "h" << v.visit_tm.tm_min << "m\t" << v.visit_tm.tm_mon << "/"
	     << v.visit_tm.tm_mday << "/" << v.visit_tm.tm_year + 1900 << "\t" << v.visit_tm.tm_wday << "\t" << v.visit_tm.tm_yday << std::endl;
      */
    }
  }


  else if (0 == analysis_mode.compare("usage_by_week")) {
    int visit_count[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  }
}
