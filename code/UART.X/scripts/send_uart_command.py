import serial
import time 
from enum import Enum
import pandas as pd

ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=1)  # open serial port
notes_df = pd.read_csv("Imperial_March.tsv", sep="\t")
notes_df["Notes"] = notes_df["Notes"].fillna("Rest")

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

def convert_note_to_freqs(notes, octave):
    freqs = []
    for note in notes:

        offset = note.count("'")
        offset -= note.count(",")
        note = note.replace("'", "").replace(",", "")
        
        if note.strip() in notes_map.keys():
            freqs.append(round(notes_map[note] * 2**(octave+offset)))

    return freqs

def convert_beat_to_duration(beats, tempo):
    min_beat_frac = min(beats)
    min_time = int((60 / tempo) * 1000 * min_beat_frac)
    print(min_time)

    return [int(beat/min_beat_frac * min_time) for beat in beats]


class CMDS(Enum):
    LED = 0x13 
    SOUND_OFF = 0x14 
    SOUND_ON = 0x15
    UPDATE_NOTES = 0x16
    UPDATE_DURATION = 0x17
    UPDATE_BEAT = 0x18


class SPEICAL_CHARS(Enum):
    STX = 0x02
    ETX = 0x03
    ESCAPE = 0x1B

class USARTPacketGen(object):
    delay = 0.2
    format = "little"
    def __init__(self):
        pass
    
    @staticmethod
    def _crc_16(msg):
        lo = hi = 0xff
        mask = 0xff
        for new in msg:
            new ^= lo
            new ^= (new << 4) & mask
            tmp = new >> 5
            lo = hi
            hi = new ^ tmp
            lo ^= (new << 3) & mask
            lo ^= new >> 4
        lo ^= mask
        hi ^= mask
        return [hi, lo]

    def _add_crc_and_flags(self, data):
        data += self._crc_16(data)
        data = self._insert_escapses(data)
        data = [SPEICAL_CHARS.STX.value] + data + [SPEICAL_CHARS.ETX.value]
        return data

    def _insert_escapses(self, data):
        data_out = []

        speical_chars = [c.value for c in SPEICAL_CHARS]
        for val in data:
            if val in speical_chars:
                data_out.append(SPEICAL_CHARS.ESCAPE.value)
            data_out.append(val)

        return data_out

    def toggle_led(self):
        data_down = [CMDS.LED.value, 0]
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)

    def sound_off(self):
        data_down = [CMDS.SOUND_OFF.value, 0]
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)

    def sound_on(self):
        data_down = [CMDS.SOUND_ON.value, 0]
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)

    def update_beat(self, beats_per_min=60):
        beat_ms = int((60 / beats_per_min) * 1000)
        data = list(beat_ms.to_bytes(4, self.format))
        
        data_down = [CMDS.UPDATE_BEAT.value] + [len(data)] + data
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)

    def update_notes(self, notes):
        data = [byte for note in notes for byte in note.to_bytes(2, self.format)]
        data_down = [CMDS.UPDATE_NOTES.value] + [len(data)] + data
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)
        
    def update_duration(self, durations):
        data = [byte for duration in durations for byte in duration.to_bytes(2, self.format)]
        data_down = [CMDS.UPDATE_DURATION.value] + [len(data)] + data
        time.sleep(self.delay)
        return self._add_crc_and_flags(data_down)


p_gen = USARTPacketGen()

ser.write(p_gen.sound_off())


tempo = 90
notes = convert_note_to_freqs(list(notes_df["Notes"]), 5)
durations = convert_beat_to_duration(list(notes_df["Duration"]), tempo)

ser.write(p_gen.toggle_led())

ser.write(p_gen.sound_off())

exit()
# ser.write(p_gen.sound_off())
ser.write(p_gen.update_beat(60))

# notes = [0xde, 0xad, 0xde, 0xad, 0xab, 0xcd]
ser.write(p_gen.update_notes(notes))

ser.write(p_gen.update_duration(durations))

print(durations)
# exit()
time.sleep(0.5)
ser.write(p_gen.sound_on())


# ser.write(p_gen.update_beat(120))
# time.sleep(5)
# ser.write(p_gen.sound_off())
ser.close()             # close port

