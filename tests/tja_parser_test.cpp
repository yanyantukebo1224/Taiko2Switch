#include "tja.hpp"
#include <cassert>
#include <string>

int main() {
    const std::string valid =
        "TITLE:Test Song\n"
        "ARTIST:Test Artist\n"
        "BPM:120\n"
        "WAVE:test.ogg\n"
        "COURSE:Oni\n"
        "LEVEL:7\n"
        "#START\n"
        "10002000,\n"
        "00000000,\n"
        "#END\n";
    taiko::Chart chart;
    assert(taiko::parseTja(valid, chart));
    assert(chart.title == "Test Song");
    assert(chart.artist == "Test Artist");
    assert(chart.wave == "test.ogg");
    assert(chart.level == 7);
    assert(chart.totalNotes == 2);
    assert(chart.notes.size() == 2);
    assert(chart.notes[0].type == '1');
    assert(chart.notes[1].type == '2');

    const std::string branch =
        "TITLE:Branch\nBPM:120\nWAVE:x.ogg\n#START\n#BRANCHSTART p,50,100\n1,\n#END\n";
    assert(!taiko::parseTja(branch, chart));
    assert(chart.error.find("Branch") != std::string::npos);
    return 0;
}
