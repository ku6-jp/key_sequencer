#include "midi_device_input_to_keyboard.h"

#include "keyboard.h"
#include "setting.h"
#include "standard_midi_format_event.h"

namespace keysequencer {

MidiDeviceInputToKeyboard::MidiDeviceInputToKeyboard(const Setting* setting, Keyboard* keyboard) : setting_(setting), keyboard_(keyboard) {
}

MidiDeviceInputToKeyboard::~MidiDeviceInputToKeyboard() {
  if (midi_in_handle_ != nullptr) {
    midiInStop(midi_in_handle_);
    midiInReset(midi_in_handle_);  // Stop�œ��͎�t���~���āAReset�ŃL���[�Ɏc�����f�[�^�̏����E�E�E�炵���B
    midiInClose(midi_in_handle_);
  }
}

MidiDeviceInputToKeyboard::OpenResult MidiDeviceInputToKeyboard::Open() {
  if (midi_in_handle_ != nullptr) {
    return OpenResult::kSuccess;
  }

  // MIDI���͋@�킪1�ȏ�t���Ă��邩���肷��
  UINT count = midiInGetNumDevs();
  if (count == 0) {
    return OpenResult::kFailureDeviceNotFound;
  }

  // MIDI���͋@�������ȂɈ�t�t���Ă����Ԃ͑z�肵�Ă��Ȃ��̂ōŏ���1��ΏۂƂ���
  if (midiInOpen(&midi_in_handle_, 0, reinterpret_cast<DWORD_PTR>(CallbackProcedure), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
    midi_in_handle_ = nullptr;
    return OpenResult::kFailureUnknown;
  }

  if (midiInStart(midi_in_handle_) != MMSYSERR_NOERROR) {
    midiInClose(midi_in_handle_);
    midi_in_handle_ = nullptr;
    return OpenResult::kFailureUnknown;
  }

  return OpenResult::kSuccess;
}

void MidiDeviceInputToKeyboard::Close() {
  if (midi_in_handle_ != nullptr) {
    midiInStop(midi_in_handle_);
    midiInReset(midi_in_handle_);  // Stop�œ��͎�t���~���āAReset�ŃL���[�Ɏc�����f�[�^�̏����E�E�E�炵���B
    midiInClose(midi_in_handle_);
    midi_in_handle_ = nullptr;
  }
}

void CALLBACK MidiDeviceInputToKeyboard::CallbackProcedure(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD dwParam1, DWORD dwParam2) {
  if (dwInstance == 0) {
    return;
  }

  reinterpret_cast<MidiDeviceInputToKeyboard*>(dwInstance)->Procedure(wMsg, dwParam1);
}

void MidiDeviceInputToKeyboard::Procedure(UINT message, DWORD param1) {
  // �ǂ����悤���Ȃ��̂ŏC���L�[�̃A�b�v�_�E���̓L�[�̃A�b�v�_�E���Ɠ����ɂȂ�
  switch (message) {
    case MIM_DATA:
    case MIM_LONGDATA:
      switch (param1 & 0xf0) {
        case static_cast<DWORD>(StandardMidiFormatEvent::MidiType::kNoteOn): {
          Setting::KeyBind key_bind = setting_->GetKeyBindByNoteNumber((param1 >> 8) & 0xff);
          if (key_bind != 0) {
            if ((key_bind & Setting::kShiftKeyBind) != 0) {
              keyboard_->Down(VK_SHIFT);
            }
            if ((key_bind & Setting::kControlKeyBind) != 0) {
              keyboard_->Down(VK_CONTROL);
            }
            if ((key_bind & Setting::kAlternateKeyBind) != 0) {
              keyboard_->Down(VK_MENU);
            }

            keyboard_->Down(key_bind & 0x00ff);
          }
        } break;
        case static_cast<DWORD>(StandardMidiFormatEvent::MidiType::kNoteOff): {
          Setting::KeyBind key_bind = setting_->GetKeyBindByNoteNumber((param1 >> 8) & 0xff);
          if (key_bind != 0) {
            keyboard_->Up(key_bind & 0x00ff);

            if ((key_bind & Setting::kAlternateKeyBind) != 0) {
              keyboard_->Up(VK_MENU);
            }
            if ((key_bind & Setting::kControlKeyBind) != 0) {
              keyboard_->Up(VK_CONTROL);
            }
            if ((key_bind & Setting::kShiftKeyBind) != 0) {
              keyboard_->Up(VK_SHIFT);
            }
          }
        } break;
      }
      break;
  }
}

}  // namespace keysequencer