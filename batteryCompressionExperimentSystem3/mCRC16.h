#ifndef TEST_H
#define TEST_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int CRC16(char *buffer,char num);//CRC16 校验返回处理
char toAscii(int number);//返回对应Ascii码

#ifdef __cplusplus
}
#endif

#endif // TEST_H
