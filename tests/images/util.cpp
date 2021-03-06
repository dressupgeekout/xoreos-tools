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
 *  Unit tests for our utility image functions.
 */

#include <cstring>

#include "gtest/gtest.h"

#include "src/common/error.h"

#include "src/images/util.h"

static const byte kImage1_1_3[] = { 0x00,0x01,0x02 };
static const byte kImage1_1_4[] = { 0x00,0x01,0x02,0x03 };

static const byte kImage2_2_3[] = { 0x00,0x01,0x02, 0x03,0x04,0x05,
                                    0x10,0x11,0x12, 0x13,0x14,0x15 };
static const byte kImage2_2_4[] = { 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
                                    0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17 };

static const byte kImage3_3_3[] = { 0x00,0x01,0x02, 0x03,0x04,0x05, 0x06,0x07,0x08,
                                    0x10,0x11,0x12, 0x13,0x14,0x15, 0x16,0x17,0x18,
                                    0x20,0x21,0x22, 0x23,0x24,0x25, 0x26,0x27,0x28 };
static const byte kImage3_3_4[] = { 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B,
                                    0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B,
                                    0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27, 0x28,0x29,0x2A,0x2B };

static const byte kImage2_3_3[] = { 0x00,0x01,0x02, 0x03,0x04,0x05,
                                    0x10,0x11,0x12, 0x13,0x14,0x15,
                                    0x20,0x21,0x22, 0x23,0x24,0x25 };
static const byte kImage2_3_4[] = { 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
                                    0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17,
                                    0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27 };

static const byte kImage3_2_3[] = { 0x00,0x01,0x02, 0x03,0x04,0x05, 0x06,0x07,0x08,
                                    0x10,0x11,0x12, 0x13,0x14,0x15, 0x16,0x17,0x18 };
static const byte kImage3_2_4[] = { 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B,
                                    0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B };


GTEST_TEST(ImagesUtil, getBPP) {
	EXPECT_EQ(Images::getBPP(Images::kPixelFormatR8G8B8), 3);
	EXPECT_EQ(Images::getBPP(Images::kPixelFormatB8G8R8), 3);

	EXPECT_EQ(Images::getBPP(Images::kPixelFormatR8G8B8A8), 4);
	EXPECT_EQ(Images::getBPP(Images::kPixelFormatB8G8R8A8), 4);

	EXPECT_EQ(Images::getBPP(Images::kPixelFormatA1R5G5B5), 2);

	EXPECT_EQ(Images::getBPP(Images::kPixelFormatR5G6B5), 2);

	EXPECT_EQ(Images::getBPP(Images::kPixelFormatDepth16), 2);

	EXPECT_EQ(Images::getBPP(Images::kPixelFormatDXT1), 0);
	EXPECT_EQ(Images::getBPP(Images::kPixelFormatDXT3), 0);
	EXPECT_EQ(Images::getBPP(Images::kPixelFormatDXT5), 0);
}

GTEST_TEST(ImagesUtil, getDataSize) {
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatA1R5G5B5, 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR5G6B5  , 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDepth16 , 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8  , 3, 3), 3 * 3 * 3);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8  , 3, 3), 3 * 3 * 3);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8A8, 3, 3), 3 * 3 * 4);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8A8, 3, 3), 3 * 3 * 4);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatA1R5G5B5, 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR5G6B5  , 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDepth16 , 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8  , 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8  , 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8A8, 0, 0), 0);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8A8, 0, 0), 0);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT1, 0, 0),  8);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT3, 0, 0), 16);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT5, 0, 0), 16);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT1, 3, 3),  8);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT3, 3, 3), 16);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT5, 3, 3), 16);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT1, 9, 9),  72);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT3, 9, 9), 144);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT5, 9, 9), 144);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT1, 16, 16), 128);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT3, 16, 16), 256);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDXT5, 16, 16), 256);

	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatA1R5G5B5, 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR5G6B5  , 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatDepth16 , 3, 3), 3 * 3 * 2);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8  , 3, 3), 3 * 3 * 3);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8  , 3, 3), 3 * 3 * 3);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatR8G8B8A8, 3, 3), 3 * 3 * 4);
	EXPECT_EQ(Images::getDataSize(Images::kPixelFormatB8G8R8A8, 3, 3), 3 * 3 * 4);

	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatA1R5G5B5, -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR5G6B5  , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDepth16 , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR8G8B8  , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatB8G8R8  , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR8G8B8A8, -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatB8G8R8A8, -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT1    , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT3    , -1, -1), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT5    , -1, -1), Common::Exception);

	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatA1R5G5B5, 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR5G6B5  , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDepth16 , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR8G8B8  , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatB8G8R8  , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatR8G8B8A8, 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatB8G8R8A8, 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT1    , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT3    , 0x8000, 0x8000), Common::Exception);
	EXPECT_THROW(Images::getDataSize(Images::kPixelFormatDXT5    , 0x8000, 0x8000), Common::Exception);
}

