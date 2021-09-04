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
    // 0x00: ���g�p
    // 0x7f: �x���V�e�B
    // 0xxx: �m�[�g�ԍ�
    // 0x90: Note on + �`�����l���ԍ�
    DWORD message = 0x007f0090 | (static_cast<DWORD>(note_number) << 8);

    // �o��
    midiOutShortMsg(midi_out_handle_, message);
  }
}

void MidiSynthesizer::NoteOff(std::uint8_t const note_number) {
  if (midi_out_handle_ != nullptr) {
    // 0x00: ���g�p
    // 0x00: �x���V�e�B(Note off�ɂ��鑁��������0x7f�Ƃ��ׂ����H)
    // 0xxx: �m�[�g�ԍ�
    // 0x90: Note off + �`�����l���ԍ�
    DWORD message = 0x00000080 | (static_cast<DWORD>(note_number) << 8);

    // �o��
    midiOutShortMsg(midi_out_handle_, message);
  }
}

}  // namespace keysequencer