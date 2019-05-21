#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>

using std::string;


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
  const uint8_t initial_line_num = 9;  // Where the data start on the txt file, starting at 0
  std::vector<std::string> tutor_names  = {"Wiggins, Robert","Lin, Min Chih", "Williams, Bradley"};
  
  string complete_path(argv[1]);
  std::ifstream file(complete_path);
  CheckinsFileRow row;

  // Skip the unused lines of the file
  for (int i = 0; i < initial_line_num; i++)
    file >> row;

  // Place data into vector of cells, one row at a time
  while (file >> row) {
    for(auto t = tutor_names.begin(); t != tutor_names.end(); ++t) {
      std::cout << *t << '\n';
      //      string this_name = row[3];
      //      if (row[3]
    }
    string this_name = row[3];
    this_name.erase(this_name.begin());
    this_name.erase(this_name.end());
    std::cout << row[3] << "\t" << this_name << "\n";
  }
}