GTEST_TEST(ImagesUtil, hasValidDimensions) {
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatA1R5G5B5, 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR5G6B5  , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDepth16 , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8  , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8  , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8A8, 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8A8, 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT1    , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT3    , 0, 0));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT5    , 0, 0));

	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatA1R5G5B5, 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR5G6B5  , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDepth16 , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8  , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8  , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8A8, 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8A8, 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT1    , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT3    , 3, 3));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT5    , 3, 3));

	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatA1R5G5B5, 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR5G6B5  , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDepth16 , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8  , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8  , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8A8, 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8A8, 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT1    , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT3    , 4, 4));
	EXPECT_TRUE(Images::hasValidDimensions(Images::kPixelFormatDXT5    , 4, 4));

	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatA1R5G5B5, -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR5G6B5  , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDepth16 , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8  , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8  , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8A8, -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8A8, -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT1    , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT3    , -1, -1));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT5    , -1, -1));

	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatA1R5G5B5, 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR5G6B5  , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDepth16 , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8  , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8  , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatR8G8B8A8, 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatB8G8R8A8, 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT1    , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT3    , 0x8000, 0x8000));
	EXPECT_FALSE(Images::hasValidDimensions(Images::kPixelFormatDXT5    , 0x8000, 0x8000));
}

template<size_t N>
static void copyData(byte *data1, const byte (&data2)[N]) {
	std::memcpy(data1, data2, N);
}

template<size_t N>
static void compareData(const byte *data1, const byte (&data2)[N], size_t t) {
	for (size_t i = 0; i < N; i++)
		EXPECT_EQ(data1[i], data2[i]) << "At case " << t << ", index " << i;
}

