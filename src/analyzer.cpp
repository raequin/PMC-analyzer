#include<ctime>  // For day of week from date etc.
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



int main(int argc,  char **argv)
{
  std::vector<Tutee> tutees;
  const uint8_t initial_line_num = 9;  // Where the data start on the txt file, starting at 0
  std::vector<std::string> tutor_names  = {"Wiggins, Robert","Lin, Min Chih", "Williams, Bradley", "Marshall, Matthew"};
  
  string complete_path(argv[1]);
  string analysis_mode(argv[2]);
  std::ifstream file(complete_path);
  CheckinsFileRow row;

  /*
   * Extract data from file
   */
  for (int i = 0; i < initial_line_num; i++)
    file >> row;  // Skip the unused lines of the file

  // Place data into vector of cells, one row at a time
  while (file >> row) {
    string this_name (row[3].begin()+1, row[3].end()-1);  // Remove quotation marks around name
    
    // Ensure row is not for a tutor swipe created when testing the kiosk
    bool is_tutor = false;
    for (auto n : tutor_names) {
      if (0 == n.compare(this_name))  // "name" cell matches entry on the tutor list
	is_tutor = true;
    }

    if (!is_tutor) {
      // Create new Tutee if needed, otherwise add Visit to existing Tutee
      bool is_new_tutee = true;
      for(std::vector<Tutee>::size_type i = 0; i < tutees.size(); ++i) {
	//      for(std::vector<Tutee>::iterator it = tutees.begin(); it != tutees.end(); ++it) {
	
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


  for (auto t : tutees) {
    cout << t.id_number << "\t" << t.name << "\t" << std::endl;
    
    for (auto v : t.visits)
      cout << v.course_number << "\t" << v.visit_tm.tm_hour << "h" << v.visit_tm.tm_min << "m\t" << v.visit_tm.tm_mon << "/"
		 << v.visit_tm.tm_mday << "/" << v.visit_tm.tm_year << "\t" << v.visit_tm.tm_wday << std::endl;

    cout << std::endl;
  }
  

  /*
   * Perform analysis of data
   */
  if (0 == analysis_mode.compare("totals")) {
    std::vector<Visit>::size_type num_visits = 0;
    for (auto t : tutees)
      num_visits += t.visits.size();
    
    cout << "The number of unique visitors is " << tutees.size() << "." << std::endl;
    cout << "The total number of visits was " << num_visits << "." << std::endl;
  }
}
