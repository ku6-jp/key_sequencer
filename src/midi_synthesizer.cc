#include "midi_synthesizer.h"

namespace keysequencer {

MidiSynthesizer::~MidiSynthesizer() {
  if (midi_out_handle_ != nullptr) {
    midiOutReset(midi_out_handle_);
    midiOutClose(midi_out_handle_);
  }
}

bool MidiSynthesizer::Open() {
  if (midi_out_handle_ != nullptr) {
    return true;
  }

  if (midiOutOpen(&midi_out_handle_, MIDIMAPPER, 0, 0, CALLBACK_NULL) !=
      MMSYSERR_NOERROR) {
    midi_out_handle_ = nullptr;
    return false;
  }

  return true;
}

void MidiSynthesizer::Close() {
  if (midi_out_handle_ != nullptr) {
    midiOutReset(midi_out_handle_);
    midiOutClose(midi_out_handle_);
    midi_out_handle_ = nullptr;
  }
}

void MidiSynthesizer::NoteOn(std::uint8_t const note_number) {
  if (midi_out_handle_ != nullptr) {
    // 0x00: 未使用
    // 0x7f: ベロシティ
    // 0xxx: ノート番号
    // 0x90: Note on + チャンネル番号
    DWORD message = 0x007f0090 | (static_cast<DWORD>(note_number) << 8);

    // 出力
    midiOutShortMsg(midi_out_handle_, message);
  }
}

void MidiSynthesizer::NoteOff(std::uint8_t const note_number) {
  if (midi_out_handle_ != nullptr) {
    // 0x00: 未使用
    // 0x00: ベロシティ(Note offにする早さだから0x7fとすべきか？)
    // 0xxx: ノート番号
    // 0x90: Note off + チャンネル番号
    DWORD message = 0x00000080 | (static_cast<DWORD>(note_number) << 8);

    // 出力
    midiOutShortMsg(midi_out_handle_, message);
  }
}

}  // namespace keysequencer