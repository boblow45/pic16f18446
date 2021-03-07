import pandas as pd

notes_map = {
    "C": 16.35,
    "C#": 17.32,
    "Db": 17.32,
    "D": 18.53,
    "Eb": 19.45,
    "E":20.60,
    "F": 21.83,
    "F#": 23.12,
    "Gb": 24.50,
    "G": 24.50,
    "G#": 25.96,
    "Ab":27.50,
    "A":27.50,
    "Bb": 29.14,
    "B":30.87,
    "Rest":0
}


def convert_beat_to_duration(beats, tempo):
    min_beat_frac = min(beats)
    min_time = int((60 / tempo) * 1000 * min_beat_frac)
    print(min_time)

    return [int(beat/min_beat_frac * min_time) for beat in beats]

def convert_note_to_freqs(notes, octave):
    freqs = []
    for note in notes:

        offset = note.count("'")
        offset -= note.count(",")
        note = note.replace("'", "").replace(",", "")
        
        if note.strip() in notes_map.keys():
            freqs.append(int(notes_map[note] * 2**(octave+offset)))

    return freqs

notes = pd.read_csv("Imperial_March.tsv", sep="\t")
notes["Notes"] = notes["Notes"].fillna("Rest")

freqs = convert_note_to_freqs(list(notes["Notes"]), 4)
print(list(map(hex, freqs)))

durations = convert_beat_to_duration(list(notes["Duration"]), 60)
print(list(map(hex, durations)))