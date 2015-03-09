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
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

#include "src/common/stream.h"
#include "src/common/file.h"
#include "src/common/util.h"
#include "src/common/encoding.h"

#include "src/aurora/erffile.h"
#include "src/aurora/error.h"
#include "src/aurora/util.h"

#include <zlib.h>

static const uint32 kERFID     = MKTAG('E', 'R', 'F', ' ');
static const uint32 kMODID     = MKTAG('M', 'O', 'D', ' ');
static const uint32 kHAKID     = MKTAG('H', 'A', 'K', ' ');
static const uint32 kSAVID     = MKTAG('S', 'A', 'V', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', '.', '0');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');
static const uint32 kVersion2  = MKTAG('V', '2', '.', '0');
static const uint32 kVersion22 = MKTAG('V', '2', '.', '2');

namespace Aurora {

ERFFile::ERFFile(const Common::UString &fileName, bool noResources) :
	_noResources(noResources), _buildYear(0), _buildDay(0), _fileName(fileName) {

	load();
}

ERFFile::~ERFFile() {
}

void ERFFile::clear() {
	_resources.clear();
}

void ERFFile::load() {
	Common::File erf;
	open(erf);

	readHeader(erf);

	if ((_id != kERFID) && (_id != kMODID) && (_id != kHAKID) && (_id != kSAVID))
		throw Common::Exception("Not an ERF file");

	if ((_version != kVersion1) && (_version != kVersion11) &&
	    (_version != kVersion2) && (_version != kVersion22))
		throw Common::Exception("Unsupported ERF file version %08X", _version);

	if ((_version != kVersion1) && (_version != kVersion11) && !_utf16le)
		throw Common::Exception("ERF file version 2.0+, but not UTF-16LE");

	try {

		ERFHeader erfHeader;

		readERFHeader  (erf, erfHeader);
		readDescription(erf, erfHeader);

		if (!_noResources)
			readResources(erf, erfHeader);

		if (erf.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading ERF file");
		throw e;
	}

}

void ERFFile::readERFHeader(Common::SeekableReadStream &erf, ERFHeader &header) {
	uint32 resCount = 0;

	if        ((_version == kVersion1) || (_version == kVersion11)) {

		header.langCount = erf.readUint32LE(); // Number of languages for the description
		erf.skip(4);                           // Number of bytes in the description
		resCount         = erf.readUint32LE(); // Number of resources in the ERF

		header.offDescription = erf.readUint32LE();
		header.offKeyList     = erf.readUint32LE();
		header.offResList     = erf.readUint32LE();

		_buildYear = erf.readUint32LE() + 1900;
		_buildDay  = erf.readUint32LE();

		header.descriptionID = erf.readUint32LE();

		erf.skip(116); // Reserved

		_flags = 0; // No flags in ERF V1.0 / V1.1

	} else if (_version == kVersion2) {

		header.langCount = 0;                  // No description in ERF V2.0
		resCount         = erf.readUint32LE(); // Number of resources in the ERF

		header.descriptionID  = 0;    // No description in ERF V2.0
		header.offDescription = 0;    // No description in ERF V2.0
		header.offKeyList     = 0;    // No separate key list in ERF V2.0
		header.offResList     = 0x20; // Resource list always starts at 0x20 in ERF V2.0

		_buildYear = erf.readUint32LE() + 1900;
		_buildDay  = erf.readUint32LE();

		erf.skip(4);     // Unknown, always 0xFFFFFFFF?

		_flags = 0; // No flags in ERF V2.0

	} else if (_version == kVersion22) {

		header.langCount = 0;                  // No description in ERF V2.2
		resCount         = erf.readUint32LE(); // Number of resources in the ERF

		erf.skip(4 + 4); // Build year and day
		erf.skip(4);     // Unknown, always 0xFFFFFFFF?

		_flags = erf.readUint32LE();

		erf.skip(16); // Password digest

		header.descriptionID   = 0;    // No description in ERF V2.2
		header.offDescription  = 0;    // No description in ERF V2.2
		header.offKeyList      = 0;    // No separate key list in ERF V2.2
		header.offResList      = 0x38; // Resource list always starts at 0x38 in ERF V2.2
	}

	if (!_noResources) {
		_resources.resize(resCount);
		_iResources.resize(resCount);
	}
}

void ERFFile::readDescription(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (_version == kVersion1) {
		if (!erf.seek(header.offDescription))
			throw Common::Exception(Common::kSeekError);

		_description.readLocString(erf, header.descriptionID, header.langCount);
	}
}

void ERFFile::readResources(Common::SeekableReadStream &erf, const ERFHeader &header) {
	assert(!_noResources);

	if        (_version == kVersion1) {

		readV1KeyList(erf, header); // Read name and type part of the resource list
		readV1ResList(erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion11) {

		readV11KeyList(erf, header); // Read name and type part of the resource list
		readV1ResList (erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion2) {

		// Read the resource list
		readV2ResList(erf, header);

	} else if (_version == kVersion22) {

		// Read the resource list
		readV22ResList(erf, header);

	}

}

void ERFFile::readV1KeyList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (!erf.seek(header.offKeyList))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++index, ++res) {
		res->name = Common::readStringFixed(erf, Common::kEncodingASCII, 16);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
		res->index = index;
	}
}

void ERFFile::readV11KeyList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (!erf.seek(header.offKeyList))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++index, ++res) {
		res->name = Common::readStringFixed(erf, Common::kEncodingASCII, 32);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
		res->index = index;
	}
}

