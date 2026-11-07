#pragma once

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace taiko {

struct Note {
    int tick = 0;       // 1/16-beat units from chart start
    char type = '0';    // 1 Don, 2 Ka, 3 big Don, 4 big Ka, 5 drumroll, 6 big roll, 7 balloon
};

struct Chart {
    std::string title;
    std::string subtitle;
    std::string artist;
    std::string genre;
    std::string wave;
    std::string course;
    float bpm = 120.0f;
    float offset = 0.0f;
    int level = 0;
    int totalNotes = 0;
    std::vector<Note> notes;
    std::string error;
};

inline std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

inline std::string upper(std::string value) {
    for (char& c : value) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return value;
}

// Parses the common TJA header and note-row subset. Branches and scroll gimmicks
// are deliberately preserved as unsupported rather than silently misinterpreted.
inline bool parseTja(const std::string& source, Chart& chart) {
    chart = Chart{};
    bool inChart = false;
    int tick = 0;
    int measure = 0;
    std::string line;
    std::size_t pos = 0;

    while (pos <= source.size()) {
        const std::size_t end = source.find('\n', pos);
        line = trim(source.substr(pos, end == std::string::npos ? std::string::npos : end - pos));
        pos = end == std::string::npos ? source.size() + 1 : end + 1;
        if (line.empty() || line[0] == '/') continue;

        if (!inChart) {
            const std::size_t colon = line.find(':');
            if (colon != std::string::npos) {
                const std::string key = upper(trim(line.substr(0, colon)));
                const std::string value = trim(line.substr(colon + 1));
                if (key == "TITLE") chart.title = value;
                else if (key == "SUBTITLE") chart.subtitle = value;
                else if (key == "ARTIST") chart.artist = value;
                else if (key == "GENRE") chart.genre = value;
                else if (key == "WAVE") chart.wave = value;
                else if (key == "BPM") chart.bpm = static_cast<float>(std::atof(value.c_str()));
                else if (key == "OFFSET") chart.offset = static_cast<float>(std::atof(value.c_str()));
                else if (key == "COURSE") chart.course = value;
                else if (key == "LEVEL") chart.level = std::atoi(value.c_str());
            }
        }

        const std::string command = upper(line);
        if (command == "#START" || command.find("#START ") == 0) {
            inChart = true;
            tick = 0;
            measure = 0;
            continue;
        }
        if (!inChart) continue;
        if (command == "#END") break;
        if (command[0] == '#') {
            // #MEASURE is supported as a timing boundary; other commands are
            // not interpreted by this first parser version.
            if (command == "#MEASURE") ++measure;
            else if (command.find("#BPMCHANGE") == 0 ||
                     command.find("#SCROLL") == 0 ||
                     command.find("#DELAY") == 0) {
                chart.error = "This chart uses timing/scroll commands not yet supported.";
                return false;
            } else if (command.find("#BRANCH") == 0 ||
                       command.find("#N") == 0 || command.find("#E") == 0 ||
                       command.find("#M") == 0) {
                chart.error = "Branch charts are not supported yet.";
                return false;
            }
            continue;
        }

        std::string digits;
        for (char c : line) {
            if (c == ',') {
                const int count = static_cast<int>(digits.size());
                if (count > 0) {
                    for (int i = 0; i < count; ++i) {
                        const char n = digits[static_cast<std::size_t>(i)];
                        if (n >= '1' && n <= '7') {
                            chart.notes.push_back(Note{tick + (i * 16) / count, n});
                            ++chart.totalNotes;
                        }
                    }
                    tick += 16;
                    digits.clear();
                }
            } else if (c >= '0' && c <= '7') {
                digits.push_back(c);
            }
        }
        if (!digits.empty()) {
            const int count = static_cast<int>(digits.size());
            for (int i = 0; i < count; ++i) {
                const char n = digits[static_cast<std::size_t>(i)];
                if (n >= '1' && n <= '7') {
                    chart.notes.push_back(Note{tick + (i * 16) / count, n});
                    ++chart.totalNotes;
                }
            }
            tick += 16;
        }
    }

    if (chart.title.empty()) chart.title = "Untitled";
    if (chart.wave.empty()) {
        chart.error = "TJA header is missing WAVE.";
        return false;
    }
    if (chart.bpm <= 0.0f) {
        chart.error = "BPM must be greater than zero.";
        return false;
    }
    if (chart.notes.empty()) {
        chart.error = "No playable notes were found between #START and #END.";
        return false;
    }
    return true;
}

} // namespace taiko
