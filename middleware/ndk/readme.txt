NDK��д�ο�(��Hi3559ADemo��Ϊ��)��

A.��дmulticore��ϵͳ�汾��
  1.����ndk��osdrv�ı��뻷����
    �޸�ndk��Ŀ¼��Makefile.param�ļ�����OSDRV_ROOT·���޸�Ϊ��Ӧ�汾��������osdrv��Ŀ¼
  2.��ndk��Ŀ¼ִ��make linux����
  3.���岦��SW1[1:2]����0��SPI Flash����, SW1.4����0��A53MP����
  4.ʹ��HiTool���ڷ�ʽ��д������multicoreСϵͳ
	����¼��ַ�ͷ�����
	|     1M       |      9M       |      16M      |
	|--------------|---------------|---------------|
	|    uboot     |     kernel    |     rootfs    |
	����д����
	uboot��u-boot-hi3559av100.bin
	kernel��uImage_hi3559av100_multi-core
	rootfs��rootfs_hi3559av100_2k_24bit.yaffs2
	����������������
	setenv bootargs 'mem=512M console=ttyAMA0,115200 root=/dev/mtdblock2 rw rootfstype=yaffs2 mtdparts=hinand:1M(boot),9M(kernel),16M(rootfs)'
	setenv bootcmd 'nand read 0x44000000 0x100000 0x900000;bootm 0x44000000'
	saveenv
	reset
  5.����ko������
  	cd [SDK������·��]/mpp/out/linux/multi-core/ko/
  	./load3559av100_multicore -i -sensor0 imx477
  6.ָ����Ƶ��̬��·����
  	export LD_LIBRARY_PATH=[SDK������·��]/mpp/out/linux/multi-core/lib/

B.��дBigLittle˫ϵͳ�汾��
  1.����ndk��osdrv�ı��뻷����
    �޸�ndk��Ŀ¼��Makefile.param�ļ�����OSDRV_ROOT·���޸�Ϊ��Ӧ�汾��������osdrv��Ŀ¼
  2.��ndk��Ŀ¼ִ��make dual���루����ǰ��make clean,���֮ǰ��.o�ļ�����ΪserverĿ¼�µ�ϵͳ��˫ϵͳ���������.o�ļ���ͬ��
  3.���岦��SW1[1:2]����0��SPI Flash����, SW1.4����0��A53MP����
  4.ʹ��HiTool���ڷ�ʽ��д������BigLittleСϵͳ
    ����¼��ַ�ͷ�����
	|     1M       |      9M       |      32M      |      16M      |      1M      |
	|--------------|---------------|---------------|---------------|--------------|
	|    uboot     |     kernel    |     rootfs    |   a53_liteos  |   m7_liteos  |
	����д����
	uboot��u-boot-hi3559av100.bin
	kernel��uImage_hi3559av100_big-little
	rootfs��rootfs_hi3559av100_2k_24bit.yaffs2
	a53_liteos��[ndk��Ŀ¼]/sample/HuaweiLite/LiteOS.bin
	m7_liteos��û�õ�Ҳ������¼����������m7Liteos��sample����make�������£�
			   ��SDK������osdrvĿ¼��ִ��make BOOT_MEDIA=spi AMP_TYPE=linux_liteos all  �ٽ���platform/liteos_m7/liteos/sample/sample_osdrvĿ¼ִ��make����sample.bin
	����������������
	setenv bootargs 'mem=512M console=ttyAMA0,115200 clk_ignore_unused rw root=/dev/mtdblock2 rootfstype=yaffs2 mtdparts=hinand:1M(boot),9M(kernel),32M(rootfs)'
	setenv bootcmd  'nand read 0x45000000 0x2A00000 0x1000000; go_a53up 0x45000000; config_m7; nand read 0x19000000 0x3A00000 0x100000; go_m7; nand read 0x52000000 0x100000 0x900000;bootm 0x52000000'
	saveenv
	reset
  5.����ko������
	insmod /komod/hi_ipcm.ko
	cd [SDK������·��]/mpp/out/linux/big-little/ko/
	./loadhi3559av100_bl -i
  6.�鿴a53 LiteOS����־��
	������Linux���������У�
	ifconfig eth0 ����ip
	telnetd &
	insmod /komod/hi_virt-tty.ko
	��Ȼ������һ�����������������У�
	telnet ���IP (�û���root �����)
	virt-tty a53 (С���ʻᱨ���Ҳ���a53dev��Ҫ��������)

C.����������˵��
    �ٵ�ǰ��֧��multi-core�汾
	
    �ڽ�i2c_bus11 ��ʱ��Ƶ�ʴ�100000 �ĳ� 400000
	   �޸��ļ�·����osdrv/opensource/kernel/linux-4.9.y_multi-core/arch/arm64/boot/dts/hisilicon/hi3559av100.dtsi
	   �ĳ�����
	   i2c_bus11: i2c@1211b000 {
			compatible = "hisilicon,hibvt-i2c";
			reg = <0x1211b000 0x1000>;
			clocks = <&clock HI3559AV100_I2C11_CLK>;
			clock-frequency = <400000>;
			dmas = <&hiedmacv310_1 0 32>, <&hiedmacv310_1 1 33>;
			dma-names = "tx","rx";
			status = "disabled";
	    }
	�޸���ɺ����±��뾵��Ȼ����¼�µľ��񼴿�
	
	���޸�source\ndk\config.mak  ��DIS��GYRO������Ϊ��
	  CFG_SUPPORT_DIS ?= y
	  CFG_SUPPORT_GYRO ?= y
	
	�ܼ�ͷ�ļ�����ǰSDK�汾�����������ĸ�ͷ�ļ�������NDK�ı����������ĸ�ͷ�ļ���������������֤������Ҫ�ֶ���������ĸ�ͷ�ļ���
	    ��sdk\mpp\out\linux\multi-core\include��·���£�
		motionsensor_exe_ext.h
		MotionSensor_ext.h
		MotionSensor_cmd.h
		motionsensor_exe.h
	  
	���޸�ko���ؽű���
	  ����sdk\mpp\out\linux\multi-core\ko\load3559av100_multicore������������KO��ע��ȥ����Ȼ�������нű�
	  �����ֶ�������������KOҲ����
	  
	  insmod extdrv/motionsensor_chip.ko
	  insmod extdrv/motionsensor_mng.ko
	  insmod hi3559av100_motionfusion.ko
	  
	
