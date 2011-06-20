//
// The vcl.* code is used to help porting an old C++Builder application
// to other platforms. In this example, only a subset of the functions
// are shown. This is from an early snapshot of the code under 
// development and is not intended for production use.
//
#include "vcl.h"
#include <vector>

static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

//----------------------------------------------------
std::ostream& operator<<(std::ostream &stream, const AnsiString &str) {
  stream << str._string;
  return stream;
}

//----------------------------------------------------
AnsiString TStringList::Text(void) {
  // Sorting not supported at the moment...
  return _string.c_str();
}

//----------------------------------------------------
void TStringList::Text(const AnsiString txt) {
  _string = txt;
}

//----------------------------------------------------
void TStringList::Clear(void) {
  _string = "";
}

//----------------------------------------------------
void TStringList::Add(const AnsiString txt) {
  if (!_string.IsEmpty()) {
    _string += "\n";
  }
  _string += txt;
}

int TStringList::Count(void) {
  std::vector<std::string> x = split(_string.str(),'\n');
  return (int)x.size();
}

AnsiString TStringList::Strings(int index) {
  std::vector<std::string> x = split(_string.str(),'\n');
  if (index < 0 || index >= (int)x.size()) {
    return "";
  }
  return AnsiString(x.at(index).c_str());
}



//----------------------------------------------------
bool FileExists(const char * file) {
  return false;
}
AnsiString ExtractShortPathName(AnsiString file) {
  return "";
}
AnsiString ExtractFilePath(AnsiString file) {
  return "";
}
AnsiString ExtractFileName(AnsiString file) {
  return "";
}
AnsiString GetCurrentDir(void) {
  return "";
}
void SetCurrentDir(AnsiString dir) {
}
