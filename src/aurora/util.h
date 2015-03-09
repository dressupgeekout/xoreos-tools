/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos-tools is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos-tools is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos-tools. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_UTIL_H
#define AURORA_UTIL_H

#include "src/aurora/types.h"

namespace Aurora {

class UString;

/** Return the human readable string of a Platform. */
Common::UString getPlatformDescription(Platform platform);

/** Return the file type of a file name, detected by its extension. */
FileType getFileType(const Common::UString &path);

/** Alias the file type according to specific rules per game. */
FileType aliasFileType(FileType type, GameID game);

/** Return the file name with a swapped extensions according to the specified file type. */
Common::UString setFileType(const Common::UString &path, FileType type);

} // End of namespace Aurora

#endif // AURORA_UTIL_H