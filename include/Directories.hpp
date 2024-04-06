#ifndef Directory_H
#define Directory_H

#include "Common.hpp"

typedef struct Entry
{
    std::string name; // Name
    bool is_file;     // Selected dirent
} Entry;

class Editor;

class Directories
{
private:

public:
    std::vector<Entry> items; // Vector of items for store Directory
    std::string name;         // Name of dir
    int selected;             // Selected Directory
    Directories();
    ~Directories();
    void Update(std::string dir_name);
    void Load(std::string dir_name);

    void Logic(Editor &editor);
    void Draw(Editor &editor);
};

#endif // Directory_H