#ifndef KEYSEQUENCER_LOGGER_H_
#define KEYSEQUENCER_LOGGER_H_

#include <tchar.h>

namespace keysequencer {

/**
 * @brief �f�o�b�O�o�͂Ƀ��O���o�͂���
 * 
 * @param message ���b�Z�[�W(sprinf�̏���)
 */
extern void debug(const TCHAR* message, ...);

}  // namespace keysequencer
#endif