void ERFFile::readV1ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (!erf.seek(header.offResList))
		throw Common::Exception(Common::kSeekError);

	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		res->offset                         = erf.readUint32LE();
		res->packedSize = res->unpackedSize = erf.readUint32LE();
	}
}

void ERFFile::readV2ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (!erf.seek(header.offResList))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		Common::UString name = Common::readStringFixed(erf, Common::kEncodingUTF16LE, 64);

		res->name  = setFileType(name, kFileTypeNone);
		res->type  = getFileType(name);
		res->index = index;

		iRes->offset                          = erf.readUint32LE();
		iRes->packedSize = iRes->unpackedSize = erf.readUint32LE();
	}

}

void ERFFile::readV22ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	if (!erf.seek(header.offResList))
		throw Common::Exception(Common::kSeekError);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		Common::UString name = Common::readStringFixed(erf, Common::kEncodingUTF16LE, 64);

		res->name  = setFileType(name, kFileTypeNone);
		res->type  = getFileType(name);
		res->index = index;

		iRes->offset       = erf.readUint32LE();
		iRes->packedSize   = erf.readUint32LE();
		iRes->unpackedSize = erf.readUint32LE();
	}

}

uint32 ERFFile::getBuildYear() const {
	return _buildYear;
}

uint32 ERFFile::getBuildDay() const {
	return _buildDay;
}

const LocString &ERFFile::getDescription() const {
	return _description;
}

const Archive::ResourceList &ERFFile::getResources() const {
	return _resources;
}

const ERFFile::IResource &ERFFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%d/%d)", index, _iResources.size());

	return _iResources[index];
}

uint32 ERFFile::getResourceSize(uint32 index) const {
	return getIResource(index).unpackedSize;
}

Common::SeekableReadStream *ERFFile::getResource(uint32 index) const {
	const IResource &res = getIResource(index);
	if (res.unpackedSize == 0)
		return new Common::MemoryReadStream(0, 0);

	if (_flags & 0xF0)
		throw Common::Exception("Unhandled ERF encryption");

	Common::File erf;
	open(erf);

	if (!erf.seek(res.offset))
		throw Common::Exception(Common::kSeekError);

	byte *compressedData = new byte[res.packedSize];

	if (erf.read(compressedData, res.packedSize) != res.packedSize) {
		delete[] compressedData;
		throw Common::Exception(Common::kReadError);
	}

	return decompress(compressedData, res.packedSize, res.unpackedSize);
}

uint32 ERFFile::getCompressionType() const {
	return (_flags >> 29) & 0x7;
}

Common::SeekableReadStream *ERFFile::decompress(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const {
	switch (getCompressionType()) {
	case 0:
		// No compression
		return new Common::MemoryReadStream(compressedData, packedSize, true);
	case 1:
		// Bioware Zlib
		return decompressBiowareZlib(compressedData, packedSize, unpackedSize);
	case 2:
	case 3:
		// Unknown
		delete[] compressedData;
		throw Common::Exception("Unknown ERF compression %d", getCompressionType());
	case 7:
		// Headerless Zlib
		return decompressHeaderlessZlib(compressedData, packedSize, unpackedSize);
	default:
		// Invalid
		delete[] compressedData;
		throw Common::Exception("Invalid ERF compression %d", getCompressionType());
	}
}

Common::SeekableReadStream *ERFFile::decompressBiowareZlib(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const {
	try {
		Common::SeekableReadStream *stream = decompressZlib(compressedData + 1, packedSize - 1, unpackedSize, *compressedData >> 4);
		delete[] compressedData;
		return stream;
	} catch (Common::Exception &e) {
		delete[] compressedData;
		throw;
	}
}

Common::SeekableReadStream *ERFFile::decompressHeaderlessZlib(byte *compressedData, uint32 packedSize, uint32 unpackedSize) const {
	try {
		Common::SeekableReadStream *stream = decompressZlib(compressedData, packedSize, unpackedSize, MAX_WBITS);
		delete[] compressedData;
		return stream;
	} catch (Common::Exception &e) {
		delete[] compressedData;
		throw;
	}
}

Common::SeekableReadStream *ERFFile::decompressZlib(byte *compressedData, uint32 packedSize, uint32 unpackedSize, int windowBits) const {
	// Allocate the decompressed data
	byte *decompressedData = new byte[unpackedSize];

	z_stream strm;
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = packedSize;
	strm.next_in  = compressedData;

	// Negative windows bits means there is no zlib header present in the data.
	int zResult = inflateInit2(&strm, -windowBits);
	if (zResult != Z_OK) {
		inflateEnd(&strm);

		delete[] decompressedData;
		throw Common::Exception("Could not initialize zlib inflate");
	}

	strm.avail_out = unpackedSize;
	strm.next_out  = decompressedData;

	zResult = inflate(&strm, Z_SYNC_FLUSH);
	if (zResult != Z_OK && zResult != Z_STREAM_END) {
		inflateEnd(&strm);

		delete[] decompressedData;
		throw Common::Exception("Failed to inflate: %d", zResult);
	}

	inflateEnd(&strm);
	return new Common::MemoryReadStream(decompressedData, unpackedSize, true);
}

void ERFFile::open(Common::File &file) const {
	if (!file.open(_fileName))
		throw Common::Exception(Common::kOpenError);
}

} // End of namespace Aurora