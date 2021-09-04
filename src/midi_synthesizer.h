#ifndef KEYSEQUENCER_MIDISYNTHESIZER_H_
#define KEYSEQUENCER_MIDISYNTHESIZER_H_

#include <Windows.h>

#include <cstdint>

namespace keysequencer {
/**
 * @brief MIDI�V���Z�T�C�U�[
 *
 * SMF�̉��t�𐧌䂷��킯�ł͂Ȃ��A�P�ɉ��̏o�͂�ON/OFF����̂�
 */
class MidiSynthesizer {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  MidiSynthesizer() = default;

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  MidiSynthesizer(const MidiSynthesizer&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  MidiSynthesizer(MidiSynthesizer&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  MidiSynthesizer& operator=(const MidiSynthesizer&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  MidiSynthesizer& operator=(MidiSynthesizer&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~MidiSynthesizer();

  /**
   * @brief ���t���J�n����
   */
  bool Open();

  /**
   * @brief ���t���I������
   */
  void Close();

  /**
   * @brief �w��m�[�g�̉��̏o�͂��J�n����
   */
  void NoteOn(std::uint8_t const note_number);

  /**
   * @brief �w��m�[�g�̉��̏o�͂��I������
   */
  void NoteOff(std::uint8_t const note_number);

 private:
  /**
   * MIDI�o�̓n���h��
   */
  HMIDIOUT midi_out_handle_ = nullptr;
};

}  // namespace keysequencer

#endif