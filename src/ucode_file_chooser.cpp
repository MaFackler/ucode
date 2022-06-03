#include "ucode_file_chooser.h"
#include "ucode_utils.h"

void FileChooser::open_folder(const char *foldername) {
    this->filter_string.clear();
    this->filtered.clear();
    this->files.clear();
    size_t i = 0;
    for (auto &f: std::filesystem::recursive_directory_iterator(foldername)) {
        //auto &ext = f.path().extension();
        if (this->extensions.size() == 0 ||
                this->extensions.find(f.path().extension()) != this->extensions.end()) { 

            bool skip = false;
            for (auto &ignore: this->ignore) {
                if (f.path().string().find(ignore) != string::npos) {
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                if (f.path().string().find(this->filter_string) != string::npos) {
                    this->files.emplace_back(f);
                    this->filtered.push_back(i++);
                }
            }

        }
    }
}

void FileChooser::filter() {
    this->filtered.clear();

    for (size_t i = 0; i < this->files.size(); ++i) {
        if (this->filter_string.size() == 0 ||
                this->files[i].string().find(this->filter_string) != string::npos) {
            this->filtered.push_back(i);
        }
    }
}

