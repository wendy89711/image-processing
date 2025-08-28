#include <stdio.h>
#include <stdlib.h>

#define BIWIDTH_OFFSET 18
#define BIHEIGHT_OFFSET 22
#define BIBITCOUNT_OFFSET 28
#define RAW_OFFSET 54
#define BIWIDTH_SIZE 4
#define BIHEIGHT_SIZE 4
#define BIBITCOUNT_SIZE 2
#define RANGE(X,Y,Z) X>=Y && X<(Y+Z) 

typedef unsigned char UINT8_t;
typedef unsigned short UINT16_t;
typedef unsigned int UINT32_t;
typedef struct {
	UINT8_t header[RAW_OFFSET];
	union {
		UINT32_t val;
		UINT8_t byte[4];
	} width, height;
	union {
		UINT16_t val;
		UINT8_t byte[2];
	} bit_count;
	UINT8_t* raw;
} IMG;

IMG read(const char* filename);
void write(const char* filename, IMG src);
IMG deep_copy(IMG src);
IMG negative(IMG src);
IMG rotation(IMG src, IMG mask, int matrix[2][2]);
IMG add(IMG src1, IMG src2);

int main(int argc, char** argv) {

	// 1. Image Read/Write
	const char* filename = "../data/InputImage1.bmp";
	IMG lena = read(filename);
	write("../data/lena.bmp", lena);

	// 2. Generate a color negative image
	const char* negative_name = "../output/negative_lena.bmp";
	IMG negative_lena = negative(lena);
	write(negative_name, negative_lena);

	// 3. Split and reassemble into new image.
	const char* reassemble_name = "../output/reassemble_lena.bmp";
	IMG mask = deep_copy(lena);
	int RAW_SIZE = mask.width.val * mask.height.val * mask.bit_count.val / 8;
	int depth = mask.bit_count.val / 8;
	float center_x = (mask.width.val - 1) / 2.;
	float center_y = (mask.height.val - 1) / 2.;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		int page = i % depth;
		int row = i / (mask.width.val * depth);
		int col = i % (mask.width.val * depth);
		col /= depth;
		bool c = (col - center_x) / (row - center_y) < 1 && (col - center_x) / (row - center_y) > -1;
		mask.raw[i] = c == true ? 255 : 0;
	}

	// 向左轉90度
	int matrix270[2][2] = { {0,1},{-1,0} }; 
	IMG im0 = rotation(lena, mask, matrix270);
	mask = negative(mask);

	// 向右轉90度
	int matrix90[2][2] = { {0,-1},{1,0} }; 
	IMG im1 = rotation(lena, mask, matrix90);
	
	IMG im_split = add(im0, im1);
	write(reassemble_name, im_split);
}

IMG read(const char* filename) {
	// rb+: 讀寫開啟一個二進位制檔案，只允許讀寫資料
	FILE* fpIn = fopen(filename, "rb");
	IMG dst;
	dst.width.val = 0;
	dst.height.val = 0;
	dst.bit_count.val = 0;
	for (size_t i = 0; i < RAW_OFFSET; i++) {
		dst.header[i] = fgetc(fpIn);
		if (RANGE(i, BIWIDTH_OFFSET, BIWIDTH_SIZE)) {
			int offset = i - BIWIDTH_OFFSET;
			dst.width.byte[offset] = dst.header[i];
		}
		else if (RANGE(i, BIHEIGHT_OFFSET, BIHEIGHT_SIZE)) {
			int offset = i - BIHEIGHT_OFFSET;
			dst.height.byte[offset] = dst.header[i];
		}
		else if (RANGE(i, BIBITCOUNT_OFFSET, BIBITCOUNT_SIZE)) {
			int offset = i - BIBITCOUNT_OFFSET;
			dst.bit_count.byte[offset] = dst.header[i];
		}
		else {
			continue;
		}
	}
	int RAW_SIZE = dst.width.val * dst.height.val * dst.bit_count.val / 8;
	dst.raw = (UINT8_t*)malloc(sizeof(UINT8_t) * RAW_SIZE);
	for (size_t i = 0; i < RAW_SIZE; i++) {
		dst.raw[i] = fgetc(fpIn);
	}
	fclose(fpIn);
	return dst;
}

