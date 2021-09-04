#ifndef KEYSEQUENCER_LOGGER_H_
#define KEYSEQUENCER_LOGGER_H_

#include <tchar.h>

namespace keysequencer {

/**
 * @brief デバッグ出力にログを出力する
 * 
 * @param message メッセージ(sprinfの書式)
 */
extern void debug(const TCHAR* message, ...);

}  // namespace keysequencer
#endif