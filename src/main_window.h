#ifndef KEYSEQUENCER_MAINWINDOW_H_
#define KEYSEQUENCER_MAINWINDOW_H_

#include <Windows.h>
#include <tchar.h>

#include "setting.h"

namespace keysequencer {

class StringTable;
class MidiDeviceInputToKeyboard;
class SequencerManager;

/**
 * @brief ���C���E�B���h�E
 */
class MainWindow {
 public:
  /**
   * @brief �f�t�H���g�R���X�g���N�^
   */
  MainWindow() = delete;

  /**
   * @brief �R���X�g���N�^
   *
   * @param instance_handle �A�v���P�[�V�����̃C���X�^���X�n���h��
   * @param string_table ���\�[�X�̕�����e�[�u��
   * @param setting �ݒ�
   * @param midi_input_to_keyboard MIDI���͂��L�[�{�[�h���͂ɕϊ����鏈��
   * @param sequencer_manager �������t����
   */
  MainWindow(HINSTANCE instance_handle, const StringTable* string_table, Setting* setting, MidiDeviceInputToKeyboard* midi_input_to_keyboard, SequencerManager* sequencer_manager);

  /**
   * @brief �R�s�[�R���X�g���N�^
   */
  MainWindow(const MainWindow&) = delete;

  /**
   * @brief ���[�u�R���X�g���N�^
   */
  MainWindow(MainWindow&&) = delete;

  /**
   * @brief �R�s�[������Z�q
   */
  MainWindow& operator=(const MainWindow&) = delete;

  /**
   * @brief ���[�u������Z�q
   */
  MainWindow& operator=(MainWindow&&) = delete;

  /**
   * @brief �f�X�g���N�^
   */
  ~MainWindow() = default;

  /**
   * @brief �E�B���h�E�̐�������
   * 
   * �E�B���h�E�����̐��ۂ𔻒肷�邽�߂ɃR���X�g���N�^�Ƃ͕ʂɂ����E�B���h�E��������
   * 1�x�������s����
   * ����2��ڈȍ~�̌Ăяo���������ꍇ�͉������Ȃ�
   *
   * @return ����ɏI�������ꍇ��true��Ԃ�
   */
  bool Create();

  /**
   * @brief �E�B���h�E�̃��b�Z�[�W����
   *
   * �E�B���h�E�����݂�����菈�����p������
   * �E�B���h�E������ꂽ���ɏ������Ԃ�
   */
  void Run();

 private:
  /**
   * @brief Window class��o�^���鎞�̖���
   */
  constexpr static const TCHAR* kWindowClassName = _T("MainWindow");

  /**
   * @brief �N���C�A���g�̈�̕�
   */
  constexpr static int kClientWidth = 480;

  /**
   * @brief �N���C�A���g�̈�̍���
   */
  constexpr static int kClientHeight = 8 + 30 + 8 + 30 + 8 + 30 + 8;

  /**
   * @brief �L�[�o�C���h�ݒ�{�^����ID
   */
  constexpr static WORD kKeyBindButtonId = 100;

  /**
   * @brief �e�X�g�Đ�/��~�{�^����ID
   */
  constexpr static WORD kTestPlayButtonId = 101;

  /**
   * @brief Hotkey: �Đ�/��~�o�^�p��ID
   */
  constexpr static int kPlayHotkeyId = 100;

  /**
   * @brief Hotkey: MIDI���͓o�^�p��ID
   */
  constexpr static int kMidiInputHotkeyId = 101;

  /**
   * @brief Hotkey: �S��~
   */
  constexpr static int kAllStopHotkeyId = 102;

  /**
   * @brief ���\�[�X�̕�����e�[�u��
   */
  const StringTable* const string_table_;

  /**
   * @brief �A�v���P�[�V�����̃C���X�^���X�n���h��
   */
  HINSTANCE const instance_handle_;

  /**
   * @brief �ݒ�
   */
  Setting* const setting_;

  /**
   * @brief MIDI���͏���
   */
  MidiDeviceInputToKeyboard* const midi_input_to_keyboard_;

  /**
   * @brief ���t����
   */
  SequencerManager* const sequencer_manager_;

  /**
   * @brief �E�B���h�E�����ς݃t���O
   */
  bool created_ = false;

  /**
   * @brief �E�B���h�E�n���h��
   */
  HWND window_handle_ = nullptr;

  /**
   * @brief �t�@�C�����\����static text�n���h��
   */
  HWND file_text_handle_ = nullptr;

  /**
   * @brief �Đ��X�e�[�^�X�\����static text�n���h��
   */
  HWND status_text_handle_ = nullptr;

  /**
   * @brief �L�[�o�C���h���J��button�̃n���h��
   */
  HWND open_key_bind_button_hanndle_ = nullptr;

  /**
   * @brief �e�X�g�Đ�/��~�{�^���n���h��
   */
  HWND test_play_button_handle_ = nullptr;

  /**
   * @brief �o�^���ꂽ�Đ�/��~�z�b�g�L�[�o�C���h
   */
  Setting::KeyBind registered_play_hotkey_bind_ = 0;

  /**
   * @brief �o�^���ꂽMIDI���̓z�b�g�L�[�o�C���h
   */
  Setting::KeyBind registered_midi_input_hotkey_bind_ = 0;

  /**
   * @brief �o�^���ꂽ�S��~�z�b�g�L�[�o�C���h
   */
  Setting::KeyBind registered_all_stop_hotkey_bind_ = 0;

  /**
   * @brief Window class�̓o�^�������1�x�����s�������p�̊֐�
   *
   * Window class�̓o�^���s��
   *
   * @param instanceHandle �A�v���P�[�V�����̃C���X�^���X�n���h��
   *
   * @return �Œ��0��Ԃ�
   */
  static int RegisterWindowClass(HINSTANCE instance_handle);

  /**
   * @brief �V�X�e������R�[���o�b�N�����E�B���h�E�v���V�[�W��
   */
  static LRESULT WINAPI CallbackProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

  /**
   * @brief �E�B���h�E�v���V�[�W���������e
   * 
   * Windows����Ăяo���ꂽCallbackProcedure���ŃC���X�^���X��������������ɏ������Ϗ�����
   * ����āA�����Ɩ߂�l�̓E�B���h�E�v���V�[�W���̎d�l�ɏ]��
   * 
   * @param window_handle �E�B���h�E�n���h��
   * @param message ���b�Z�[�W
   * @param wparam ���b�Z�[�W�p�����[�^1
   * @param lparam ���b�Z�[�W�p�����[�^2
   * 
   * @return ����
   */
  LRESULT Procedure(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);

  /**
   * @brief �z�b�g�L�[��o�^����
   */
  void RegisterHotkeys();

  /**
   * @brief �z�b�g�L�[����������
   */
  void UnregisterHotkeys();

  /**
   * @brief ���t��Ԃ����ɕ\����؂�ւ���
   */
  void RefreshStatus();
};

}  // namespace keysequencer

#endif