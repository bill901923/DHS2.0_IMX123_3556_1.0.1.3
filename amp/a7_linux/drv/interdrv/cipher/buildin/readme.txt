����Build in��Kernel�еķ�����

1. �ڽű���ָ����Ҫ�����kernle·������Ŀ¼��ִ�нű� ./patch.sh
   �ű�˵��:
   a) kernel�´����ļ��У�             mkdir drivers\crypto\hisi-cipher
   b) ��SDK��drv�����ŵ�����ļ����£�  board\drv\interdrv\common\cipher\src\drv -> drivers\crypto\hisi-cipher\src\drv
                                      board\drv\interdrv\common\cipher\include -> drivers\crypto\hisi-cipher\include
                                      ��Ŀ¼��ɾ���û�̬��ͷ�ļ�include\mm_ext.h
   c) drivers\crypto\Makefile�ļ��������һ�У�obj-y += hisi-cipher/

2. ���±���Kernel����

3. ����Kernel������Ҫ��hi_cipher.ko��ֱ������SDK��board\drv\interdrv\common\cipher\sample\sample_cipher_via���������Ƿ�Build in�ɹ�.
   ���Ҫ����MMZ�ļӽ��ܽӿڣ���Ҫ����hi_osal.ko

ע�⣺cipher��Build In��ʽ��֧�ֵ��㷨��
    * AES/TDES��ECB/CBC/CTR/CFB/OFBģʽ
    * ���Ҫ����CCM/GCM/MMZ�ļӽ��ܣ���Ҫ����hi_osal.ko
    * sha1/sha224/sha256/sha384/sha512
    * hmac1/hmac224/hmac256/hmac384/hmac512
    * rsa1024/rsa2048/rsa4096
    * trng