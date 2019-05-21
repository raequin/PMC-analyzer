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
};



struct Tutee {
  string id_number;
  string name;
  string email;
  string classification;
  string major;
  std::vector<Visit> visits;
};



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
  std::ifstream file(complete_path);
  CheckinsFileRow row;

  // Skip the unused lines of the file
  for (int i = 0; i < initial_line_num; i++)
    file >> row;

  // Place data into vector of cells, one row at a time
  while (file >> row) {
    string this_name (row[3].begin()+1, row[3].end()-1);
    
    // Ensure row is not for a tutor swipe created when testing the kiosk
    bool is_tutor = false;
    for (auto n : tutor_names) {
      if (0 == n.compare(this_name))  // "name" cell matches entry on the tutor list
	is_tutor = true;
    }

    if (!is_tutor) {
      // Create new Tutee for row if needed, otherwise add Visit to existing Tutee
      bool is_new_tutee = true;
      for (auto t : tutees) {
	if (0 == t.name.compare(this_name)) {  // Need to add Visit to Tutee
	  is_new_tutee = false;
	  // Add Visit to Tutee
	}
      }
      
      if (is_new_tutee) {  // Create new Tutee
	Tutee this_tutee;
	this_tutee.id_number = row[2];
	this_tutee.name = this_name;
	tutees.push_back(this_tutee);
      }
    }  // End treatment of non-tutor row
  }  // End loop through all rows
  
  for (auto t : tutees)
    cout << t.name << "\n";
}
/*

  string id_number;
  string name;
  string email;
  string classification;
  string major;
  std::vector<Visit> visits;
*/