GTEST_TEST(ImageUtil, flipHorizontally) {
	static const byte kImageFlipped1_1_3[] = { 0x00,0x01,0x02 };
	static const byte kImageFlipped1_1_4[] = { 0x00,0x01,0x02,0x03 };

	static const byte kImageFlipped2_2_3[] = { 0x03,0x04,0x05, 0x00,0x01,0x02,
	                                           0x13,0x14,0x15, 0x10,0x11,0x12 };
	static const byte kImageFlipped2_2_4[] = { 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03,
	                                           0x14,0x15,0x16,0x17, 0x10,0x11,0x12,0x13 };

	static const byte kImageFlipped3_3_3[] = { 0x06,0x07,0x08, 0x03,0x04,0x05, 0x00,0x01,0x02,
	                                           0x16,0x17,0x18, 0x13,0x14,0x15, 0x10,0x11,0x12,
	                                           0x26,0x27,0x28, 0x23,0x24,0x25, 0x20,0x21,0x22 };
	static const byte kImageFlipped3_3_4[] = { 0x08,0x09,0x0A,0x0B, 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03,
	                                           0x18,0x19,0x1A,0x1B, 0x14,0x15,0x16,0x17, 0x10,0x11,0x12,0x13,
	                                           0x28,0x29,0x2A,0x2B, 0x24,0x25,0x26,0x27, 0x20,0x21,0x22,0x23 };

	static const byte kImageFlipped2_3_3[] = { 0x03,0x04,0x05, 0x00,0x01,0x02,
	                                           0x13,0x14,0x15, 0x10,0x11,0x12,
	                                           0x23,0x24,0x25, 0x20,0x21,0x22 };
	static const byte kImageFlipped2_3_4[] = { 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03,
	                                           0x14,0x15,0x16,0x17, 0x10,0x11,0x12,0x13,
	                                           0x24,0x25,0x26,0x27, 0x20,0x21,0x22,0x23 };

	static const byte kImageFlipped3_2_3[] = { 0x06,0x07,0x08, 0x03,0x04,0x05, 0x00,0x01,0x02,
	                                           0x16,0x17,0x18, 0x13,0x14,0x15, 0x10,0x11,0x12 };
	static const byte kImageFlipped3_2_4[] = { 0x08,0x09,0x0A,0x0B, 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03,
	                                           0x18,0x19,0x1A,0x1B, 0x14,0x15,0x16,0x17, 0x10,0x11,0x12,0x13 };

	byte buffer[3 * 3 * 4];

	copyData(buffer, kImage1_1_3);
	Images::flipHorizontally(buffer, 1, 1, 3);
	compareData(buffer, kImageFlipped1_1_3, 3);

	copyData(buffer, kImage1_1_4);
	Images::flipHorizontally(buffer, 1, 1, 4);
	compareData(buffer, kImageFlipped1_1_4, 3);

	copyData(buffer, kImage2_2_3);
	Images::flipHorizontally(buffer, 2, 2, 3);
	compareData(buffer, kImageFlipped2_2_3, 3);

	copyData(buffer, kImage2_2_4);
	Images::flipHorizontally(buffer, 2, 2, 4);
	compareData(buffer, kImageFlipped2_2_4, 3);

	copyData(buffer, kImage3_3_3);
	Images::flipHorizontally(buffer, 3, 3, 3);
	compareData(buffer, kImageFlipped3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::flipHorizontally(buffer, 3, 3, 4);
	compareData(buffer, kImageFlipped3_3_4, 3);

	copyData(buffer, kImage2_3_3);
	Images::flipHorizontally(buffer, 2, 3, 3);
	compareData(buffer, kImageFlipped2_3_3, 3);

	copyData(buffer, kImage2_3_4);
	Images::flipHorizontally(buffer, 2, 3, 4);
	compareData(buffer, kImageFlipped2_3_4, 3);

	copyData(buffer, kImage3_2_3);
	Images::flipHorizontally(buffer, 3, 2, 3);
	compareData(buffer, kImageFlipped3_2_3, 3);

	copyData(buffer, kImage3_2_4);
	Images::flipHorizontally(buffer, 3, 2, 4);
	compareData(buffer, kImageFlipped3_2_4, 3);


	copyData(buffer, kImage1_1_3);
	Images::flipHorizontally(buffer, 1, 1, 0);
	compareData(buffer, kImageFlipped1_1_3, 3);

	copyData(buffer, kImage1_1_3);
	Images::flipHorizontally(buffer, 0, 0, 3);
	compareData(buffer, kImageFlipped1_1_3, 3);

	copyData(buffer, kImage1_1_3);
	Images::flipHorizontally(buffer, 0, 0, 0);
	compareData(buffer, kImageFlipped1_1_3, 3);
}

GTEST_TEST(ImageUtil, flipVertically) {
	static const byte kImageFlipped1_1_3[] = { 0x00,0x01,0x02 };
	static const byte kImageFlipped1_1_4[] = { 0x00,0x01,0x02,0x03 };

	static const byte kImageFlipped2_2_3[] = { 0x10,0x11,0x12, 0x13,0x14,0x15,
	                                           0x00,0x01,0x02, 0x03,0x04,0x05 };
	static const byte kImageFlipped2_2_4[] = { 0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17,
	                                           0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07 };

	static const byte kImageFlipped3_3_3[] = { 0x20,0x21,0x22, 0x23,0x24,0x25, 0x26,0x27,0x28,
	                                           0x10,0x11,0x12, 0x13,0x14,0x15, 0x16,0x17,0x18,
	                                           0x00,0x01,0x02, 0x03,0x04,0x05, 0x06,0x07,0x08 };
	static const byte kImageFlipped3_3_4[] = { 0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27, 0x28,0x29,0x2A,0x2B,
	                                           0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B,
	                                           0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B };

	static const byte kImageFlipped2_3_3[] = { 0x20,0x21,0x22, 0x23,0x24,0x25,
	                                           0x10,0x11,0x12, 0x13,0x14,0x15,
	                                           0x00,0x01,0x02, 0x03,0x04,0x05 };
	static const byte kImageFlipped2_3_4[] = { 0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27,
	                                           0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17,
	                                           0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07 };

	static const byte kImageFlipped3_2_3[] = { 0x10,0x11,0x12, 0x13,0x14,0x15, 0x16,0x17,0x18,
	                                           0x00,0x01,0x02, 0x03,0x04,0x05, 0x06,0x07,0x08 };
	static const byte kImageFlipped3_2_4[] = { 0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B,
	                                           0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B };

	byte buffer[3 * 3 * 4];

	copyData(buffer, kImage1_1_3);
	Images::flipVertically(buffer, 1, 1, 3);
	compareData(buffer, kImageFlipped1_1_3, 3);

	copyData(buffer, kImage1_1_4);
	Images::flipVertically(buffer, 1, 1, 4);
	compareData(buffer, kImageFlipped1_1_4, 3);

	copyData(buffer, kImage2_2_3);
	Images::flipVertically(buffer, 2, 2, 3);
	compareData(buffer, kImageFlipped2_2_3, 3);

	copyData(buffer, kImage2_2_4);
	Images::flipVertically(buffer, 2, 2, 4);
	compareData(buffer, kImageFlipped2_2_4, 3);

	copyData(buffer, kImage3_3_3);
	Images::flipVertically(buffer, 3, 3, 3);
	compareData(buffer, kImageFlipped3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::flipVertically(buffer, 3, 3, 4);
	compareData(buffer, kImageFlipped3_3_4, 3);

	copyData(buffer, kImage2_3_3);
	Images::flipVertically(buffer, 2, 3, 3);
	compareData(buffer, kImageFlipped2_3_3, 3);

	copyData(buffer, kImage2_3_4);
	Images::flipVertically(buffer, 2, 3, 4);
	compareData(buffer, kImageFlipped2_3_4, 3);

	copyData(buffer, kImage3_2_3);
	Images::flipVertically(buffer, 3, 2, 3);
	compareData(buffer, kImageFlipped3_2_3, 3);

	copyData(buffer, kImage3_2_4);
	Images::flipVertically(buffer, 3, 2, 4);
	compareData(buffer, kImageFlipped3_2_4, 3);


	copyData(buffer, kImage3_3_3);
	Images::flipVertically(buffer, 1, 1, 0);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::flipVertically(buffer, 0, 0, 3);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::flipVertically(buffer, 0, 0, 0);
	compareData(buffer, kImage3_3_3, 3);
}

GTEST_TEST(ImageUtil, rotate90) {
	static const byte kImage090Rot3_3_3[] = { 0x20,0x21,0x22, 0x10,0x11,0x12, 0x00,0x01,0x02,
                                            0x23,0x24,0x25, 0x13,0x14,0x15, 0x03,0x04,0x05,
                                            0x26,0x27,0x28, 0x16,0x17,0x18, 0x06,0x07,0x08 };
	static const byte kImage090Rot3_3_4[] = { 0x20,0x21,0x22,0x23, 0x10,0x11,0x12,0x13, 0x00,0x01,0x02,0x03,
                                            0x24,0x25,0x26,0x27, 0x14,0x15,0x16,0x17, 0x04,0x05,0x06,0x07,
                                            0x28,0x29,0x2A,0x2B, 0x18,0x19,0x1A,0x1B, 0x08,0x09,0x0A,0x0B };

	static const byte kImage180Rot3_3_3[] = { 0x26,0x27,0x28, 0x23,0x24,0x25, 0x20,0x21,0x22,
                                            0x16,0x17,0x18, 0x13,0x14,0x15, 0x10,0x11,0x12,
                                            0x06,0x07,0x08, 0x03,0x04,0x05, 0x00,0x01,0x02 };
	static const byte kImage180Rot3_3_4[] = { 0x28,0x29,0x2A,0x2B, 0x24,0x25,0x26,0x27, 0x20,0x21,0x22,0x23,
                                            0x18,0x19,0x1A,0x1B, 0x14,0x15,0x16,0x17, 0x10,0x11,0x12,0x13,
                                            0x08,0x09,0x0A,0x0B, 0x04,0x05,0x06,0x07, 0x00,0x01,0x02,0x03 };

	static const byte kImage270Rot3_3_3[] = { 0x06,0x07,0x08, 0x16,0x17,0x18, 0x26,0x27,0x28,
                                            0x03,0x04,0x05, 0x13,0x14,0x15, 0x23,0x24,0x25,
                                            0x00,0x01,0x02, 0x10,0x11,0x12, 0x20,0x21,0x22 };
	static const byte kImage270Rot3_3_4[] = { 0x08,0x09,0x0A,0x0B, 0x18,0x19,0x1A,0x1B, 0x28,0x29,0x2A,0x2B,
                                            0x04,0x05,0x06,0x07, 0x14,0x15,0x16,0x17, 0x24,0x25,0x26,0x27,
                                            0x00,0x01,0x02,0x03, 0x10,0x11,0x12,0x13, 0x20,0x21,0x22,0x23 };

	static const byte kImage360Rot3_3_3[] = { 0x00,0x01,0x02, 0x03,0x04,0x05, 0x06,0x07,0x08,
                                            0x10,0x11,0x12, 0x13,0x14,0x15, 0x16,0x17,0x18,
                                            0x20,0x21,0x22, 0x23,0x24,0x25, 0x26,0x27,0x28 };
	static const byte kImage360Rot3_3_4[] = { 0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07, 0x08,0x09,0x0A,0x0B,
                                            0x10,0x11,0x12,0x13, 0x14,0x15,0x16,0x17, 0x18,0x19,0x1A,0x1B,
                                            0x20,0x21,0x22,0x23, 0x24,0x25,0x26,0x27, 0x28,0x29,0x2A,0x2B };

	byte buffer[3 * 3 * 4];

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 3, 1);
	compareData(buffer, kImage090Rot3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::rotate90(buffer, 3, 3, 4, 1);
	compareData(buffer, kImage090Rot3_3_4, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 3, 2);
	compareData(buffer, kImage180Rot3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::rotate90(buffer, 3, 3, 4, 2);
	compareData(buffer, kImage180Rot3_3_4, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 3, 3);
	compareData(buffer, kImage270Rot3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::rotate90(buffer, 3, 3, 4, 3);
	compareData(buffer, kImage270Rot3_3_4, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 3, 4);
	compareData(buffer, kImage360Rot3_3_3, 3);

	copyData(buffer, kImage3_3_4);
	Images::rotate90(buffer, 3, 3, 4, 4);
	compareData(buffer, kImage360Rot3_3_4, 3);


	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 3, 0);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 3, 3, 0, 1);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 0, 0, 3, 1);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 0, 0, 0, 1);
	compareData(buffer, kImage3_3_3, 3);

	copyData(buffer, kImage3_3_3);
	Images::rotate90(buffer, 0, 0, 0, 0);
	compareData(buffer, kImage3_3_3, 3);
}

GTEST_TEST(ImagesUtil, deSwizzleOffset) {
	static const uint32 kWidth = 4, kHeight = 4;
	static const uint32 kSwizzled[kWidth * kHeight] = {
		 0,  1,  4,  5,
		 2,  3,  6,  7,
		 8,  9, 12, 13,
		10, 11, 14, 15,
	};

	uint32 buffer[kWidth * kHeight];

	for (uint32 y = 0; y < kHeight; y++)
		for (uint32 x = 0; x < kWidth; x++)
			buffer[y * kHeight + x] = Images::deSwizzleOffset(x, y, kWidth, kHeight);

	for (size_t i = 0; i < (kWidth * kHeight); i++)
		EXPECT_EQ(buffer[i], kSwizzled[i]) << "At index " << i;
}
