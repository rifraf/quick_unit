//
// The vcl.* code is used to help porting an old C++Builder application
// to other platforms. In this example, only a subset of the functions
// are shown. This is from an early snapshot of the code under 
// development and is not intended for production use.
//
#ifndef VCL_H
#define	VCL_H
#include <string>
#include <sstream>
#include <algorithm>


#ifdef unix
typedef unsigned DWORD;
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#else
#include <windows.h>
#endif

//----------------------------------------------------
class TCriticalSection {
//----------------------------------------------------

};

//----------------------------------------------------
class AnsiString {
//----------------------------------------------------
protected:
  std::string _string;

public:
  friend std::ostream& operator<<(std::ostream &, const AnsiString &);

  AnsiString() {
  };

  AnsiString(const char *init) {
    _string = std::string(init);
  };

  AnsiString(unsigned init) {
    std::ostringstream tmp;
		tmp << init;
    _string = tmp.str();
  };

  AnsiString(int init) {
    std::ostringstream tmp;
		tmp << init;
    _string = tmp.str();
  };

  const char *c_str(void) const {
    return _string.c_str();
  }

  std::string str() const {
    return std::string(_string);
  }
  
  AnsiString & operator=(const char *str) {
    _string = str;
    return *this;
  }

  AnsiString & operator=(std::string str) {
    _string = str;
    return *this;
  }

  AnsiString & operator+=(const char *str) {
    _string += str;
    return *this;
  }

  AnsiString & operator+=(const AnsiString &str) {
    _string += str._string;
    return *this;
  }

  AnsiString operator+(const char *str) {
    AnsiString result;
    result = (_string + str).c_str();
    return result;
  }

  AnsiString operator+(const AnsiString &str) {
    AnsiString result;
    result = (_string + str._string).c_str();
    return result;
  }

  bool operator==(const char *str) const {
    return _string == str;
  }

  bool operator!=(const char *str) const {
    return _string != str;
  }

  bool operator==(const AnsiString &str) const {
    return _string == str._string;
  }

  bool operator!=(const AnsiString &str) const {
    return _string != str._string;
  }

  AnsiString UpperCase(void) {
    AnsiString result = AnsiString(this->c_str());
		#ifdef _MSC_VER
    std::transform(result._string.begin(), result._string.end(), result._string.begin(), (int(*)(int)) toupper);
		#else
    std::transform(result._string.begin(), result._string.end(), result._string.begin(), (int(*)(int)) std::toupper);
		#endif
    return result;
  }

  int Pos(const char *str) {
    return 1 + (int)_string.find(str);
  }
  
  int Length(void) {
    return (int)_string.length();
  }

  bool IsEmpty(void) {
    return _string.empty();
  }

  AnsiString SubString(int start, int len) {
    std::string result = _string.substr(start - 1, len);
    return AnsiString(result.c_str());
  }

  int ToIntDef(int def) {
    std::stringstream ss(_string);
    int num;
    if((ss >> num).fail()) {
      return def;
    }
    return num;
  }
   
  int printf(const char* fmt, ...) {
    int count = 0;
    va_list args;
    va_start(args, fmt);
    std::string s;
    int length = 256;
    int result = format_arg_list(s, length, fmt, args);
    count += result;
    va_end(args);
    if (result >= 256) {
      va_start(args, fmt);
      format_arg_list(s, result + 1, fmt, args);
      va_end(args);
    }
    _string = s;
    return count;
  }

private:
  int format_arg_list(std::string& out, int length, const char *fmt, va_list args) {
    if (!fmt) return -1;
    int result = 0;
    char *buffer = NULL;
    buffer = new char [length + 1];
    memset(buffer, 0, length + 1);
		#ifdef _MSC_VER
    result = vsnprintf_s(buffer, length + 1, _TRUNCATE, fmt, args);
		#else
    result = vsnprintf(buffer, length, fmt, args);
		#endif
    out = buffer;
    delete [] buffer;
    return result;
  }

};

//----------------------------------------------------
class TStringList {
//----------------------------------------------------
protected:
  AnsiString _string;

public:
  void Text(const AnsiString ref);
  AnsiString Text(void);
  int Count(void);
  AnsiString Strings(int index);
  void Add(const AnsiString txt);
  void Clear(void);
};

//----------------------------------------------------
// Helper methods
//----------------------------------------------------
extern bool FileExists(const char * file);
extern AnsiString ExtractShortPathName(AnsiString file);
extern AnsiString ExtractFilePath(AnsiString file);
extern AnsiString ExtractFileName(AnsiString file);
extern AnsiString GetCurrentDir(void);
extern void SetCurrentDir(AnsiString dir);

#endif	/* VCL_H */

