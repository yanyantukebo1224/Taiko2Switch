#include <switch.h>
#include <dirent.h>
#include <cstdio>
#include <string>
#include <vector>
#include "tja.hpp"

namespace {
struct SongEntry {
    std::string folder;
    std::string file;
    taiko::Chart chart;
    bool valid = false;
};

std::vector<SongEntry> songs;
int selected = 0;
std::string status = "Create /switch/Taiko2Switch/songs/ on the SD card.";

bool readFile(const std::string& path, std::string& out) {
    FILE* fp = std::fopen(path.c_str(), "rb");
    if (!fp) return false;
    if (std::fseek(fp, 0, SEEK_END) != 0) { std::fclose(fp); return false; }
    const long size = std::ftell(fp);
    if (size <= 0 || size > 4 * 1024 * 1024) { std::fclose(fp); return false; }
    std::rewind(fp);
    out.resize(static_cast<std::size_t>(size));
    const std::size_t got = std::fread(&out[0], 1, out.size(), fp);
    std::fclose(fp);
    out.resize(got);
    return got > 0;
}

void scanSongs() {
    songs.clear();
    DIR* root = opendir("sdmc:/switch/Taiko2Switch/songs");
    if (!root) {
        status = "Song folder missing. Create it on the SD card.";
        return;
    }
    dirent* item = nullptr;
    while ((item = readdir(root)) != nullptr) {
        if (item->d_name[0] == '.') continue;
        const std::string folder = item->d_name;
        const std::string base = "sdmc:/switch/Taiko2Switch/songs/" + folder + "/";
        DIR* dir = opendir(base.c_str());
        if (!dir) continue;
        dirent* file = nullptr;
        while ((file = readdir(dir)) != nullptr) {
            const std::string name = file->d_name;
            if (name.size() < 4 || name.substr(name.size() - 4) != ".tja") continue;
            SongEntry entry;
            entry.folder = folder;
            entry.file = name;
            std::string data;
            if (readFile(base + name, data)) entry.valid = taiko::parseTja(data, entry.chart);
            songs.push_back(entry);
            break;
        }
        closedir(dir);
    }
    closedir(root);
    status = songs.empty() ? "No TJA files found." :
        "Loaded " + std::to_string(songs.size()) + " song folder(s).";
    if (selected >= static_cast<int>(songs.size())) selected = 0;
}

void draw() {
    consoleClear();
    std::printf("\x1b[1;1HTaiko2Switch  |  TJA Player");
    std::printf("\x1b[2;1H==============================================");
    std::printf("\x1b[3;1H%s", status.c_str());
    std::printf("\x1b[5;1HSONG SELECT  (UP/DOWN: select, X: rescan)");
    for (int i = 0; i < 12 && i < static_cast<int>(songs.size()); ++i) {
        const SongEntry& song = songs[static_cast<std::size_t>(i)];
        std::printf("\x1b[%d;1H%c %s%s", 7 + i, i == selected ? '>' : ' ',
                    song.valid ? song.chart.title.c_str() : song.folder.c_str(),
                    song.valid ? "" : "  [TJA error]");
    }
    if (!songs.empty()) {
        const SongEntry& song = songs[static_cast<std::size_t>(selected)];
        std::printf("\x1b[21;1H----------------------------------------------");
        if (song.valid) {
            std::printf("\x1b[22;1HArtist: %s", song.chart.artist.c_str());
            std::printf("\x1b[23;1HBPM: %.2f  Course: %s  Level: %d  Notes: %d",
                        song.chart.bpm, song.chart.course.c_str(), song.chart.level,
                        song.chart.totalNotes);
        } else {
            std::printf("\x1b[22;1H%s", song.chart.error.c_str());
        }
    }
    std::printf("\x1b[25;1HPLUS: Exit");
}
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    consoleInit(nullptr);
    const Result mountResult = fsdevMountSdmc();
    if (R_FAILED(mountResult)) status = "Could not mount SD card.";
    else scanSongs();

    while (appletMainLoop()) {
        hidScanInput();
        const u64 down = hidKeysDown(CONTROLLER_P1_AUTO);
        if (down & KEY_PLUS) break;
        if ((down & KEY_DOWN) && !songs.empty())
            selected = (selected + 1) % static_cast<int>(songs.size());
        if ((down & KEY_UP) && !songs.empty())
            selected = (selected + static_cast<int>(songs.size()) - 1) % static_cast<int>(songs.size());
        if (down & KEY_X) scanSongs();
        draw();
        consoleUpdate(nullptr);
    }
    fsdevUnmountDevice("sdmc");
    consoleExit();
    return 0;
}
