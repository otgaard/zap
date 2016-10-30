//
// Created by Darren Otgaar on 2016/10/28.
//

#include "os.hpp"
#ifndef WIN32
#include <dirent.h>
#else
#include <windows.h>
#endif
#include <sys/stat.h>
#include <GLFW/glfw3.h>

using namespace zap;

bool is_file(const std::string& filename) {
#ifndef WIN32
    struct stat buf;
    return stat(filename.c_str(), &buf) == 0 && S_ISREG(buf.st_mode);
#else
	return false;
#endif
}

bool is_dir(const std::string& path) {
#ifndef WIN32
    struct stat buf;
    return stat(path.c_str(), &buf) == 0 && S_ISDIR(buf.st_mode);
#else
	return false;
#endif
}

std::vector<std::string> get_files(const std::string& path) {
    std::vector<std::string> out;

#ifdef WIN32
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if((dir = FindFirstFile((path + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE) return out;

    do {
        const std::string file_name = path + "/" + file_data.cFileName;
        if(file_data.cFileName[0] == '.' || (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) continue;
        out.push_back(file_name);
    } while(FindNextFile(dir, &file_data));

    FindClose(dir);
#else
    DIR* dir;
    struct dirent* ent;
    struct stat st;

    dir = opendir(path.c_str());
    while((ent = readdir(dir)) != nullptr) {
        const std::string file_path = path + "/" + ent->d_name;
        if(ent->d_name[0] == '.' || stat(file_path.c_str(), &st) == -1 || (st.st_mode & S_IFDIR) != 0) continue;
        out.push_back(file_path);
    }
    closedir(dir);
#endif

    return out;
}