void write(const char* filename, IMG src) {
	// wb+: 讀寫開啟或建立一個二進位制檔案，允許讀和寫
	FILE* fpOut = fopen(filename, "wb");
	for (size_t i = 0; i < RAW_OFFSET; i++) {
		if (RANGE(i, BIWIDTH_OFFSET, BIWIDTH_SIZE)) {
			int offset = i - BIWIDTH_OFFSET;
			fputc(src.width.byte[offset], fpOut);
		}
		else if (RANGE(i, BIHEIGHT_OFFSET, BIHEIGHT_SIZE)) {
			int offset = i - BIHEIGHT_OFFSET;
			fputc(src.height.byte[offset], fpOut);
		}
		else if (RANGE(i, BIBITCOUNT_OFFSET, BIBITCOUNT_SIZE)) {
			int offset = i - BIBITCOUNT_OFFSET;
			fputc(src.bit_count.byte[offset], fpOut);
		}
		else {
			fputc(src.header[i], fpOut);
		}
	}
	int RAW_SIZE = src.width.val * src.height.val * src.bit_count.val / 8;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		fputc(src.raw[i], fpOut);
	}
	fclose(fpOut);
}

IMG deep_copy(IMG src) {
	IMG dst;
	dst.bit_count = src.bit_count;
	dst.height = src.height;
	dst.width = src.width;
	for (size_t i = 0; i < RAW_OFFSET; i++) {
		dst.header[i] = src.header[i];
	}
	int RAW_SIZE = dst.width.val * dst.height.val * dst.bit_count.val / 8;
	dst.raw = (UINT8_t*)malloc(sizeof(UINT8_t) * RAW_SIZE);
	for (size_t i = 0; i < RAW_SIZE; i++) {
		dst.raw[i] = src.raw[i];
	}
	return dst;
}

IMG negative(IMG src) {
	IMG dst = deep_copy(src);
	int RAW_SIZE = dst.width.val * dst.height.val * dst.bit_count.val / 8;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		dst.raw[i] = ~src.raw[i];
	}
	return dst;
}

IMG rotation(IMG src, IMG mask, int matrix[2][2]) {
	IMG dst = deep_copy(src);
	int RAW_SIZE = dst.width.val * dst.height.val * dst.bit_count.val / 8;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		dst.raw[i] = src.raw[i] & mask.raw[i];
	}
	IMG dst_copy = deep_copy(dst);
	int depth = dst.bit_count.val / 8;
	float center_x = (dst.width.val - 1) / 2.;
	float center_y = (dst.height.val - 1) / 2.;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		int page = i % depth;
		int row = i / (mask.width.val * depth);
		int col = i % (mask.width.val * depth);
		col /= depth;
		int new_col = matrix[0][0] * (col - center_x) + matrix[0][1] * (row - center_y) + center_x;
		int new_row = matrix[1][0] * (col - center_x) + matrix[1][1] * (row - center_y) + center_y;
		bool c = new_col < dst.width.val&& new_row < dst.height.val;
		size_t new_i = new_row * mask.width.val * depth + new_col * depth + page;
		if (c) {
			dst.raw[new_i] = dst_copy.raw[i];
		}
	}
	return dst;
}

IMG add(IMG src1, IMG src2) {
	IMG dst = deep_copy(src1);
	int RAW_SIZE = dst.width.val * dst.height.val * dst.bit_count.val / 8;
	for (size_t i = 0; i < RAW_SIZE; i++) {
		int c = src1.raw[i] + src2.raw[i];
		dst.raw[i] = c > 255 ? 255 : c;
	}
	return dst;
}